#include "navegacion_directorios.hpp"
#include "../estructuras/inodo.hpp"
#include "../estructuras/bloques.hpp"
#include "bitmap.hpp"
#include <fstream>
#include <sstream>
#include <cstring>
#include <ctime>

namespace navegacion{

std::vector<std::string> separar_ruta(const std::string& ruta){
    std::vector<std::string> segmentos;
    std::stringstream flujo(ruta);
    std::string segmento;
    while (std::getline(flujo, segmento, '/')) {
        if (!segmento.empty()) segmentos.push_back(segmento);
    }
    return segmentos;
}

static Inodo leer_inodo(const std::string& ruta, const SuperBloque& sb, int indice){
    Inodo ino;
    std::ifstream archivo(ruta, std::ios::binary);
    archivo.seekg(sb.s_inode_start + indice * sizeof(Inodo));
    archivo.read(reinterpret_cast<char*>(&ino), sizeof(Inodo));
    archivo.close();
    return ino;
}

static void escribir_inodo(const std::string& ruta, const SuperBloque& sb, int indice, const Inodo& ino){
    std::fstream archivo(ruta, std::ios::in | std::ios::out | std::ios::binary);
    archivo.seekp(sb.s_inode_start + indice * sizeof(Inodo));
    archivo.write(reinterpret_cast<const char*>(&ino), sizeof(Inodo));
    archivo.close();
}

static Bloque leer_bloque(const std::string& ruta, const SuperBloque& sb, int indice){
    Bloque blo;
    std::ifstream archivo(ruta, std::ios::binary);
    archivo.seekg(sb.s_block_start + indice * sizeof(Bloque));
    archivo.read(reinterpret_cast<char*>(&blo), sizeof(Bloque));
    archivo.close();
    return blo;
}

static void escribir_bloque(const std::string& ruta, const SuperBloque& sb, int indice, const Bloque& blo){
    std::fstream archivo(ruta, std::ios::in | std::ios::out | std::ios::binary);
    archivo.seekp(sb.s_block_start + indice * sizeof(Bloque));
    archivo.write(reinterpret_cast<const char*>(&blo), sizeof(Bloque));
    archivo.close();
}

int buscar_en_carpeta(const std::string& ruta_disco, const SuperBloque& sb, int indice_inodo_carpeta, const std::string& nombre){
    Inodo carpeta = leer_inodo(ruta_disco, sb, indice_inodo_carpeta);
    for (int b = 0; b < 12; b++) {
        if (carpeta.i_block[b] == -1) continue;
        Bloque bloque = leer_bloque(ruta_disco, sb, carpeta.i_block[b]);
        for (int i = 0; i < 4; i++) {
            if (bloque.carpeta.b_content[i].b_inodo != -1 &&
                std::string(bloque.carpeta.b_content[i].b_name) == nombre) {
                return bloque.carpeta.b_content[i].b_inodo;
            }
        }
    }
    return -1;
}

bool agregar_entrada_carpeta(const std::string& ruta_disco, const SuperBloque& sb, int indice_inodo_carpeta, const std::string& nombre, int indice_inodo_nuevo){
    Inodo carpeta = leer_inodo(ruta_disco, sb, indice_inodo_carpeta);
    // buscar un slot vacio en un bloque directa ya existente
    for(int b = 0; b < 12; b++){
        if(carpeta.i_block[b] == -1) continue;
        Bloque bloque = leer_bloque(ruta_disco, sb, carpeta.i_block[b]);
        for(int i = 0; i < 4; i++){
            if(bloque.carpeta.b_content[i].b_inodo == -1){
                memset(bloque.carpeta.b_content[i].b_name, 0, 12);
                strncpy(bloque.carpeta.b_content[i].b_name, nombre.c_str(), 11);
                bloque.carpeta.b_content[i].b_inodo = indice_inodo_nuevo;
                escribir_bloque(ruta_disco, sb, carpeta.i_block[b], bloque);
                return true;
            }
        }
    }

    // si no hay slot libre en bloques existentes, reserva un bloque directo nuevo
    for(int b = 0; b < 12; b++){
        if(carpeta.i_block[b] != -1) continue; // ya esta usado, sigue buscando uno libre

        int nuevo_indice_bloque = bitmap::ocupar_primera_libre(ruta_disco, sb.s_bm_block_start, sb.s_blocks_count);
        if (nuevo_indice_bloque == -1) return false; // disco lleno

        Bloque bloque_nuevo;
        memset(&bloque_nuevo, 0, sizeof(Bloque));
        for (int i = 0; i < 4; i++) bloque_nuevo.carpeta.b_content[i].b_inodo = -1;

        strncpy(bloque_nuevo.carpeta.b_content[0].b_name, nombre.c_str(), 11);
        bloque_nuevo.carpeta.b_content[0].b_inodo = indice_inodo_nuevo;

        escribir_bloque(ruta_disco, sb, nuevo_indice_bloque, bloque_nuevo);

        carpeta.i_block[b] = nuevo_indice_bloque;
        escribir_inodo(ruta_disco, sb, indice_inodo_carpeta, carpeta);
        return true;
    }
    return false; // se acabaron los 12 bloqes directos 
}

// crea una nueva carpeta  enlaza dentro de su padre
static int crear_carpeta_nueva(const std::string& ruta_disco, SuperBloque& sb, int indice_inodo_padre, const std::string& nombre, int uid, int gid) {
    int nuevo_indice_inodo = bitmap::ocupar_primera_libre(ruta_disco, sb.s_bm_inode_start, sb.s_inodes_count);
    int nuevo_indice_bloque = bitmap::ocupar_primera_libre(ruta_disco, sb.s_bm_block_start, sb.s_blocks_count);
    if (nuevo_indice_inodo == -1 || nuevo_indice_bloque == -1) return -1;

    Inodo nueva_carpeta;
    memset(&nueva_carpeta, 0, sizeof(Inodo));
    nueva_carpeta.i_uid = uid;
    nueva_carpeta.i_gid = gid;
    nueva_carpeta.i_s = 0;
    nueva_carpeta.i_atime = nueva_carpeta.i_ctime = nueva_carpeta.i_mtime = time(nullptr);
    for (int i = 0; i < 15; i++) nueva_carpeta.i_block[i] = -1;
    nueva_carpeta.i_block[0] = nuevo_indice_bloque;
    nueva_carpeta.i_type = '0'; // carpeta
    strncpy(nueva_carpeta.i_perm, "664", 3);

    Bloque bloque_nuevo;
    memset(&bloque_nuevo, 0, sizeof(Bloque));
    strncpy(bloque_nuevo.carpeta.b_content[0].b_name, ".", 12);
    bloque_nuevo.carpeta.b_content[0].b_inodo = nuevo_indice_inodo; // se apunta a si misma
    strncpy(bloque_nuevo.carpeta.b_content[1].b_name, "..", 12);
    bloque_nuevo.carpeta.b_content[1].b_inodo = indice_inodo_padre; // apunta a su padre real
    bloque_nuevo.carpeta.b_content[2].b_inodo = -1;
    bloque_nuevo.carpeta.b_content[3].b_inodo = -1;

    escribir_inodo(ruta_disco, sb, nuevo_indice_inodo, nueva_carpeta);
    escribir_bloque(ruta_disco, sb, nuevo_indice_bloque, bloque_nuevo);

    agregar_entrada_carpeta(ruta_disco, sb, indice_inodo_padre, nombre, nuevo_indice_inodo);

    return nuevo_indice_inodo;
}

ResultadoNavegacion navegar_hasta_padre(const std::string& ruta_disco, SuperBloque& sb, const std::string& ruta_completa, bool crear_intermedias, int uid_actual, int gid_actual) {
    ResultadoNavegacion resultado;

    auto segmentos = separar_ruta(ruta_completa);
    if (segmentos.empty()) {
        resultado.mensaje_error = "ERROR: la ruta no puede estar vacia";
        return resultado;
    }

    resultado.nombre_final = segmentos.back();
    segmentos.pop_back(); 

    int indice_actual = 0; // empezamos en la raiz (inodo 0)

    for (const auto& segmento : segmentos) {
        int encontrado = buscar_en_carpeta(ruta_disco, sb, indice_actual, segmento);

        if (encontrado == -1) {
            if (!crear_intermedias) {
                resultado.mensaje_error = "ERROR: la carpeta '" + segmento + "' no existe en la ruta. Use -r o -p para crearla automaticamente";
                return resultado;
            }
            encontrado = crear_carpeta_nueva(ruta_disco, sb, indice_actual, segmento, uid_actual, gid_actual);
            if (encontrado == -1) {
                resultado.mensaje_error = "ERROR: no se pudo crear la carpeta intermedia '" + segmento + "' (disco lleno)";
                return resultado;
            }
        }
        indice_actual = encontrado;
    }

    resultado.ok = true;
    resultado.indice_inodo_padre = indice_actual;
    return resultado;
}

}