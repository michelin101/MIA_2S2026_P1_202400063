#include "mounted.hpp"
#include "../../sesion/particiones_montadas.hpp"
#include <sstream>

namespace comandos{

std::string ejecutar_mounted(const std::map<std::string, std::string>& parametros){
    auto ids = sesion::GestorMontajes::instancia().listar_ids();

    if(ids.empty()){
        return "MOUNTED: no hay particiones montadas actualmente";
    }

    std::ostringstream salida;
    salida << "MOUNTED: ";
    for(size_t i = 0; i < ids.size(); i++){
        salida << ids[i];
        if(i != ids.size() - 1) salida << ", "; 
    }
    return salida.str();
}
}