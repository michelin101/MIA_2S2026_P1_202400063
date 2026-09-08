#include "logout.hpp"
#include "../../sesion/sesion_usuario.hpp"

namespace comandos {

std::string ejecutar_logout(const std::map<std::string, std::string>& parametros) {
    if (!sesion::SesionUsuario::instancia().hay_sesion_activa()) {
        return "ERROR: LOGOUT - no hay ninguna sesión activa actualmente.";
    }
    sesion::SesionUsuario::instancia().cerrar_sesion();
    return "LOGOUT: sesión cerrada exitosamente.";
}

} 