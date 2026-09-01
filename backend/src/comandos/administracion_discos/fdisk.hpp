#pragma once
#include <map>
#include <string>

namespace comandos {
    std::string ejecutar_fdisk(const std::map<std::string, std::string>& parametros);
}