#include "reporte_inode.hpp"
#include "../../estructuras/inodo.hpp"
#include <fstream>
#include <sstream>
#include <ctime>
#include <vector>

namespace reportes {

static std::string formatear_fecha(time_t t) {
    char buffer[64];
    struct tm* info = localtime(&t);
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M", info);
    return std::string(buffer);
}

std::string generar_dot_inode(const std::string& ruta_disco, const SuperBloque& sb) {
    std::ifstream disco(ruta_disco, std::ios::binary);

    std::ostringstream dot;
    dot << "digraph INODE {\n";
    dot << "  rankdir=LR;\n";
    dot << "  node [shape=none, margin=0];\n";

    std::vector<int> indices_usados;

    disco.seekg(sb.s_bm_inode_start);
    for (int i = 0; i < sb.s_inodes_count; i++) {
        char bit;
        disco.read(&bit, 1);
        if (bit == '1') indices_usados.push_back(i);
    }

    for (int idx : indices_usados) {
        Inodo ino;
        disco.seekg(sb.s_inode_start + idx * sizeof(Inodo));
        disco.read(reinterpret_cast<char*>(&ino), sizeof(Inodo));

        dot << "  inodo" << idx << " [label=<\n";
        dot << "  <table border=\"1\" cellborder=\"1\" cellspacing=\"0\">\n";
        dot << "    <tr><td colspan=\"2\" bgcolor=\"#4472C4\"><font color=\"white\">Inodo " << idx << "</font></td></tr>\n";
        dot << "    <tr><td>i_uid</td><td>" << ino.i_uid << "</td></tr>\n";
        dot << "    <tr><td>i_gid</td><td>" << ino.i_gid << "</td></tr>\n";
        dot << "    <tr><td>i_s</td><td>" << ino.i_s << "</td></tr>\n";
        dot << "    <tr><td>i_atime</td><td>" << formatear_fecha(ino.i_atime) << "</td></tr>\n";
        dot << "    <tr><td>i_ctime</td><td>" << formatear_fecha(ino.i_ctime) << "</td></tr>\n";
        dot << "    <tr><td>i_mtime</td><td>" << formatear_fecha(ino.i_mtime) << "</td></tr>\n";
        for (int b = 0; b < 15; b++) {
            dot << "    <tr><td>i_block[" << b << "]</td><td>" << ino.i_block[b] << "</td></tr>\n";
        }
        dot << "    <tr><td>i_type</td><td>" << ino.i_type << "</td></tr>\n";
        dot << "    <tr><td>i_perm</td><td>" << std::string(ino.i_perm, 3) << "</td></tr>\n";
        dot << "  </table>\n  >];\n";
    }

    for (size_t i = 0; i + 1 < indices_usados.size(); i++) {
        dot << "  inodo" << indices_usados[i] << " -> inodo" << indices_usados[i + 1] << ";\n";
    }

    dot << "}\n";
    disco.close();
    return dot.str();
}

} 