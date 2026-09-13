#include <iostream>
#include <ctime>
#include <cstdlib>
#include <cstring>
#include "despachador_global.hpp"
#include "servidor/servidor_http.hpp"

int main(int argc, char* argv[]) {
    srand(time(nullptr));

    if (argc > 1 && std::strcmp(argv[1], "servidor") == 0) {
        servidor::iniciar(8080);
        return 0;
    }

    std::cout << "=== ExtreamFS - modo consola ===" << std::endl;
    std::cout << "(ejecuta './extreamfs servidor' para modo API REST)" << std::endl;

    std::string linea;
    while (true) {
        std::cout << "> ";
        std::getline(std::cin, linea);
        if (linea == "salir") break;
        if (linea.empty()) continue;

        std::cout << procesar_comando(linea) << std::endl;
    }

    return 0;
}