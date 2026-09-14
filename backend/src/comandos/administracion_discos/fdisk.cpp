#include "fdisk.hpp"
#include "../../estructuras/mbr.hpp"
#include "../../estructuras/ebr.hpp"
#include "../../utilidades/manejo_binario.hpp"
#include "../../utilidades/ajustes.hpp"
#include "../../utilidades/validaciones.hpp"
#include <cstring>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <cctype>

namespace comandos {

static std::string a_mayus(std::string s){
    for(auto& c : s) c = toupper(c);
    return s;
}

// contar cuantas particiones activas hay en el MBR y si existe una extendida
static void inspeccionar_mbr(const MBR& mbr, int& total_primarias_extendidas, bool& existe_extendida, int& indice_extendida) {
    total_primarias_extendidas = 0;
    existe_extendida = false;
    indice_extendida = -1;
    for (int i = 0; i < 4; i++) {
        if (mbr.mbr_partitions[i].part_s > 0) {
            total_primarias_extendidas++;
            if (mbr.mbr_partitions[i].part_type == 'E') {
                existe_extendida = true;
                indice_extendida = i;
            }
        }
    }
}

// crea una particion primaria o extendida
static std::string crear_primaria_o_extendida(MBR& mbr, const std::string& ruta, int tamano_bytes, char fit, const std::string& nombre, char tipo){
    int total, indice_ext;
    bool existe_ext;
    inspeccionar_mbr(mbr, total, existe_ext, indice_ext);

    if(total >= 4){
        return "ERROR: FDISK - ya existen 4 particiones en el disco, no se pueden crear mas";
    }
    if(tipo == 'E' && existe_ext){
        return "ERROR: FDISK - ya existe una particion extendida en este disco";
    }

    // valida nombre no repetido
    for(int i = 0; i < 4; i++){
        if (mbr.mbr_partitions[i].part_s > 0 && std::string(mbr.mbr_partitions[i].part_name) == nombre){
            return "ERROR: FDISK - ya existe una particion con el nombre '" + nombre + "' en este disco";
        }
    }

    // busca un slot libre en el arreglo de 4
    int slot_libre = -1;
    for (int i = 0; i < 4; i++) {
        if (mbr.mbr_partitions[i].part_s == 0) { 
            slot_libre = i; 
            break; 
        }
    }

    // calcula los huecos disponibles despues del MBR y hasta el fin del disco
    std::vector<ajustes::Hueco> ocupados;
    for (int i = 0; i < 4; i++) {
        if (mbr.mbr_partitions[i].part_s > 0) {
            ocupados.push_back({mbr.mbr_partitions[i].part_start, mbr.mbr_partitions[i].part_s});
        }
    }
    auto huecos = ajustes::calcular_huecos(sizeof(MBR), mbr.mbr_tamano - sizeof(MBR), ocupados);
    int inicio_elegido = ajustes::elegir_espacio(huecos, tamano_bytes, fit);

    if(inicio_elegido == -1){
        return "ERROR: FDISK - no hay espacio suficiente en el disco para esta parte";
    }

    // llenar la particion
    Partition& p = mbr.mbr_partitions[slot_libre];
    p.part_status = '0';       // no esta montada
    p.part_type = tipo;
    p.part_fit = fit;
    p.part_start = inicio_elegido;
    p.part_s = tamano_bytes;
    memset(p.part_name, 0, sizeof(p.part_name));
    strncpy(p.part_name, nombre.c_str(), sizeof(p.part_name) - 1);
    p.part_correlative = -1;
    memset(p.part_id, 0, sizeof(p.part_id));

    // si es extendida escribe el primer EBR
    if(tipo == 'E'){
        EBR primer_ebr;
        memset(&primer_ebr, 0, sizeof(EBR));
        primer_ebr.part_mount = '0';
        primer_ebr.part_fit = fit;
        primer_ebr.part_start = inicio_elegido;
        primer_ebr.part_s = 0; // no tiene logica creada
        primer_ebr.part_next = -1; // no hay siguiente EBR

        std::fstream archivo(ruta, std::ios::in | std::ios::out | std::ios::binary);
        archivo.seekp(inicio_elegido);
        archivo.write(reinterpret_cast<char*>(&primer_ebr), sizeof(EBR));
        archivo.close();
    }
    return "OK";
}

// crea una particion logica dentro de la extendida existente
static std::string crear_logica(const MBR& mbr, const std::string& ruta, int tamano_bytes, char fit, const std::string& nombre){
    int total, indice_ext;
    bool existe_ext;
    inspeccionar_mbr(mbr, total, existe_ext, indice_ext);

    if(!existe_ext){
                return "ERROR: FDISK - no existe una particion extendida en este disco, no se puede crear una logica";
    }

    const Partition& extendida = mbr.mbr_partitions[indice_ext];

    std::vector<ajustes::Hueco> ocupados;
    int byte_ebr_actual = extendida.part_start;
    int byte_ultimo_ebr = extendida.part_start;
    bool primer_ebr_vacio = true; 

    std::ifstream lectura(ruta, std::ios::binary);
    while(true){
        EBR actual;
        lectura.seekg(byte_ebr_actual);
        lectura.read(reinterpret_cast<char*>(&actual), sizeof(EBR));
        
        if(actual.part_s > 0){
            primer_ebr_vacio = false;
            if(std::string(actual.part_name) == nombre){
                lectura.close();
                return "ERROR: FDISK - ya existe una particion logica con el nombre '" + nombre + "' en el disco";
            }
            ocupados.push_back({actual.part_start, actual.part_s + (int)sizeof(EBR)});
        }

        byte_ultimo_ebr = byte_ebr_actual;
        if(actual.part_next == -1) break;
        byte_ebr_actual = actual.part_next;
    }
    lectura.close();

    // calcula huecos dentro del espacio total de la extendida
    auto huecos = ajustes::calcular_huecos(extendida.part_start, extendida.part_s, ocupados);

    int espacio_necesario = tamano_bytes + (primer_ebr_vacio ? 0 : (int)sizeof(EBR));
    int inicio_elegido = ajustes::elegir_espacio(huecos, espacio_necesario, fit);

    if(inicio_elegido == -1){
        return "ERROR: FDISK - no hay espacio suficiente dentro de la particion extendida";
    }

    if(primer_ebr_vacio){
        EBR nuevo;
        memset(&nuevo, 0, sizeof(EBR));
        nuevo.part_mount = '0';
        nuevo.part_fit = fit;
        nuevo.part_start = extendida.part_start + sizeof(EBR);
        nuevo.part_s = tamano_bytes;
        nuevo.part_next = -1;
        strncpy(nuevo.part_name, nombre.c_str(), 15);

        std::fstream escritura(ruta, std::ios::in | std::ios::out | std::ios::binary);
        escritura.seekp(extendida.part_start);
        escritura.write(reinterpret_cast<char*>(&nuevo), sizeof(EBR));
        escritura.close();
    }else{
        EBR nuevo;
        memset(&nuevo, 0, sizeof(EBR));
        nuevo.part_mount = '0';
        nuevo.part_fit = fit;
        nuevo.part_start = inicio_elegido + sizeof(EBR);
        nuevo.part_s = tamano_bytes;
        nuevo.part_next = -1;
        strncpy(nuevo.part_name, nombre.c_str(), 15);

        std::fstream escritura(ruta, std::ios::in | std::ios::out | std::ios::binary);
        // escribir el nuevo EBR en su posición
        escritura.seekp(inicio_elegido);
        escritura.write(reinterpret_cast<char*>(&nuevo), sizeof(EBR));

        // actualizar el EBR anterior para que apunte hacia el nuevo
        EBR ultimo;
        escritura.seekg(byte_ultimo_ebr);
        escritura.read(reinterpret_cast<char*>(&ultimo), sizeof(EBR));
        ultimo.part_next = inicio_elegido;
        escritura.seekp(byte_ultimo_ebr);
        escritura.write(reinterpret_cast<char*>(&ultimo), sizeof(EBR));
        escritura.close();
    }
    return "OK";
}

// Funcion principal del comando
std::string ejecutar_fdisk(const std::map<std::string, std::string>& parametros){
    auto error_param = validaciones::validar_parametros_permitidos(
        parametros, {"size", "unit", "path", "type", "fit", "name"}, "FDISK");
    if (!error_param.empty()) return error_param;
    
    // path (obligatorio)
    auto it_path = parametros.find("path");
    if(it_path == parametros.end()){
        return "ERROR: FDISK - el parametro -path es obligatorio";
    }
    std::string ruta = it_path->second;

    MBR mbr;
    if(!manejo_binario::leer_mbr(ruta, mbr)){
        return "ERROR: FDISK - el disco '" + ruta + "' no existe";
    }

    // name (obligatorio)
    auto it_name = parametros.find("name");
    if(it_name == parametros.end()){
        return "ERROR: FDISK - el parametro -name es obligatorio";
    }
    std::string nombre = it_name->second;

    // size (obligatorio)
    auto it_size = parametros.find("size");
    if(it_size == parametros.end()){
        return "ERROR: FDISK - el parametro -size es obligatorio";
    }
    long size_ingresado;
    try {
        size_ingresado = std::stol(it_size->second);
    }catch (...){
        return "ERROR: FDISK - el valor de -size no es numerico";
    }
    if(size_ingresado <= 0){
        return "ERROR: FDISK - -size debe ser positivo y mayor que cero";
    }

    // unit (opcional, default k)
    std::string unidad = "K";
    auto it_unit = parametros.find("unit");
    if(it_unit != parametros.end()){
        unidad = a_mayus(it_unit->second);
        if(unidad != "B" && unidad != "K" && unidad != "M"){
            return "ERROR: FDISK - -unit invalido, solo se permite B, K o M";
        }
    }
    long tamano_bytes = size_ingresado;
    if(unidad == "K") tamano_bytes *= 1024;
    else if(unidad == "M") tamano_bytes *= 1024*1024;

    // type (opcional, default P)
    std::string tipo_str = "P";
    auto it_type = parametros.find("type");
    if(it_type != parametros.end()){
        tipo_str = a_mayus(it_type->second);
        if(tipo_str != "P" && tipo_str != "E" && tipo_str != "L"){
            return "ERROR: FDISK - -type invalido, solo se permite P, E o L";
        }
    }
    char tipo = tipo_str[0];

    // fit (opcional, default WF)
    std::string fit_str = "WF";
    auto it_fit = parametros.find("fit");
    if(it_fit != parametros.end()){
        fit_str = a_mayus(it_fit->second);
        if(fit_str != "BF" && fit_str != "FF" && fit_str != "WF"){
            return "ERROR: FDISK - -fit invalido, solo se permite BF, FF o WF";
        }
    }
    char fit = fit_str[0];

    // segun el tipo
    if(tipo == 'L'){
        std::string resultado = crear_logica(mbr, ruta, (int)tamano_bytes, fit, nombre);
        if(resultado != "OK") return resultado;
        return "FDISK: particon logica '" + nombre + "' creada exitosamente";
    }else{
        std::string resultado = crear_primaria_o_extendida(mbr, ruta, (int)tamano_bytes, fit, nombre, tipo);
        if(resultado != "OK") return resultado;

        if(!manejo_binario::escribir_mbr(ruta, mbr)){
            return "ERROR: FDISK - no se pudo actualizar el MBR en el disco";
        }
        std::string tipo_texto = (tipo == 'P') ? "primaria" : "extendida";
        return "FDISK: particion " + tipo_texto + " '" + nombre + "' creada existosamente";
    }
}

}
