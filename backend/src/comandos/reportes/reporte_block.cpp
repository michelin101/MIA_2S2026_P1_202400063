#include "reporte_block.hpp"
#include "../../estructuras/bloques.hpp"
#include "../../estructuras/inodo.hpp"
#include "../../utilidades/texto_dot.hpp"
#include <fstream>
#include <sstream>
#include <vector>

namespace reportes {

std::string generar_dot_block(const std::string& ruta_disco, const SuperBloque& sb) {
    std::ifstream disco(ruta_disco, std::ios::binary);

    std::ostringstream dot;
    dot << "digraph BLOCK {\n";
    dot << "  rankdir=LR;\n";
    dot << "  node [shape=none, margin=0];\n";

    std::vector<char> tipo_por_bloque(sb.s_blocks_count, '?');
    std::vector<std::vector<int>> bloques_por_inodo; // secuencia ordenada por inodo

    disco.seekg(sb.s_bm_inode_start);
    for (int i = 0; i < sb.s_inodes_count; i++) {
        char bit;
        disco.read(&bit, 1);
        if (bit != '1') continue;

        Inodo ino;
        disco.seekg(sb.s_inode_start + i * sizeof(Inodo));
        disco.read(reinterpret_cast<char*>(&ino), sizeof(Inodo));

        std::vector<int> secuencia;
        for (int b = 0; b < 12; b++) {
            if (ino.i_block[b] != -1) {
                tipo_por_bloque[ino.i_block[b]] = ino.i_type;
                secuencia.push_back(ino.i_block[b]);
            }
        }
        if (secuencia.size() > 1) bloques_por_inodo.push_back(secuencia);

        disco.seekg(sb.s_bm_inode_start + i + 1);
    }

    std::vector<int> indices_usados;
    disco.seekg(sb.s_bm_block_start);
    for (int i = 0; i < sb.s_blocks_count; i++) {
        char bit;
        disco.read(&bit, 1);
        if (bit == '1') indices_usados.push_back(i);
    }

    for (int idx : indices_usados) {
        char tipo = tipo_por_bloque[idx];

        dot << "  bloque" << idx << " [label=<\n";
        dot << "  <table border=\"1\" cellborder=\"1\" cellspacing=\"0\">\n";

        if (tipo == '0') {
            BloqueCarpeta bc;
            disco.seekg(sb.s_block_start + idx * sizeof(Bloque));
            disco.read(reinterpret_cast<char*>(&bc), sizeof(BloqueCarpeta));

            dot << "    <tr><td colspan=\"2\" bgcolor=\"#8064A2\"><font color=\"white\">Bloque Carpeta " << idx << "</font></td></tr>\n";
            dot << "    <tr><td>b_name</td><td>b_inodo</td></tr>\n";
            for (int i = 0; i < 4; i++) {
                if (bc.b_content[i].b_inodo == -1) continue;
                dot << "    <tr><td>" << texto_dot::sanear_para_html(std::string(bc.b_content[i].b_name, 12))
                    << "</td><td>" << bc.b_content[i].b_inodo << "</td></tr>\n";
            }
        } else {
            BloqueArchivo ba;
            disco.seekg(sb.s_block_start + idx * sizeof(Bloque));
            disco.read(reinterpret_cast<char*>(&ba), sizeof(BloqueArchivo));

            dot << "    <tr><td bgcolor=\"#C0504D\"><font color=\"white\">Bloque Archivo " << idx << "</font></td></tr>\n";
            dot << "    <tr><td>" << texto_dot::sanear_para_html(std::string(ba.b_content, 64)) << "</td></tr>\n";
        }

        dot << "  </table>\n  >];\n";
    }

    for (size_t i = 0; i + 1 < indices_usados.size(); i++) {
        dot << "  bloque" << indices_usados[i] << " -> bloque" << indices_usados[i + 1] << " [style=invis];\n";
    }

    for (const auto& secuencia : bloques_por_inodo) {
        for (size_t i = 0; i + 1 < secuencia.size(); i++) {
            dot << "  bloque" << secuencia[i] << " -> bloque" << secuencia[i + 1] << ";\n";
        }
    }

    dot << "}\n";
    disco.close();
    return dot.str();
}

} 