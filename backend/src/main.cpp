#include <iostream>
#include <ctime>
#include <cstdlib>
#include "analizador/tokenizador.hpp"
#include "comandos/administracion_discos/mkdisk.hpp"
#include "comandos/administracion_discos/rmdisk.hpp"
#include "comandos/administracion_discos/fdisk.hpp"
#include "comandos/administracion_discos/mount.hpp"
#include "comandos/administracion_discos/mounted.hpp"
#include "comandos/sistema_archivos/mkfs.hpp"
#include "comandos/sistema_archivos/mkfs.hpp"
#include "comandos/usuarios/login.hpp"
#include "comandos/usuarios/logout.hpp"
#include "comandos/usuarios/mkgrp.hpp"
#include "comandos/usuarios/rmgrp.hpp"
#include "comandos/usuarios/mkusr.hpp"
#include "comandos/usuarios/rmusr.hpp"
#include "comandos/usuarios/chgrp.hpp"
#include "comandos/sistema_archivos/cat.hpp"

int main() {
    srand(time(nullptr));

    std::cout << "=== ExtreamFS - modo consola (pruebas Sprint 5) ===" << std::endl;

    std::string linea;
    while (true) {
        std::cout << "> ";
        std::getline(std::cin, linea);
        if (linea == "salir") break;
        if (linea.empty()) continue;

        auto parseado = analizador::parsear_linea(linea);
        if (parseado.error) {
            std::cout << "ERROR: " << parseado.mensaje_error << std::endl;
            continue;
        }

        if (parseado.nombre_comando == "mkdisk") {
            std::cout << comandos::ejecutar_mkdisk(parseado.parametros) << std::endl;
        } else if (parseado.nombre_comando == "rmdisk") {
            std::cout << comandos::ejecutar_rmdisk(parseado.parametros) << std::endl;
        } else if (parseado.nombre_comando == "fdisk") {
            std::cout << comandos::ejecutar_fdisk(parseado.parametros) << std::endl;
        } else if (parseado.nombre_comando == "mount") {
            std::cout << comandos::ejecutar_mount(parseado.parametros) << std::endl;
        } else if (parseado.nombre_comando == "mounted") {
            std::cout << comandos::ejecutar_mounted(parseado.parametros) << std::endl;
        } else if(parseado.nombre_comando == "mkfs"){
            std::cout << comandos::ejecutar_mkfs(parseado.parametros) << std::endl;
        } else if (parseado.nombre_comando == "mkfs") {
            std::cout << comandos::ejecutar_mkfs(parseado.parametros) << std::endl;     
        } else if (parseado.nombre_comando == "login") {
            std::cout << comandos::ejecutar_login(parseado.parametros) << std::endl;
        } else if (parseado.nombre_comando == "logout") {
            std::cout << comandos::ejecutar_logout(parseado.parametros) << std::endl;
        } else if (parseado.nombre_comando == "mkgrp") {
            std::cout << comandos::ejecutar_mkgrp(parseado.parametros) << std::endl;
        } else if (parseado.nombre_comando == "rmgrp") {
            std::cout << comandos::ejecutar_rmgrp(parseado.parametros) << std::endl;
        } else if (parseado.nombre_comando == "mkusr") {
            std::cout << comandos::ejecutar_mkusr(parseado.parametros) << std::endl;
        } else if (parseado.nombre_comando == "rmusr") {
            std::cout << comandos::ejecutar_rmusr(parseado.parametros) << std::endl;
        } else if (parseado.nombre_comando == "chgrp") {
            std::cout << comandos::ejecutar_chgrp(parseado.parametros) << std::endl;
        } else if (parseado.nombre_comando == "cat") {
            std::cout << comandos::ejecutar_cat(parseado.parametros) << std::endl;
        }else {
            std::cout << "ERROR: comando '" << parseado.nombre_comando << "' no reconocido" << std::endl;
        }
    }

    return 0;
}