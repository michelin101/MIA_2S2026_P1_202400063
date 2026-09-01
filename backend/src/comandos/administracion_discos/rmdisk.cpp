#include "rmdisk.hpp"
#include <cstdio>
#include <fstream>

namespace comandos {

std::string ejecutar_rmdisk(const std::map<std::string, std::string>& parametros){
    auto it_path = parametros.find("path");
    if(it_path == parametros.end()){
        return "ERROR: RMDISK - el parametro -path es obligatorio";
    }
    std::string ruta = it_path->second;

    // verificar que existe antes de borrarlo
    std::ifstream prueba(ruta);
    if(!prueba.good()){
        return "ERROR: RMDISK - el archivo '" + ruta + "' no existe";
    }
    prueba.close();

    if(std::remove(ruta.c_str()) != 0){
        return "ERROR: RMDISK - no se pudo eliminar el archivo";
    }
    return "RMDISK: disco '" + ruta + "' eliminado exitosamente";
}

}
