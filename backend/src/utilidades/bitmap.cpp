#include "bitmap.hpp"
#include <fstream>

namespace bitmap{

void inicializar(const std::string& ruta_disco, int byte_inicio, int cantidad) {
    std::fstream archivo(ruta_disco, std::ios::in | std::ios::out | std::ios::binary);
    archivo.seekp(byte_inicio);
    for (int i = 0; i < cantidad; i++) {
        char bit = '0';
        archivo.write(&bit, 1);
    }
    archivo.close();
}

int ocupar_primera_libre(const std::string& ruta_disco, int byte_inicio, int cantidad) {
    std::fstream archivo(ruta_disco, std::ios::in | std::ios::out | std::ios::binary);

    for (int i = 0; i < cantidad; i++) {
        char bit;
        archivo.seekg(byte_inicio + i);
        archivo.read(&bit, 1);
        if (bit == '0') {
            // la marcamos como ocupada y retornamos su índice
            char ocupado = '1';
            archivo.seekp(byte_inicio + i);
            archivo.write(&ocupado, 1);
            archivo.close();
            return i;
        }
    }

    archivo.close();
    return -1; // no hay espacio libre
}

void liberar_posicion(const std::string& ruta_disco, int byte_inicio, int indice) {
    std::fstream archivo(ruta_disco, std::ios::in | std::ios::out | std::ios::binary);
    char bit = '0';
    archivo.seekp(byte_inicio + indice);
    archivo.write(&bit, 1);
    archivo.close();
}

}