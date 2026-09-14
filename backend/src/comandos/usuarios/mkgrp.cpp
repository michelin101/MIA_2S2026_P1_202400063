#include "mkgrp.hpp"
#include "../../sesion/sesion_usuario.hpp"
#include "../../utilidades/contexto_particion.hpp"
#include "../../utilidades/gestor_users_txt.hpp"
#include <sstream>

namespace comandos{

// funcion auxiliar para quitar espacios al inicio
static std::string recortar(std::string s){
    size_t inicio = s.find_first_not_of(" ");
    if(inicio == std::string::npos) return "";
    return s.substr(inicio);
}

std::string ejecutar_mkgrp(const std::map<std::string, std::string>& parametros){
    if(!sesion::SesionUsuario::instancia().es_root()){
        return "ERROR: MKGRP - solo el usuario root puede ejectuar este comando";
    }

    auto it_name = parametros.find("name");
    if(it_name == parametros.end()){
        return "ERROR: MKGRP - el parametro -name es obligatorio";
    }
    std::string nombre_grupo = it_name->second;

    auto ctx = contexto::obtener_contexto_sesion_activa();
    if(!ctx.ok) return ctx.mensaje_error;

    std::string contenido = gestor_users::leer_contenido(ctx.ruta_disco, ctx.sb, ctx.indice_inodo_users);

    std::istringstream flujo(contenido);
    std::string linea;
    int mayor_gid = 0;

    while (std::getline(flujo, linea)){
        std::istringstream partes(linea);
        std::string gid_str, tipo, grupo;
        std::getline(partes, gid_str, ',');
        std::getline(partes, tipo, ',');
        std::getline(partes, grupo, ',');

        tipo = recortar(tipo);
        grupo = recortar(grupo);

        int gid = std::stoi(recortar(gid_str));
        if(gid > mayor_gid) mayor_gid = gid;

        if(tipo == "G" && grupo == nombre_grupo && gid != 0){
            return "ERROR: MKGRP - el grupo '" + nombre_grupo + "' ya existe";
        }
    }

    int nuevo_gid = mayor_gid + 1;

    std::string nueva_linea = std::to_string(nuevo_gid) + ", G, " + nombre_grupo + "\n";
    std::string contenido_actualizado = contenido + nueva_linea;

    if(!gestor_users::escribir_contenido(ctx.ruta_disco, ctx.sb, ctx.indice_inodo_users, contenido_actualizado)){
        return "ERROR: MKGRP - no se pudo escribir en users.txt (posible falta de espacio)";
    }

    return "MKGRP: grupo '" + nombre_grupo + "' creado exitosamente con GID=" + std::to_string(nuevo_gid);
}

}