// backend/src/comandos/sistema_archivos/cat.cpp
#include "cat.hpp"
#include "../../utilidades/contexto_particion.hpp"
#include "../../utilidades/gestor_users_txt.hpp"
#include "../../utilidades/navegacion_directorios.hpp"
#include <vector>
#include <algorithm>
#include <sstream>

namespace comandos{

std::string ejecutar_cat(const std::map<std::string, std::string>& parametros) {
    auto ctx = contexto::obtener_contexto_sesion_activa();
    if (!ctx.ok) return ctx.mensaje_error;

    std::vector<std::pair<int, std::string>> archivos_ordenados;
    for (const auto& par : parametros) {
        if (par.first.rfind("file", 0) == 0) { 
            std::string numero_str = par.first.substr(4);
            try {
                int numero = std::stoi(numero_str);
                archivos_ordenados.push_back({numero, par.second});
            } catch (...) {
            }
        }
    }

    if (archivos_ordenados.empty()) {
        return "ERROR: CAT - debe especificar al menos un archivo con -file1, -file2, etc.";
    }

    std::sort(archivos_ordenados.begin(), archivos_ordenados.end());

    std::ostringstream resultado;
    for (size_t i = 0; i < archivos_ordenados.size(); i++) {
        std::string ruta_completa = archivos_ordenados[i].second;

        auto nav = navegacion::navegar_hasta_padre(ctx.ruta_disco, ctx.sb, ruta_completa, false, 0, 0);
        if (!nav.ok) {
            return "ERROR: CAT - el archivo '" + ruta_completa + "' no existe (ruta invalida)";
        }

        int indice_inodo = navegacion::buscar_en_carpeta(ctx.ruta_disco, ctx.sb, nav.indice_inodo_padre, nav.nombre_final);
        if (indice_inodo == -1) {
            return "ERROR: CAT - el archivo '" + ruta_completa + "' no existe";
        }

        std::string contenido = gestor_users::leer_contenido(ctx.ruta_disco, ctx.sb, indice_inodo);
        resultado << contenido;
        if (i != archivos_ordenados.size() - 1) resultado << "\n";
    }

    return resultado.str();
}

} 