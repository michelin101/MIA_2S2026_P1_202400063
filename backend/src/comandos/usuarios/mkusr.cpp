#include "mkusr.hpp"
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

std::string ejecutar_mkusr(const std::map<std::string, std::string>& parametros){
    if(!sesion::SesionUsuario::instancia().es_root()){
        return "ERROR: MKUSR - solo el usuario root puede ejecutar este comando";
    }

    auto it_user = parametros.find("user");
    auto it_pass = parametros.find("pass");
    auto it_grp = parametros.find("grp");
    if(it_user == parametros.end() || it_pass == parametros.end() || it_grp == parametros.end()){
        return "ERROR: MKUSR - los parametros -user, -pass y -grp son obligatorios";
    }

    std::string usuario = it_user->second;
    std::string clave = it_pass->second;
    std::string grupo_deseado = it_grp->second;

    if (usuario.size() > 10) return "ERROR: MKUSR - el nombre de usuario supera los 10 caracteres";
    if (clave.size() > 10) return "ERROR: MKUSR - la contraseña supera los 10 caracteres";
    if (grupo_deseado.size() > 10) return "ERROR: MKUSR - el nombre de grupo supera los 10 caracteres";

    auto ctx = contexto::obtener_contexto_sesion_activa();
    if (!ctx.ok) return ctx.mensaje_error;

    std::string contenido = gestor_users::leer_contenido(ctx.ruta_disco, ctx.sb, ctx.indice_inodo_users);

    std::istringstream flujo(contenido);
    std::string linea;
    int mayor_uid = 0;
    bool grupo_existe = false;
    bool usuario_ya_existe = false;

    while (std::getline(flujo, linea)) {
        std::istringstream partes(linea);
        std::string id_str, tipo, campo3, campo4;
        std::getline(partes, id_str, ',');
        std::getline(partes, tipo, ',');
        std::getline(partes, campo3, ',');
        std::getline(partes, campo4, ',');

        tipo = recortar(tipo);
        campo3 = recortar(campo3);
        campo4 = recortar(campo4);
        int id_num = std::stoi(recortar(id_str));

        if (tipo == "G" && campo3 == grupo_deseado && id_num != 0) {
            grupo_existe = true;
        }
        if (tipo == "U") {
            if (id_num > mayor_uid) mayor_uid = id_num;
            // en registros de usuario: campo3=Grupo, campo4=Usuario
            if (campo4 == usuario && id_num != 0) {
                usuario_ya_existe = true;
            }
        }
    }

    if(!grupo_existe){
        return "ERROR: MKUSR - el grupo '" + grupo_deseado + "' no existe";
    }
    if(usuario_ya_existe){
        return "ERROR: MKUSR - el usuario '" + usuario + "' ya existe";
    }

    int nuevo_uid = mayor_uid + 1;
    std::string nueva_linea = std::to_string(nuevo_uid) + ", U, " + grupo_deseado + ", " + usuario + ", " + clave + "\n";
    std::string contenido_actualizado = contenido + nueva_linea;

    if(!gestor_users::escribir_contenido(ctx.ruta_disco, ctx.sb, ctx.indice_inodo_users, contenido_actualizado)){
        return "ERROR: MKUSR - no se pudo escribir en users.txt";
    }

    return "MKUSR: usuario '" + usuario + "' creado exitosamente con UID=" + std::to_string(nuevo_uid);    
}

}