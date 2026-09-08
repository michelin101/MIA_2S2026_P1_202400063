#include "sesion_usuario.hpp"

namespace sesion{

SesionUsuario& SesionUsuario::instancia(){
    static SesionUsuario unica;
    return unica;
}

bool SesionUsuario::hay_sesion_activa() const{
    return activa_;
}

bool SesionUsuario::iniciar_sesion(const std::string& usuario, const std::string& id_particion){
    if(activa_) return false; // ya hay algquie logueado
    activa_ = true;
    usuario_ = usuario;
    id_particion_ = id_particion;
    return true;
}

void SesionUsuario::cerrar_sesion(){
    activa_ = false;
    usuario_.clear();
    id_particion_.clear();
}

std::string SesionUsuario::usuario_actual() const{
    return usuario_;
}

std::string SesionUsuario::id_particion_actual() const{
    return id_particion_;
}

bool SesionUsuario::es_root() const{
    return usuario_ == "root";
}

}