#include "rmgrp.hpp"
#include "../../sesion/sesion_usuario.hpp"
#include "../../utilidades/contexto_particion.hpp"
#include "../../utilidades/gestor_users_txt.hpp"
#include <sstream>

namespace comandos{

static std::string recortar(std::string s) {
    size_t inicio = s.find_first_not_of(" ");
    if (inicio == std::string::npos) return "";
    return s.substr(inicio);
}

std::string ejecutar_rmgrp(const std::map<std::string, std::string>& parametros){
    if(!sesion::SesionUsuario::instancia().es_root()){
        return "ERROR: RMGRP - solo el usuario root puede ejecutar este comando";
    }

    auto it_name = parametros.find("name");
    if(it_name == parametros.end()){
        return "ERROR: RMGRP - el parametro -name es obligatorio";
    }
    std::string nombre_grupo = it_name->second;

    auto ctx = contexto::obtener_contexto_sesion_activa();
    if(!ctx.ok) return ctx.mensaje_error;

    std::string contenido = gestor_users::leer_contenido(ctx.ruta_disco, ctx.sb, ctx.indice_inodo_users);

    // reconstruir el archivo linea por linea, poniendo GID= 0 en la linea encontrada
    std::istringstream flujo(contenido);
    std::string linea;
    std::ostringstream nuevo_contenido;
    bool encontrado = false;

    while (std::getline(flujo, linea)) {
        std::istringstream partes(linea);
        std::string gid_str, tipo, grupo, resto;
        std::getline(partes, gid_str, ',');
        std::getline(partes, tipo, ',');
        std::getline(partes, grupo, ',');

        std::string tipo_limpio = recortar(tipo);
        std::string grupo_limpio = recortar(grupo);
        int gid = std::stoi(recortar(gid_str));

        if (tipo_limpio == "G" && grupo_limpio == nombre_grupo && gid != 0) {
            encontrado = true;
            nuevo_contenido << "0, G, " << grupo_limpio << "\n";
        } else {
            nuevo_contenido << linea << "\n";
        }
    }

    if(!encontrado){
        return "ERROR: RMGRP - el grupo '" + nombre_grupo + "' no existe o ya fue eliminado";
    }

    if(!gestor_users::escribir_contenido(ctx.ruta_disco, ctx.sb, ctx.indice_inodo_users, nuevo_contenido.str())){
        return "ERROR: RMGRP - no se pudo actualizar users.txt";
    }
    return "RMGRP: grupo '" + nombre_grupo + "' eliminado exitosamente"; 
}

}