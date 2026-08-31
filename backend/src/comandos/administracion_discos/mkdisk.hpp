#pragma once
#include <map>
#include <string>

namespace comandos {
    // Recibe los parametros y retorna el mensaje que se mostrara en el area de salida
    std::string ejecutar_mkdisk(const std::map<std::string, std::string>& parametros);
}