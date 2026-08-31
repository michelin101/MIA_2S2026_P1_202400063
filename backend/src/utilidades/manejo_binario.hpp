#pragma once
#include <string>
#include <fstream>

namespace manejo_binario {

// Crea las carpetas padres de una ruta si no existen 
bool crear_carpetas_padre(const std::string& ruta);

// Crea el archivo del disco lleno de ceros, tamaño exacto en bytes
bool crear_archivo_disco(const std::string& ruta, long tamano_bytes);

} 