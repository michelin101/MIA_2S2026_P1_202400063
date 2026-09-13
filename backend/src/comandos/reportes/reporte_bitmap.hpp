#pragma once
#include <string>

namespace reportes {
    bool generar_reporte_bitmap(const std::string& ruta_disco, int byte_inicio, int cantidad, const std::string& ruta_salida);
}