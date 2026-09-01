#include "ajustes.hpp"
#include <algorithm>
#include <climits>

namespace ajustes{

std::vector<Hueco> calcular_huecos(int inicio_zona, int tamano_zona, std::vector<Hueco> ocupados){
    std::vector<Hueco> huecos;

    std::sort(ocupados.begin(), ocupados.end(), [](const Hueco& a, const Hueco& b) { 
        return a.inicio < b.inicio; 
    });

    int cursor = inicio_zona;
    int fin_zona = inicio_zona + tamano_zona;

    for(const auto& ocupado : ocupados){
        if(ocupado.inicio > cursor){
            huecos.push_back({cursor, ocupado.inicio - cursor});
        }
        cursor = ocupado.inicio + ocupado.tamano;
    }

    if(cursor < fin_zona){
        huecos.push_back({cursor, fin_zona - cursor});
    }
    return huecos;
}

int elegir_espacio(const std::vector<Hueco>& huecos, int tamano_requerido, char tipo_fit){
    int mejor_inicio = -1;

    if(tipo_fit == 'F'){
        // el primero que alcance, First fit
        for(const auto& h : huecos){
            if(h.tamano >= tamano_requerido){
                return h.inicio;
            }
        }
    }else if(tipo_fit == 'B'){
        // el que sobre menos espacio, Best fit
        int menor_sobrante = INT_MAX;
        for(const auto& h : huecos){
            if(h.tamano >= tamano_requerido){
                int sobrante = h.tamano - tamano_requerido;
                if(sobrante < menor_sobrante){
                    menor_sobrante = sobrante;
                    mejor_inicio = h.inicio;
                }
            }
        }
    }else if(tipo_fit == 'W'){
        // el que sobre mas espcio, Worst fit
        int mayor_sobrante = -1;
        for(const auto& h : huecos){
            if(h.tamano >= tamano_requerido){
                int sobrante = h.tamano - tamano_requerido;
                if(sobrante > mayor_sobrante){
                    mayor_sobrante = sobrante;
                    mejor_inicio = h.inicio;
                }
            }
        }
    }
    return mejor_inicio;
}

}