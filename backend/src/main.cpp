#include <iostream>
#include <ctime>
#include <cstdlib>
#include "analizador/tokenizador.hpp"
#include "comandos/administracion_discos/mkdisk.hpp"

int main() {
    srand(time(nullptr)); 
    std::cout << "=== ExtreamFS - modo consola (pruebas Sprint 1) ===" << std::endl;
    std::cout << "Escribe un comando (o 'salir' para terminar):" << std::endl;

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
        } else {
            std::cout << "ERROR: comando '" << parseado.nombre_comando << "' no reconocido." << std::endl;
        }
    }

    return 0;
}