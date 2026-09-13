#pragma once
#include <string>
#include "../../estructuras/superbloque.hpp"

namespace reportes {
    std::string generar_dot_sb(const SuperBloque& sb, const std::string& nombre_disco);
}