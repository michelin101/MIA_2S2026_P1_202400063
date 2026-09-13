#pragma once
#include <string>
#include "../../estructuras/mbr.hpp"

namespace reportes {
    std::string generar_dot_disk(const MBR& mbr, const std::string& nombre_disco);
}