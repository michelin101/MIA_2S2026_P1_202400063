#include "login.hpp"
#include "../../sesion/sesion_usuario.hpp"
#include "../../sesion/particiones_montadas.hpp"
#include "../../utilidades/manejo_binario.hpp"
#include "../../utilidades/gestor_users_txt.hpp"
#include "../../estructuras/superbloque.hpp"
#include <fstream>
#include <sstream>

namespace comandos{

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

    // se parsea linea por linea buscando un registro tipo usario que coincida
    std::istringstream flujo(contenido);
    std::string linea;
    bool usuario_encontrado = false;
    bool clave_correcta = false;

    while(std::getline(flujo, linea)){
        // formato: UID, U, grupo, usuario, contraseña
        std::istringstream partes(linea);
        std::string uid, tipo, grupo, user, pass;
        std::getline(partes, uid, ',');
        std::getline(partes, tipo, ',');
        std::getline(partes, grupo, ',');
        std::getline(partes, user, ',');
        std::getline(partes, pass, ',');

        // quitar espacios en blanco al inicio
        auto quitar_espacios = [](std::string& s){
            size_t inicio = s.find_first_not_of(" ");
            if(inicio != std::string::npos) s = s.substr(inicio);
        };
        quitar_espacios(tipo);
        quitar_espacios(user);
        quitar_espacios(pass);

        if(tipo == "U" && user == usuario){
            usuario_encontrado = true;
            if(pass == clave){
                clave_correcta = true;
            }
            break;
        }

    }
    if(!usuario_encontrado){
        return "ERROR: LOGIN - el usuario '" + usuario + "' no existe";
    }
    if(!clave_correcta){
        return "ERROR: LOGIN - autenticacion fallida, contraseña incorrecta";
    }
    int uid_encontrado = 0;
    int gid_encontrado = 0; // gid 

    while (std::getline(flujo, linea)) {
        std::istringstream partes(linea);
        std::string uid, tipo, grupo, user, pass;
        std::getline(partes, uid, ',');
        std::getline(partes, tipo, ',');
        std::getline(partes, grupo, ',');
        std::getline(partes, user, ',');
        std::getline(partes, pass, ',');

        auto quitar_espacios = [](std::string& s) {
            size_t inicio = s.find_first_not_of(" ");
            if (inicio != std::string::npos) s = s.substr(inicio);
        };
        quitar_espacios(tipo); quitar_espacios(grupo); quitar_espacios(user); quitar_espacios(pass);

        if (tipo == "U" && user == usuario) {
            usuario_encontrado = true;
            if (pass == clave) {
                clave_correcta = true;
                uid_encontrado = std::stoi(uid);
                std::istringstream flujo2(contenido);
                std::string linea2;
                while (std::getline(flujo2, linea2)) {
                    std::istringstream p2(linea2);
                    std::string gid2, tipo2, grupo2;
                    std::getline(p2, gid2, ','); std::getline(p2, tipo2, ','); std::getline(p2, grupo2, ',');
                    quitar_espacios(tipo2); quitar_espacios(grupo2);
                    if (tipo2 == "G" && grupo2 == grupo) { gid_encontrado = std::stoi(gid2); break; }
                }
            }
            break;
        }
    }
    
    sesion::SesionUsuario::instancia().establecer_uid_gid(uid_encontrado, gid_encontrado);
    sesion::SesionUsuario::instancia().iniciar_sesion(usuario, id);

    return "LOGIN: sesion iniciada exitosamente como '" + usuario + "' en id= " + id;


}
}