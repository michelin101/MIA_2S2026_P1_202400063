#include "reporte_tree.hpp"
#include "../../estructuras/inodo.hpp"
#include "../../estructuras/bloques.hpp"
#include <fstream>
#include <sstream>
#include <set>
#include "../../utilidades/texto_dot.hpp"

namespace reportes {

static Inodo leer_inodo(std::ifstream& disco, const SuperBloque& sb, int idx) {
    Inodo ino;
    disco.seekg(sb.s_inode_start + idx * sizeof(Inodo));
    disco.read(reinterpret_cast<char*>(&ino), sizeof(Inodo));
    return ino;
}

static void recorrer(std::ifstream& disco, const SuperBloque& sb, int indice_inodo, std::ostringstream& dot, std::set<int>& visitados) {
    if (visitados.count(indice_inodo)) return; 
    visitados.insert(indice_inodo);

    Inodo ino = leer_inodo(disco, sb, indice_inodo);

    dot << "  inodo" << indice_inodo << " [label=<\n";
    dot << "  <table border=\"1\" cellborder=\"1\" cellspacing=\"0\">\n";
    dot << "    <tr><td colspan=\"2\" bgcolor=\"#4472C4\"><font color=\"white\">Inodo " << indice_inodo << "</font></td></tr>\n";
    dot << "    <tr><td>i_type</td><td>" << ino.i_type << "</td></tr>\n";
    dot << "    <tr><td>i_s</td><td>" << ino.i_s << "</td></tr>\n";
    dot << "  </table>\n  >];\n";

    if (ino.i_type == '0') { 
        for (int b = 0; b < 12; b++) {
            if (ino.i_block[b] == -1) continue;
            int idx_bloque = ino.i_block[b];

            Bloque blo;
            disco.seekg(sb.s_block_start + idx_bloque * sizeof(Bloque));
            disco.read(reinterpret_cast<char*>(&blo), sizeof(Bloque));

            dot << "  bloque" << idx_bloque << " [label=<\n";
            dot << "  <table border=\"1\" cellborder=\"1\" cellspacing=\"0\">\n";
            dot << "    <tr><td colspan=\"2\" bgcolor=\"#8064A2\"><font color=\"white\">Bloque " << idx_bloque << "</font></td></tr>\n";
            for (int i = 0; i < 4; i++) {
                if (blo.carpeta.b_content[i].b_inodo == -1) continue;
                dot << "    <tr><td>" << std::string(blo.carpeta.b_content[i].b_name)
                    << "</td><td>" << blo.carpeta.b_content[i].b_inodo << "</td></tr>\n";
            }
            dot << "  </table>\n  >];\n";

            dot << "  inodo" << indice_inodo << " -> bloque" << idx_bloque << ";\n";

            for (int i = 0; i < 4; i++) {
                int hijo = blo.carpeta.b_content[i].b_inodo;
                std::string nombre = blo.carpeta.b_content[i].b_name;
                if (hijo == -1 || nombre == "." || nombre == "..") continue;

                dot << "  bloque" << idx_bloque << " -> inodo" << hijo << ";\n";
                recorrer(disco, sb, hijo, dot, visitados);
            }
        }
    } else { 
        for (int b = 0; b < 12; b++) {
            if (ino.i_block[b] == -1) continue;
            int idx_bloque = ino.i_block[b];

            BloqueArchivo ba;
            disco.seekg(sb.s_block_start + idx_bloque * sizeof(Bloque));
            disco.read(reinterpret_cast<char*>(&ba), sizeof(BloqueArchivo));

            dot << "  bloque" << idx_bloque << " [label=<\n";
            dot << "  <table border=\"1\" cellborder=\"1\" cellspacing=\"0\">\n";
            dot << "    <tr><td bgcolor=\"#C0504D\"><font color=\"white\">Bloque Archivo " << idx_bloque << "</font></td></tr>\n";
            dot << "    <tr><td>" << texto_dot::sanear_para_html(std::string(ba.b_content, 64)) << "</td></tr>\n";
            dot << "  </table>\n  >];\n";

            dot << "  inodo" << indice_inodo << " -> bloque" << idx_bloque << ";\n";
        }
    }
}

std::string generar_dot_tree(const std::string& ruta_disco, const SuperBloque& sb) {
    std::ifstream disco(ruta_disco, std::ios::binary);

    std::ostringstream dot;
    dot << "digraph TREE {\n";
    dot << "  node [shape=none, margin=0];\n";

    std::set<int> visitados;
    recorrer(disco, sb, 0, dot, visitados); 

    dot << "}\n";
    disco.close();
    return dot.str();
}

} 