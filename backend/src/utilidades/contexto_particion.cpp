#include "contexto_particion.hpp"
#include "../sesion/sesion_usuario.hpp"
#include "../sesion/particiones_montadas.hpp"
#include "../utilidades/manejo_binario.hpp"
#include "../utilidades/gestor_users_txt.hpp"
#include "../estructuras/mbr.hpp"
#include <fstream>

namespace contexto{

ContextoParticion obtener_contexto_sesion_activa(){
    ContextoParticion ctx;

    if(!sesion::SesionUsuario::instancia().hay_sesion_activa()){
        ctx.mensaje_error = "ERROR: no existe una sesion activa. Debe iniciar sesion primero";
        return ctx;
    }
    
    std::string id = sesion::SesionUsuario::instancia().id_particion_actual();

    sesion::InfoMontaje info;
    if(!sesion::GestorMontajes::instancia().buscar_por_id(id, info)){
        ctx.mensaje_error = "ERROR: la particion de la sesion activa ya no esta montada";
        return ctx;
    }

    MBR mbr;
    if(!manejo_binario::leer_mbr(info.ruta_disco, mbr)){
        ctx.mensaje_error = "ERROR: no se pudo leer el disco de la sesion activa";
        return ctx;
    }
    Partition& particion = mbr.mbr_partitions[info.indice_particion_en_mbr];

    SuperBloque sb;
    std::ifstream archivo(info.ruta_disco, std::ios::binary);
    archivo.seekg(particion.part_start);
    archivo.read(reinterpret_cast<char*>(&sb), sizeof(SuperBloque));
    archivo.close();

    int indice_users = gestor_users::buscar_inodo_users(info.ruta_disco, sb);
    if(indice_users == -1){
        ctx.mensaje_error = "ERROR: no se encontro users.txt en esta particon";
        return ctx;
    }

    ctx.ok = true;
    ctx.ruta_disco = info.ruta_disco;
    ctx.sb = sb;
    ctx.indice_inodo_users = indice_users;
    return ctx;
}

}