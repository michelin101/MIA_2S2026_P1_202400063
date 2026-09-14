#include "login.hpp"
#include "../../sesion/sesion_usuario.hpp"
#include "../../sesion/particiones_montadas.hpp"
#include "../../utilidades/manejo_binario.hpp"
#include "../../utilidades/gestor_users_txt.hpp"
#include "../../estructuras/superbloque.hpp"
#include <fstream>
#include <sstream>

namespace comandos{

static std::string quitar_espacios(std::string s){
    size_t inicio = s.find_first_not_of(" ");
    if(inicio != std::string::npos) return s.substr(inicio);
    return s;
}

std::string ejecutar_login(const std::map<std::string, std::string>& parametros){
    if(sesion::SesionUsuario::instancia().hay_sesion_activa()){
        return "ERROR: LOGIN - ya existe una sesion acitva, debe cerrar sesion antes de iniciar otra";
    }

    auto it_user = parametros.find("user");
    auto it_pass = parametros.find("pass");
    auto it_id = parametros.find("id");

    if(it_user == parametros.end() || it_pass == parametros.end() || it_id == parametros.end()){
        return "ERROR: LOGIN - los parametros -user, -pass, e -id son obligatorios";
    }

    std::string usuario = it_user->second;
    std::string clave = it_pass->second;
    std::string id = it_id->second;

    sesion::InfoMontaje info;
    if(!sesion::GestorMontajes::instancia().buscar_por_id(id, info)){
        return "ERROR: LOGIN - no existe ninguna particon montada con el id '" + id + "'";
    }

    MBR mbr;
    manejo_binario::leer_mbr(info.ruta_disco, mbr);
    Partition& particion = mbr.mbr_partitions[info.indice_particion_en_mbr];

    SuperBloque sb;
    std::ifstream archivo(info.ruta_disco, std::ios::binary);
    archivo.seekg(particion.part_start);
    archivo.read(reinterpret_cast<char*>(&sb), sizeof(SuperBloque));
    archivo.close();

    int indice_inodo_users = gestor_users::buscar_inodo_users(info.ruta_disco, sb);
    if(indice_inodo_users == -1){
        return "ERROR: LOGIN - no se encontro el archivo users.txt en esta particion";
    }

    std::string contenido = gestor_users::leer_contenido(info.ruta_disco, sb, indice_inodo_users);

    // --- Primera pasada: buscar al usuario, validar clave, y guardar su grupo ---
    bool usuario_encontrado = false;
    bool clave_correcta = false;
    int uid_encontrado = 0;
    std::string nombre_grupo_usuario;

    {
        std::istringstream flujo(contenido);
        std::string linea;
        while(std::getline(flujo, linea)){
            std::istringstream partes(linea);
            std::string uid, tipo, grupo, user, pass;
            std::getline(partes, uid, ',');
            std::getline(partes, tipo, ',');
            std::getline(partes, grupo, ',');
            std::getline(partes, user, ',');
            std::getline(partes, pass, ',');

            tipo = quitar_espacios(tipo);
            grupo = quitar_espacios(grupo);
            user = quitar_espacios(user);
            pass = quitar_espacios(pass);

            if(tipo == "U" && user == usuario){
                usuario_encontrado = true;
                if(pass == clave){
                    clave_correcta = true;
                    uid_encontrado = std::stoi(quitar_espacios(uid));
                    nombre_grupo_usuario = grupo;
                }
                break;
            }
        }
    }

    if(!usuario_encontrado){
        return "ERROR: LOGIN - el usuario '" + usuario + "' no existe";
    }
    if(!clave_correcta){
        return "ERROR: LOGIN - autenticacion fallida, contraseña incorrecta";
    }

    // --- Segunda pasada (stream nuevo, independiente): buscar el GID del grupo ---
    int gid_encontrado = 0;
    {
        std::istringstream flujo2(contenido);
        std::string linea2;
        while(std::getline(flujo2, linea2)){
            std::istringstream p2(linea2);
            std::string gid2, tipo2, grupo2;
            std::getline(p2, gid2, ',');
            std::getline(p2, tipo2, ',');
            std::getline(p2, grupo2, ',');

            tipo2 = quitar_espacios(tipo2);
            grupo2 = quitar_espacios(grupo2);

            if(tipo2 == "G" && grupo2 == nombre_grupo_usuario){
                gid_encontrado = std::stoi(quitar_espacios(gid2));
                break;
            }
        }
    }

    sesion::SesionUsuario::instancia().establecer_uid_gid(uid_encontrado, gid_encontrado);
    sesion::SesionUsuario::instancia().iniciar_sesion(usuario, id);

    return "LOGIN: sesion iniciada exitosamente como '" + usuario + "' en id= " + id;
}
}