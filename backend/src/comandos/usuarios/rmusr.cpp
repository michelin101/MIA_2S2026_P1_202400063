#include "rmusr.hpp"
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

std::string ejecutar_rmusr(const std::map<std::string, std::string>& parametros){
    if (!sesion::SesionUsuario::instancia().es_root()) {
        return "ERROR: RMUSR - solo el usuario root puede ejecutar este comando";
    }

    auto it_user = parametros.find("user");
    if (it_user == parametros.end()) {
        return "ERROR: RMUSR - el parametro -user es obligatorio";
    }
    std::string usuario = it_user->second;

    auto ctx = contexto::obtener_contexto_sesion_activa();
    if(!ctx.ok) return ctx.mensaje_error;

    std::string contenido = gestor_users::leer_contenido(ctx.ruta_disco, ctx.sb, ctx.indice_inodo_users);

    std::istringstream flujo(contenido);
    std::string linea;
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
        std::string grupo_limpio = recortar(grupo);
        std::string pass_limpio = recortar(pass);
        int id_num = std::stoi(recortar(id_str));

        if (tipo_limpio == "U" && user_limpio == usuario && id_num != 0) {
            encontrado = true;
            nuevo_contenido << "0, U, " << grupo_limpio << ", " << user_limpio << ", " << pass_limpio << "\n";
        } else {
            nuevo_contenido << linea << "\n";
        }
    }

    if (!encontrado) {
        return "ERROR: RMUSR - el usuario '" + usuario + "' no existe o ya fue eliminado";
    }

    if (!gestor_users::escribir_contenido(ctx.ruta_disco, ctx.sb, ctx.indice_inodo_users, nuevo_contenido.str())) {
        return "ERROR: RMUSR - no se pudo actualizar users.txt";
    }

    return "RMUSR: usuario '" + usuario + "' eliminado exitosamente";
}

}