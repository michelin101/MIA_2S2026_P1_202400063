#include "reporte_disk.hpp"
#include "../../utilidades/ajustes.hpp"
#include <sstream>
#include <algorithm>
#include <cmath>

namespace reportes {

std::string generar_dot_disk(const MBR& mbr, const std::string& nombre_disco) {
    std::vector<ajustes::Hueco> ocupados;
    for (int i = 0; i < 4; i++) {
        if (mbr.mbr_partitions[i].part_s > 0) {
            ocupados.push_back({mbr.mbr_partitions[i].part_start, mbr.mbr_partitions[i].part_s});
        }
    }
    std::sort(ocupados.begin(), ocupados.end(),
              [](const ajustes::Hueco& a, const ajustes::Hueco& b) { return a.inicio < b.inicio; });

    std::ostringstream dot;
    dot << "digraph DISK {\n";
    dot << "  rankdir=LR;\n";
    dot << "  nodo_disk [shape=none, margin=0, label=<\n";
    dot << "  <table border=\"0\" cellborder=\"1\" cellspacing=\"0\">\n";
    dot << "    <tr><td colspan=\"20\">" << nombre_disco << "</td></tr>\n";
    dot << "    <tr>\n";

    double tam_total = mbr.mbr_tamano;

    // Bloque del MBR (siempre el primer segmento)
    double pct_mbr = (sizeof(MBR) / tam_total) * 100.0;
    dot << "      <td bgcolor=\"#D9D9D9\">MBR<br/>" << std::round(pct_mbr) << "%</td>\n";

    int cursor = sizeof(MBR);
    for (const auto& p : ocupados) {
        if (p.inicio > cursor) {
            double pct_libre = ((p.inicio - cursor) / tam_total) * 100.0;
            dot << "      <td bgcolor=\"#FFFFFF\">Libre<br/>" << std::round(pct_libre) << "%</td>\n";
        }
        double pct_part = (p.tamano / tam_total) * 100.0;
        dot << "      <td bgcolor=\"#C6E0B4\">Particion<br/>" << std::round(pct_part) << "%</td>\n";
        cursor = p.inicio + p.tamano;
    }
    if (cursor < mbr.mbr_tamano) {
        double pct_libre = ((mbr.mbr_tamano - cursor) / tam_total) * 100.0;
        dot << "      <td bgcolor=\"#FFFFFF\">Libre<br/>" << std::round(pct_libre) << "%</td>\n";
    }

    dot << "    </tr>\n";
    dot << "  </table>\n";
    dot << "  >];\n";
    dot << "}\n";

    return dot.str();
}

} 