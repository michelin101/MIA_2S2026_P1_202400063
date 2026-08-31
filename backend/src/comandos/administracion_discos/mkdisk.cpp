// backend/src/comandos/administracion_discos/mkdisk.cpp
#include "mkdisk.hpp"
#include "../../utilidades/manejo_binario.hpp"
#include "../../estructuras/mbr.hpp"
#include <cstring>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <sstream>

namespace comandos {

std::string ejecutar_mkdisk(const std::map<std::string, std::string>& parametros) {
    // validad -path
    auto it_path = parametros.find("path");
    if (it_path == parametros.end()) {
        return "ERROR: MKDISK - el parámetro -path es obligatorio";
    }
    std::string ruta = it_path->second;

    // validar -size (es obligatorio, positivo)
    auto it_size = parametros.find("size");
    if (it_size == parametros.end()) {
        return "ERROR: MKDISK - el parámetro -size es obligatorio";
    }
    long size_ingresado = 0;
    try {
        size_ingresado = std::stol(it_size->second);
    } catch (...) {
        return "ERROR: MKDISK - el valor de -size no es un número válido";
    }
    if (size_ingresado <= 0) {
        return "ERROR: MKDISK - -size debe ser un número positivo mayor a cero";
    }

    // validar -unit (opcional, default M)
    std::string unidad = "M";
    auto it_unit = parametros.find("unit");
    if (it_unit != parametros.end()) {
        unidad = it_unit->second;
        for (auto& c : unidad) c = toupper(c);
        if (unidad != "K" && unidad != "M") {
            return "ERROR: MKDISK - -unit inválido. Solo se permite K o M";
        }
    }

    // validar -fit (opcional, default FF)
    std::string fit = "FF";
    auto it_fit = parametros.find("fit");
    if (it_fit != parametros.end()) {
        fit = it_fit->second;
        for (auto& c : fit) c = toupper(c);
        if (fit != "BF" && fit != "FF" && fit != "WF") {
            return "ERROR: MKDISK - -fit inválido. Solo se permite BF, FF o WF";
        }
    }
    char fit_char = fit[0]; // 'B', 'F' o 'W'

    // calcular el tamaño en bytes
    long tamano_bytes = size_ingresado;
    if (unidad == "K") tamano_bytes *= 1024;
    else tamano_bytes *= 1024 * 1024; // M

     // crear carpetas padre si no existen, y crear el archivo lleno de ceros
    manejo_binario::crear_carpetas_padre(ruta);
    if (!manejo_binario::crear_archivo_disco(ruta, tamano_bytes)) {
        return "ERROR: MKDISK - no se pudo crear el archivo en la ruta indicada";
    }

    // construir y escribir el MBR en el primer sector
    MBR mbr;
    memset(&mbr, 0, sizeof(MBR)); 

    mbr.mbr_tamano = static_cast<int>(tamano_bytes);
    mbr.mbr_fecha_creacion = time(nullptr);
    mbr.mbr_dsk_signature = rand(); // numero random que identifica el disco
    mbr.dsk_fit = fit_char;

    // las 4 particiones inician vacias
    for (int i = 0; i < 4; i++) {
        mbr.mbr_partitions[i].part_status = '0';
        mbr.mbr_partitions[i].part_correlative = -1;
        mbr.mbr_partitions[i].part_start = -1;
        mbr.mbr_partitions[i].part_s = 0;
    }

   // abre el archivo creado y escribe el MBR exactamente en el byte 0
    std::fstream archivo(ruta, std::ios::in | std::ios::out | std::ios::binary);
    if (!archivo.is_open()) {
        return "ERROR: MKDISK - no se pudo abrir el disco para escribir el MBR";
    }
    archivo.seekp(0);
    archivo.write(reinterpret_cast<char*>(&mbr), sizeof(MBR));
    archivo.close();

    // exito
    std::ostringstream salida;
    salida << "MKDISK: disco creado exitosamente en " << ruta
           << " (" << tamano_bytes << " bytes, fit=" << fit << ")";
    return salida.str();
}

} 




