// backend/src/comandos/archivos_carpetas/mkdir.cpp
#include "mkdir.hpp"
#include "../../sesion/sesion_usuario.hpp"
#include "../../utilidades/contexto_particion.hpp"
#include "../../utilidades/navegacion_directorios.hpp"
#include "../../estructuras/inodo.hpp"
#include "../../estructuras/bloques.hpp"
#include "../../utilidades/bitmap.hpp"
#include <fstream>
#include <cstring>
#include <ctime>

namespace comandos {

std::string ejecutar_mkdir(const std::map<std::string, std::string>& parametros) {
    auto it_path = parametros.find("path");
    if (it_path == parametros.end()) {
        return "ERROR: MKDIR - el parametro -path es obligatorio";
    }
    std::string ruta = it_path->second;

    if (parametros.count("p") && !parametros.at("p").empty()) {
        return "ERROR: MKDIR - el parametro -p no debe recibir ningun valor";
    }
    bool crear_intermedias = parametros.count("p") > 0;

    auto ctx = contexto::obtener_contexto_sesion_activa();
    if (!ctx.ok) return ctx.mensaje_error;

    int uid = sesion::SesionUsuario::instancia().uid_actual();
    int gid = sesion::SesionUsuario::instancia().gid_actual();

    auto nav = navegacion::navegar_hasta_padre(ctx.ruta_disco, ctx.sb, ruta, crear_intermedias, uid, gid);
    if (!nav.ok) return nav.mensaje_error;

    int existente = navegacion::buscar_en_carpeta(ctx.ruta_disco, ctx.sb, nav.indice_inodo_padre, nav.nombre_final);
    if (existente != -1) {
        if (crear_intermedias) {
            return "MKDIR: la carpeta '" + ruta + "' ya existia, no se realizo ningun cambio.";
        }
        return "ERROR: MKDIR - la carpeta '" + ruta + "' ya existe";
    }

    int nuevo_indice_inodo = bitmap::ocupar_primera_libre(ctx.ruta_disco, ctx.sb.s_bm_inode_start, ctx.sb.s_inodes_count);
    int nuevo_indice_bloque = bitmap::ocupar_primera_libre(ctx.ruta_disco, ctx.sb.s_bm_block_start, ctx.sb.s_blocks_count);
    if (nuevo_indice_inodo == -1 || nuevo_indice_bloque == -1) {
        return "ERROR: MKDIR - no hay espacio disponible en el disco";
    }

    Inodo nueva_carpeta;
    memset(&nueva_carpeta, 0, sizeof(Inodo));
    nueva_carpeta.i_uid = uid;
    nueva_carpeta.i_gid = gid;
    nueva_carpeta.i_s = 0;
    nueva_carpeta.i_atime = nueva_carpeta.i_ctime = nueva_carpeta.i_mtime = time(nullptr);
    for (int i = 0; i < 15; i++) nueva_carpeta.i_block[i] = -1;
    nueva_carpeta.i_block[0] = nuevo_indice_bloque;
    nueva_carpeta.i_type = '0'; // carpeta
    strncpy(nueva_carpeta.i_perm, "664", 3);

    Bloque bloque_nuevo;
    memset(&bloque_nuevo, 0, sizeof(Bloque));
    strncpy(bloque_nuevo.carpeta.b_content[0].b_name, ".", 12);
    bloque_nuevo.carpeta.b_content[0].b_inodo = nuevo_indice_inodo;
    strncpy(bloque_nuevo.carpeta.b_content[1].b_name, "..", 12);
    bloque_nuevo.carpeta.b_content[1].b_inodo = nav.indice_inodo_padre;
    bloque_nuevo.carpeta.b_content[2].b_inodo = -1;
    bloque_nuevo.carpeta.b_content[3].b_inodo = -1;

    std::fstream archivo_disco(ctx.ruta_disco, std::ios::in | std::ios::out | std::ios::binary);
    archivo_disco.seekp(ctx.sb.s_inode_start + nuevo_indice_inodo * sizeof(Inodo));
    archivo_disco.write(reinterpret_cast<char*>(&nueva_carpeta), sizeof(Inodo));
    archivo_disco.seekp(ctx.sb.s_block_start + nuevo_indice_bloque * sizeof(Bloque));
    archivo_disco.write(reinterpret_cast<char*>(&bloque_nuevo), sizeof(Bloque));
    archivo_disco.close();

    navegacion::agregar_entrada_carpeta(ctx.ruta_disco, ctx.sb, nav.indice_inodo_padre, nav.nombre_final, nuevo_indice_inodo);

    return "MKDIR: carpeta '" + ruta + "' creada exitosamente.";
}

} 