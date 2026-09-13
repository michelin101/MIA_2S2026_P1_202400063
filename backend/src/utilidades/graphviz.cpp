#include "graphviz.hpp"
#include <fstream>
#include <cstdlib>

namespace graphviz {

static std::string obtener_extension(const std::string& ruta) {
    size_t punto = ruta.find_last_of('.');
    if (punto == std::string::npos) return "png"; 
    std::string ext = ruta.substr(punto + 1);
    return ext;
}

bool generar_imagen(const std::string& contenido_dot, const std::string& ruta_salida) {
    // escribe el contenido DOT en un archivo temporal
    std::string ruta_temporal = ruta_salida + ".dot";
    std::ofstream archivo_dot(ruta_temporal);
    if (!archivo_dot.is_open()) return false;
    archivo_dot << contenido_dot;
    archivo_dot.close();

    // construye el comando: dot -T<formato> entrada.dot -o salida.png
    std::string formato = obtener_extension(ruta_salida);
    std::string comando = "dot -T" + formato + " \"" + ruta_temporal + "\" -o \"" + ruta_salida + "\"";

    int resultado = system(comando.c_str());

    // limpia el archivo temporal .dot 
    std::remove(ruta_temporal.c_str());

    return resultado == 0;
}

}