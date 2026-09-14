#include "validaciones.hpp"

namespace validaciones {
std::string validar_parametros_permitidos(const std::map<std::string, std::string>& parametros, const std::set<std::string>& permitidos, const std::string& nombre_comando) {
    for (const auto& par : parametros) {
        if (permitidos.find(par.first) == permitidos.end()) {
            return "ERROR: " + nombre_comando + " - el parametro '-" + par.first + "' no es reconocido";
        }
    }
    return "";
}
}