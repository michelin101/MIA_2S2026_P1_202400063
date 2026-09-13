#include "reporte_mbr.hpp"
#include <sstream>
#include <ctime>

namespace reportes {

static std::string formatear_fecha(time_t t) {
    char buffer[64];
    struct tm* info = localtime(&t);
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M", info);
    return std::string(buffer);
}

std::string generar_dot_mbr(const MBR& mbr) {
    std::ostringstream dot;

    dot << "digraph MBR {\n";
    dot << "  nodo_mbr [shape=none, margin=0, label=<\n";
    dot << "    <table border=\"1\" cellborder=\"1\" cellspacing=\"0\">\n";
    dot << "      <tr><td colspan=\"2\" bgcolor=\"#4472C4\"><font color=\"white\">REPORTE DE MBR</font></td></tr>\n";
    dot << "      <tr><td>mbr_tamano</td><td>" << mbr.mbr_tamano << "</td></tr>\n";
    dot << "      <tr><td>mbr_fecha_creacion</td><td>" << formatear_fecha(mbr.mbr_fecha_creacion) << "</td></tr>\n";
    dot << "      <tr><td>mbr_dsk_signature</td><td>" << mbr.mbr_dsk_signature << "</td></tr>\n";
    dot << "      <tr><td>dsk_fit</td><td>" << mbr.dsk_fit << "</td></tr>\n";

    for (int i = 0; i < 4; i++) {
        const Partition& p = mbr.mbr_partitions[i];
        if (p.part_s == 0) continue; 

        std::string tipo_texto = (p.part_type == 'P') ? "Particion" : "Particion Extendida";
        std::string color = (p.part_type == 'P') ? "#8064A2" : "#C0504D";

        dot << "      <tr><td colspan=\"2\" bgcolor=\"" << color << "\"><font color=\"white\">"
            << tipo_texto << "</font></td></tr>\n";
        dot << "      <tr><td>part_status</td><td>" << p.part_status << "</td></tr>\n";
        dot << "      <tr><td>part_type</td><td>" << p.part_type << "</td></tr>\n";
        dot << "      <tr><td>part_fit</td><td>" << p.part_fit << "</td></tr>\n";
        dot << "      <tr><td>part_start</td><td>" << p.part_start << "</td></tr>\n";
        dot << "      <tr><td>part_s</td><td>" << p.part_s << "</td></tr>\n";
        dot << "      <tr><td>part_name</td><td>" << std::string(p.part_name) << "</td></tr>\n";
    }

    dot << "    </table>\n";
    dot << "  >];\n";
    dot << "}\n";

    return dot.str();
}

} 