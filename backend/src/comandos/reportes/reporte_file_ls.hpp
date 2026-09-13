#pragma once
#include <string>
#include "../../estructuras/superbloque.hpp"

namespace reportes {
    // Muestra nombre y contenido completo de un archivo
    std::string generar_dot_file(const std::string& ruta_disco, const SuperBloque& sb, const std::string& nombre_archivo, const std::string& contenido);

    // Muestra una tabla de permisos/propietario/tamaño/fecha/tipo/nombre de todos los elementos dentro de una carpeta dada.
    std::string generar_dot_ls(const std::string& ruta_disco, const SuperBloque& sb, int indice_inodo_carpeta);
}