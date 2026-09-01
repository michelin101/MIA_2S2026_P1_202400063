#pragma once
#include <vector>

namespace ajustes {

struct Hueco{
    int inicio;
    int tamano;
};

std::vector<Hueco> calcular_huecos(int inicio_zona, int tamano_zona, std::vector<Hueco> ocupados);
int elegir_espacio(const std::vector<Hueco>& huecos, int tamano_requerido, char tipo_fit);

}