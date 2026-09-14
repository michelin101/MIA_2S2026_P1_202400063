#pragma once
#include <map>
#include <set>
#include <string>

namespace validaciones {
    std::string validar_parametros_permitidos(const std::map<std::string, std::string>& parametros, const std::set<std::string>& permitidos, const std::string& nombre_comando);
}