#include "reporte_file_ls.hpp"
#include "../../estructuras/inodo.hpp"
#include "../../estructuras/bloques.hpp"
#include <fstream>
#include <sstream>
#include <ctime>
#include "../../utilidades/texto_dot.hpp"

namespace reportes {

std::string generar_dot_file(const std::string& ruta_disco, const SuperBloque& sb, const std::string& nombre_archivo, const std::string& contenido) {
    std::ostringstream dot;
    dot << "digraph FILE {\n";
    dot << "  nodo_file [shape=none, margin=0, label=<\n";
    dot << "  <table border=\"1\" cellborder=\"1\" cellspacing=\"0\">\n";
    dot << "    <tr><td bgcolor=\"#4472C4\"><font color=\"white\">" << nombre_archivo << "</font></td></tr>\n";
    dot << "    <tr><td>" << texto_dot::sanear_para_html(contenido) << "</td></tr>\n";
    dot << "  </table>\n  >];\n}\n";
    return dot.str();
}

static std::string formatear_fecha(time_t t) {
    char buffer[64];
    struct tm* info = localtime(&t);
    strftime(buffer, sizeof(buffer), "%Y-%m-%d", info);
    return std::string(buffer);
}
static std::string formatear_hora(time_t t) {
    char buffer[64];
    struct tm* info = localtime(&t);
    strftime(buffer, sizeof(buffer), "%H:%M", info);
    return std::string(buffer);
}

std::string generar_dot_ls(const std::string& ruta_disco, const SuperBloque& sb, int indice_inodo_carpeta) {
    std::ifstream disco(ruta_disco, std::ios::binary);

    std::ostringstream dot;
    dot << "digraph LS {\n";
    dot << "  nodo_ls [shape=none, margin=0, label=<\n";
    dot << "  <table border=\"1\" cellborder=\"1\" cellspacing=\"0\">\n";
    dot << "    <tr><td bgcolor=\"#4472C4\"><font color=\"white\">Permisos</font></td>"
        << "<td bgcolor=\"#4472C4\"><font color=\"white\">UID</font></td>"
        << "<td bgcolor=\"#4472C4\"><font color=\"white\">GID</font></td>"
        << "<td bgcolor=\"#4472C4\"><font color=\"white\">Size</font></td>"
        << "<td bgcolor=\"#4472C4\"><font color=\"white\">Fecha</font></td>"
        << "<td bgcolor=\"#4472C4\"><font color=\"white\">Hora</font></td>"
        << "<td bgcolor=\"#4472C4\"><font color=\"white\">Tipo</font></td>"
        << "<td bgcolor=\"#4472C4\"><font color=\"white\">Name</font></td></tr>\n";

    Inodo carpeta;
    disco.seekg(sb.s_inode_start + indice_inodo_carpeta * sizeof(Inodo));
    disco.read(reinterpret_cast<char*>(&carpeta), sizeof(Inodo));

    for (int b = 0; b < 12; b++) {
        if (carpeta.i_block[b] == -1) continue;
        Bloque blo;
        disco.seekg(sb.s_block_start + carpeta.i_block[b] * sizeof(Bloque));
        disco.read(reinterpret_cast<char*>(&blo), sizeof(Bloque));

        for (int i = 0; i < 4; i++) {
            int idx_hijo = blo.carpeta.b_content[i].b_inodo;
            std::string nombre = blo.carpeta.b_content[i].b_name;
            if (idx_hijo == -1 || nombre == "." || nombre == "..") continue;

            Inodo hijo;
            disco.seekg(sb.s_inode_start + idx_hijo * sizeof(Inodo));
            disco.read(reinterpret_cast<char*>(&hijo), sizeof(Inodo));

            std::string tipo_texto = (hijo.i_type == '1') ? "Archivo" : "Carpeta";
            std::string permisos_texto = (hijo.i_type == '1' ? "-" : "d") +
                std::string("rw-rw-r--"); 

            dot << "    <tr><td>" << permisos_texto << "</td><td>" << hijo.i_uid << "</td><td>"
                << hijo.i_gid << "</td><td>" << hijo.i_s << "</td><td>"
                << formatear_fecha(hijo.i_ctime) << "</td><td>" << formatear_hora(hijo.i_ctime)
                << "</td><td>" << tipo_texto << "</td><td>" << nombre << "</td></tr>\n";
        }
    }

    dot << "  </table>\n  >];\n}\n";
    disco.close();
    return dot.str();
}

} 