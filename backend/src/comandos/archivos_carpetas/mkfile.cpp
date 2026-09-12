#include "mkfile.hpp"
#include "../../sesion/sesion_usuario.hpp"
#include "../../utilidades/contexto_particion.hpp"
#include "../../utilidades/navegacion_directorios.hpp"
#include "../../utilidades/gestor_users_txt.hpp"
#include "../../estructuras/inodo.hpp"
#include "../../utilidades/bitmap.hpp"
#include <fstream>
#include <sstream>
#include <cstring>

namespace comandos{

std::string ejecutar_mkfile(const std::map<std::string, std::string>& parametros){
    auto it_path = parametros.find("path");
    if(it_path == parametros.end()){
        return "ERROR: MKFILE - el parametro -path es obligatoria";
    }
    std::string ruta = it_path->second;

    if(parametros.count("r") && !parametros.at("r").empty()){
        return "ERROR: MKFILE - el parametro -r no debe recibir ningun valor";
    }
    bool crear_intermedias = parametros.count("r") > 0;

    long tamano_solicitado = 0;
    if(parametros.count("size")){
        try{
            tamano_solicitado = std::stol(parametros.at("size"));
        }catch(...){
            return "ERROR: MKFILE - el valor de -size no es numerico";
        }
        if(tamano_solicitado < 0){
            return "ERROR: MKFILE - -size no puede ser negativo";
        }
    }

    std::string ruta_contenido_externo;
    if (parametros.count("cont")) {
        ruta_contenido_externo = parametros.at("cont");
        std::ifstream prueba(ruta_contenido_externo);
        if (!prueba.good()) {
            return "ERROR: MKFILE - el archivo de contenido '" + ruta_contenido_externo + "' no existe";
        }
    }

    auto ctx = contexto::obtener_contexto_sesion_activa();
    if (!ctx.ok) return ctx.mensaje_error;

    int uid = sesion::SesionUsuario::instancia().uid_actual();
    int gid = sesion::SesionUsuario::instancia().gid_actual();

    auto nav = navegacion::navegar_hasta_padre(ctx.ruta_disco, ctx.sb, ruta, crear_intermedias, uid, gid);
    if (!nav.ok) return nav.mensaje_error;

    int existente = navegacion::buscar_en_carpeta(ctx.ruta_disco, ctx.sb, nav.indice_inodo_padre, nav.nombre_final);

    std::string contenido_final;
    if (!ruta_contenido_externo.empty()) {
        // prioridad: -cont sobre -size, 
        std::ifstream archivo_externo(ruta_contenido_externo);
        std::ostringstream buffer;
        buffer << archivo_externo.rdbuf();
        contenido_final = buffer.str();
    } else if (tamano_solicitado > 0) {
        // rellenar con 0-9 repetidos hasta completar el tamaño
        for (long i = 0; i < tamano_solicitado; i++) {
            contenido_final += static_cast<char>('0' + (i % 10));
        }
    }

    int indice_inodo_archivo;
    bool es_nuevo = (existente == -1);

    if (es_nuevo) {
        indice_inodo_archivo = bitmap::ocupar_primera_libre(ctx.ruta_disco, ctx.sb.s_bm_inode_start, ctx.sb.s_inodes_count);
        if (indice_inodo_archivo == -1) {
            return "ERROR: MKFILE - no hay inodos libres disponibles";
        }

        Inodo nuevo_inodo;
        memset(&nuevo_inodo, 0, sizeof(Inodo));
        nuevo_inodo.i_uid = uid;
        nuevo_inodo.i_gid = gid;
        nuevo_inodo.i_s = 0;
        nuevo_inodo.i_atime = nuevo_inodo.i_ctime = nuevo_inodo.i_mtime = time(nullptr);
        for (int i = 0; i < 15; i++) nuevo_inodo.i_block[i] = -1;
        nuevo_inodo.i_type = '1'; // archivo
        strncpy(nuevo_inodo.i_perm, "664", 3);

        std::fstream archivo_disco(ctx.ruta_disco, std::ios::in | std::ios::out | std::ios::binary);
        archivo_disco.seekp(ctx.sb.s_inode_start + indice_inodo_archivo * sizeof(Inodo));
        archivo_disco.write(reinterpret_cast<char*>(&nuevo_inodo), sizeof(Inodo));
        archivo_disco.close();

        navegacion::agregar_entrada_carpeta(ctx.ruta_disco, ctx.sb, nav.indice_inodo_padre, nav.nombre_final, indice_inodo_archivo);
    } else {
        indice_inodo_archivo = existente;
    }

    if (!contenido_final.empty()) {
        if (!gestor_users::escribir_contenido(ctx.ruta_disco, ctx.sb, indice_inodo_archivo, contenido_final)) {
            return "ERROR: MKFILE - no se pudo escribir el contenido (posible falta de espacio)";
        }
    }

    std::string mensaje_extra = es_nuevo ? "creado" : "sobrescrito";
    return "MKFILE: archivo '" + ruta + "' " + mensaje_extra + " exitosamente";

}
}