#include "manejo_binario.hpp"
#include <sys/stat.h>
#include <cstring>
#include <cstdio>

namespace manejo_binario{
    
bool crear_carpetas_padre(const std::string& ruta){
    std::string acumulado;
    size_t pos = 0;
    while ((pos = ruta.find('/', pos + 1)) != std::string::npos){
        acumulado = ruta.substr(0, pos);
        if(!acumulado.empty()){
            mkdir(acumulado.c_str(), 0777);
        }
    }
    return true;
}

bool crear_archivo_disco(const std::string& ruta, long tamano_bytes){
    FILE* archivo = fopen(ruta.c_str(), "wb");
    if(archivo == nullptr){
        return false;
    }
    char buffer[1024];
    memset(buffer, 0, sizeof(buffer));

    long escrito = 0;
    while(escrito < tamano_bytes){
        long restante = tamano_bytes - escrito;
        long a_escribir = (restante < 1024) ? restante : 1024; 
        fwrite(buffer, 1, a_escribir, archivo);
        escrito += a_escribir;
    }

    fclose(archivo);
    return true;
}
}