#include "reporte_bitmap.hpp"
#include <fstream>

namespace reportes {

bool generar_reporte_bitmap(const std::string& ruta_disco, int byte_inicio, int cantidad, const std::string& ruta_salida) {
    std::ifstream disco(ruta_disco, std::ios::binary);
    if (!disco.is_open()) return false;
    disco.seekg(byte_inicio);

    std::ofstream salida(ruta_salida);
    if (!salida.is_open()) return false;

    for (int i = 0; i < cantidad; i++) {
        char bit;
        disco.read(&bit, 1);
        salida << bit;
        if ((i + 1) % 20 == 0) salida << "\n";
        else salida << " ";
    }
    salida << "\n";

    disco.close();
    salida.close();
    return true;
}

} 