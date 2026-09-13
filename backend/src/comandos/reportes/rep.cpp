#include "rep.hpp"
#include "reporte_mbr.hpp"
#include "reporte_disk.hpp"
#include "reporte_sb.hpp"
#include "reporte_bitmap.hpp"
#include "reporte_inode.hpp"
#include "reporte_block.hpp"
#include "reporte_tree.hpp"
#include "reporte_file_ls.hpp"
#include "../../sesion/particiones_montadas.hpp"
#include "../../utilidades/manejo_binario.hpp"
#include "../../utilidades/graphviz.hpp"
#include "../../utilidades/navegacion_directorios.hpp"
#include "../../utilidades/gestor_users_txt.hpp"
#include "../../estructuras/mbr.hpp"
#include <fstream>

namespace comandos{

std::string ejecutar_rep(const std::map<std::string, std::string>& parametros){
    auto it_name = parametros.find("name");
    auto it_path = parametros.find("path");
    auto it_id = parametros.find("id");

    if(it_name == parametros.end() || it_path == parametros.end() || it_id == parametros.end()){
        return "ERROR: REP - los parametros -name, -path e -id son obligatorios";
    }

    std::string tipo_reporte = it_name->second;
    std::string ruta_salida = it_path->second;
    std::string id = it_id->second;

    sesion::InfoMontaje info;
    if(!sesion::GestorMontajes::instancia().buscar_por_id(id, info)){
        return "ERROR: REP - no existe ninguna particion montada con el id '" + id + "'";
    }

    MBR mbr;
    if(!manejo_binario::leer_mbr(info.ruta_disco, mbr)){
        return "ERROR: REP - no se pudo leer el disco";
    }
    Partition& particion = mbr.mbr_partitions[info.indice_particion_en_mbr];

    // crea carpetas padre de la ruta de salida si no existen
    manejo_binario::crear_carpetas_padre(ruta_salida);

    if (tipo_reporte == "bm_inode" || tipo_reporte == "bm_block") {
        SuperBloque sb;
        std::ifstream archivo(info.ruta_disco, std::ios::binary);
        archivo.seekg(particion.part_start);
        archivo.read(reinterpret_cast<char*>(&sb), sizeof(SuperBloque));
        archivo.close();

        bool ok;
        if (tipo_reporte == "bm_inode") {
            ok = reportes::generar_reporte_bitmap(info.ruta_disco, sb.s_bm_inode_start, sb.s_inodes_count, ruta_salida);
        } else {
            ok = reportes::generar_reporte_bitmap(info.ruta_disco, sb.s_bm_block_start, sb.s_blocks_count, ruta_salida);
        }
        if (!ok) return "ERROR: REP - no se pudo generar el reporte " + tipo_reporte + ".";
        return "REP: reporte '" + tipo_reporte + "' generado exitosamente en " + ruta_salida;
    }

    std::string contenido_dot;

    if(tipo_reporte == "mbr"){
        contenido_dot = reportes::generar_dot_mbr(mbr);
    } else if (tipo_reporte == "disk") {
        size_t barra = info.ruta_disco.find_last_of('/');
        std::string nombre_disco = (barra == std::string::npos) ? info.ruta_disco : info.ruta_disco.substr(barra + 1);
        contenido_dot = reportes::generar_dot_disk(mbr, nombre_disco);
    } else if (tipo_reporte == "sb" || tipo_reporte == "inode" || tipo_reporte == "block" ||
               tipo_reporte == "tree" || tipo_reporte == "file" || tipo_reporte == "ls") {

        SuperBloque sb;
        std::ifstream archivo(info.ruta_disco, std::ios::binary);
        archivo.seekg(particion.part_start);
        archivo.read(reinterpret_cast<char*>(&sb), sizeof(SuperBloque));
        archivo.close();

        if (tipo_reporte == "sb") {
            size_t barra = info.ruta_disco.find_last_of('/');
            std::string nombre_disco = (barra == std::string::npos) ? info.ruta_disco : info.ruta_disco.substr(barra + 1);
            contenido_dot = reportes::generar_dot_sb(sb, nombre_disco);
        } else if (tipo_reporte == "inode") {
            contenido_dot = reportes::generar_dot_inode(info.ruta_disco, sb);
        } else if (tipo_reporte == "block") {
            contenido_dot = reportes::generar_dot_block(info.ruta_disco, sb);
        } else if (tipo_reporte == "tree") {
            contenido_dot = reportes::generar_dot_tree(info.ruta_disco, sb);
        } else if (tipo_reporte == "file" || tipo_reporte == "ls") {
            auto it_pfl = parametros.find("path_file_ls");
            if (it_pfl == parametros.end()) {
                return "ERROR: REP - el parametro -path_file_ls es obligatorio para el reporte " + tipo_reporte;
            }
            std::string ruta_objetivo = it_pfl->second;

            int indice_encontrado;

            if (ruta_objetivo == "/") {
                indice_encontrado = 0;
            } else {
                auto nav = navegacion::navegar_hasta_padre(info.ruta_disco, sb, ruta_objetivo, false, 0, 0);
                if (!nav.ok) return "ERROR: REP - " + nav.mensaje_error;

                indice_encontrado = navegacion::buscar_en_carpeta(info.ruta_disco, sb, nav.indice_inodo_padre, nav.nombre_final);
                if (indice_encontrado == -1) {
                    return "ERROR: REP - la ruta '" + ruta_objetivo + "' no existe.";
                }
            }

            if (tipo_reporte == "file") {
                std::string contenido_archivo = gestor_users::leer_contenido(info.ruta_disco, sb, indice_encontrado);
                contenido_dot = reportes::generar_dot_file(info.ruta_disco, sb, ruta_objetivo, contenido_archivo);
            } else {
                contenido_dot = reportes::generar_dot_ls(info.ruta_disco, sb, indice_encontrado);
            }
        }
    }else{
        return "ERROR: REP - el tipo de reporte '" + tipo_reporte + "' no esta implementado";
    }

    if(!graphviz::generar_imagen(contenido_dot, ruta_salida)){
        return "ERROR: REP - no se pudo generar la imagen con Graphviz";
    }

    return "REP: reporte '" + tipo_reporte + "' generado exitosamente en " + ruta_salida;
}

}