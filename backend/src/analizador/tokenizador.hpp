#pragma once
#include <string>
#include <map>

namespace analizador {

struct ComandoParseado {
    std::string nombre_comando;               
    std::map<std::string, std::string> parametros; 
    bool error = false;
    std::string mensaje_error;
};

ComandoParseado parsear_linea(const std::string& linea);

}