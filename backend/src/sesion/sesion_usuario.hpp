#pragma once
#include <string>

namespace sesion {

class SesionUsuario {
public:
    static SesionUsuario& instancia();

    bool hay_sesion_activa() const;
    bool iniciar_sesion(const std::string& usuario, const std::string& id_particion);
    void cerrar_sesion();

    std::string usuario_actual() const;
    std::string id_particion_actual() const;
    bool es_root() const;

    void establecer_uid_gid(int uid, int gid);
    int uid_actual() const;
    int gid_actual() const;

private:
    SesionUsuario() = default;
    bool activa_ = false;
    std::string usuario_;
    std::string id_particion_;

    int uid_ = 0;
    int gid_ = 0;
};

}