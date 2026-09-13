#include "reporte_sb.hpp"
#include <sstream>
#include <ctime>
#include <iomanip>

namespace reportes {

static std::string formatear_fecha(time_t t) {
    if (t == 0) return "N/A";
    char buffer[64];
    struct tm* info = localtime(&t);
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M", info);
    return std::string(buffer);
}

std::string generar_dot_sb(const SuperBloque& sb, const std::string& nombre_disco) {
    std::ostringstream dot;
    dot << "digraph SB {\n";
    dot << "  nodo_sb [shape=none, margin=0, label=<\n";
    dot << "  <table border=\"1\" cellborder=\"1\" cellspacing=\"0\">\n";
    dot << "    <tr><td colspan=\"2\" bgcolor=\"#1F6B3C\"><font color=\"white\">REPORTE DE SUPERBLOQUE</font></td></tr>\n";

    auto fila = [&](const std::string& nombre, const std::string& valor) {
        dot << "    <tr><td>" << nombre << "</td><td>" << valor << "</td></tr>\n";
    };

    fila("s_filesystem_type", std::to_string(sb.s_filesystem_type));
    fila("s_inodes_count", std::to_string(sb.s_inodes_count));
    fila("s_blocks_count", std::to_string(sb.s_blocks_count));
    fila("s_free_blocks_count", std::to_string(sb.s_free_blocks_count));
    fila("s_free_inodes_count", std::to_string(sb.s_free_inodes_count));
    fila("s_mtime", formatear_fecha(sb.s_mtime));
    fila("s_umtime", formatear_fecha(sb.s_umtime));
    fila("s_mnt_count", std::to_string(sb.s_mnt_count));
    std::ostringstream hex_magic;
    hex_magic << "0x" << std::hex << sb.s_magic;
    fila("s_magic", hex_magic.str());
    fila("s_inode_s", std::to_string(sb.s_inode_s));
    fila("s_block_s", std::to_string(sb.s_block_s));
    fila("s_firts_ino", std::to_string(sb.s_firts_ino));
    fila("s_first_blo", std::to_string(sb.s_first_blo));
    fila("s_bm_inode_start", std::to_string(sb.s_bm_inode_start));
    fila("s_bm_block_start", std::to_string(sb.s_bm_block_start));
    fila("s_inode_start", std::to_string(sb.s_inode_start));
    fila("s_block_start", std::to_string(sb.s_block_start));

    dot << "  </table>\n  >];\n}\n";
    return dot.str();
}

} 