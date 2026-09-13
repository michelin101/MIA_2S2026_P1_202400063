#pragma once
#include <string>
#include "../../estructuras/superbloque.hpp"

namespace reportes {
    std::string generar_dot_block(const std::string& ruta_disco, const SuperBloque& sb);
}