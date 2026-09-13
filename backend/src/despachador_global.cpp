#include "despachador_global.hpp"
#include "analizador/tokenizador.hpp"
#include "comandos/administracion_discos/mkdisk.hpp"
#include "comandos/administracion_discos/rmdisk.hpp"
#include "comandos/administracion_discos/fdisk.hpp"
#include "comandos/administracion_discos/mount.hpp"
#include "comandos/administracion_discos/mounted.hpp"
#include "comandos/sistema_archivos/mkfs.hpp"
#include "comandos/sistema_archivos/cat.hpp"
#include "comandos/usuarios/login.hpp"
#include "comandos/usuarios/logout.hpp"
#include "comandos/usuarios/mkgrp.hpp"
#include "comandos/usuarios/rmgrp.hpp"
#include "comandos/usuarios/mkusr.hpp"
#include "comandos/usuarios/rmusr.hpp"
#include "comandos/usuarios/chgrp.hpp"
#include "comandos/archivos_carpetas/mkfile.hpp"
#include "comandos/archivos_carpetas/mkdir.hpp"
#include "comandos/reportes/rep.hpp"

#include <sstream>

std::string procesar_comando(const std::string& linea){
    if(linea.empty()) return "";

    auto parseado = analizador::parsear_linea(linea);
    if(parseado.error){
        return "ERROR: " + parseado.mensaje_error;
    }

    const std::string& c = parseado.nombre_comando;
    const auto& p = parseado.parametros;

    if(c == "mkdisk") return comandos::ejecutar_mkdisk(p);
    if(c == "rmdisk") return comandos::ejecutar_rmdisk(p);
    if(c == "fdisk") return comandos::ejecutar_fdisk(p);
    if(c == "mount") return comandos::ejecutar_mount(p);
    if(c == "mounted") return comandos::ejecutar_mounted(p);
    if(c == "mkfs") return comandos::ejecutar_mkfs(p);
    if(c == "cat") return comandos::ejecutar_cat(p);
    if(c == "login") return comandos::ejecutar_login(p);
    if(c == "logout") return comandos::ejecutar_logout(p);
    if(c == "mkgrp") return comandos::ejecutar_mkgrp(p);
    if(c == "rmgrp") return comandos::ejecutar_rmgrp(p);
    if(c == "mkusr") return comandos::ejecutar_mkusr(p);
    if(c == "rmusr") return comandos::ejecutar_rmusr(p);
    if(c == "chgrp") return comandos::ejecutar_chgrp(p);
    if(c == "mkfile") return comandos::ejecutar_mkfile(p);
    if(c == "mkdir") return comandos::ejecutar_mkdir(p);
    if(c == "rep") return comandos::ejecutar_rep(p);

    return "ERROR: comando '" + c + "' no encontrado";
}

std::string procesar_bloque(const std::string& texto_completo){
    std::istringstream flujo(texto_completo);
    std::string linea;
    std::ostringstream salida;

    while(std::getline(flujo, linea)){
        size_t inicio = linea.find_first_not_of(" \t\r");
        std::string linea_limpia = (inicio == std::string::npos) ? "" : linea.substr(inicio);

        if (linea_limpia.empty()) {
            salida << "\n"; // preserva lineas en blanco 
            continue;
        }
        if (linea_limpia[0] == '#') {
            salida << linea << "\n"; // muestra el comentario tal cual aparece
            continue;
        }

        std::string resultado = procesar_comando(linea_limpia);
        salida << resultado << "\n";
    }
    return salida.str();
}