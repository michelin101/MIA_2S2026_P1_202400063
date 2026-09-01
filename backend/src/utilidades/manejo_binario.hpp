#pragma once
#include <string>
#include <fstream>
#include "../estructuras/mbr.hpp"

namespace manejo_binario {

// Crea las carpetas padres de una ruta si no existen 
bool crear_carpetas_padre(const std::string& ruta);

// Crea el archivo del disco lleno de ceros, tamaño exacto en bytes
bool crear_archivo_disco(const std::string& ruta, long tamano_bytes);

// Lee el MBR desde el byte 0 del disco
bool leer_mbr(const std::string& ruta, MBR& mbr_salida);

// Escribe el MBR de vuelta al byte 0 del disco
bool escribir_mbr(const std::string& ruta, const MBR& mbr);

} 