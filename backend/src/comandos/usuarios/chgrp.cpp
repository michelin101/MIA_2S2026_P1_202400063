#include "chgrp.hpp"
#include "../../sesion/sesion_usuario.hpp"
#include "../../utilidades/contexto_particion.hpp"
#include "../../utilidades/gestor_users_txt.hpp"
#include <sstream>

namespace comandos {

static std::string recortar(std::string s) {
    size_t inicio = s.find_first_not_of(" ");
    if (inicio == std::string::npos) return "";
    return s.substr(inicio);
}

std::string ejecutar_chgrp(const std::map<std::string, std::string>& parametros) {
    if (!sesion::SesionUsuario::instancia().es_root()) {
        return "ERROR: CHGRP - solo el usuario root puede ejecutar este comando";
    }

    auto it_user = parametros.find("user");
    auto it_grp = parametros.find("grp");
    if (it_user == parametros.end() || it_grp == parametros.end()) {
        return "ERROR: CHGRP - los parametros -user y -grp son obligatorios";
    }
    std::string usuario = it_user->second;
    std::string nuevo_grupo = it_grp->second;

    auto ctx = contexto::obtener_contexto_sesion_activa();
    if (!ctx.ok) return ctx.mensaje_error;

    std::string contenido = gestor_users::leer_contenido(ctx.ruta_disco, ctx.sb, ctx.indice_inodo_users);

    // Primero: validar que el nuevo grupo exista y este activo
    std::istringstream verificacion(contenido);
    std::string linea;
    bool grupo_valido = false;
    while (std::getline(verificacion, linea)) {
        std::istringstream partes(linea);
        std::string id_str, tipo, grupo;
        std::getline(partes, id_str, ',');
        std::getline(partes, tipo, ',');
        std::getline(partes, grupo, ',');
        if (recortar(tipo) == "G" && recortar(grupo) == nuevo_grupo && std::stoi(recortar(id_str)) != 0) {
            grupo_valido = true;
            break;
        }
    }
    if (!grupo_valido) {
        return "ERROR: CHGRP - el grupo '" + nuevo_grupo + "' no existe o esta eliminado";
    }

    // reconstruimos cambiando el grupo del usuario indicado
    std::istringstream flujo(contenido);
    std::ostringstream nuevo_contenido;
    bool encontrado = false;

    while (std::getline(flujo, linea)) {
        std::istringstream partes(linea);
        std::string id_str, tipo, grupo, user, pass;
        std::getline(partes, id_str, ',');
        std::getline(partes, tipo, ',');
        std::getline(partes, grupo, ',');
        std::getline(partes, user, ',');
        std::getline(partes, pass, ',');

        std::string tipo_limpio = recortar(tipo);
        std::string user_limpio = recortar(user);
        std::string pass_limpio = recortar(pass);
        int id_num = std::stoi(recortar(id_str));

        if (tipo_limpio == "U" && user_limpio == usuario && id_num != 0) {
            encontrado = true;
            nuevo_contenido << id_num << ", U, " << nuevo_grupo << ", " << user_limpio << ", " << pass_limpio << "\n";
        } else {
            nuevo_contenido << linea << "\n";
        }
    }

    if (!encontrado) {
        return "ERROR: CHGRP - el usuario '" + usuario + "' no existe";
    }

    if (!gestor_users::escribir_contenido(ctx.ruta_disco, ctx.sb, ctx.indice_inodo_users, nuevo_contenido.str())) {
        return "ERROR: CHGRP - no se pudo actualizar users.txt";
    }

    return "CHGRP: usuario '" + usuario + "' movido exitosamente al grupo '" + nuevo_grupo + "'";
}

} 