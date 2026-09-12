#pragma once
#include <string>
#include <vector>
#include "../estructuras/superbloque.hpp"

namespace navegacion{
// seprar una ruta  tipo "/home/user/docs/a.txt" en sus segmentos
std::vector<std::string> separar_ruta(const std::string& ruta);

// busca un nombre dentro del inodo carpeta indicado, retorna el indice de inodo encontrado
int buscar_en_carpeta(const std::string& ruta_disco, const SuperBloque& sb, int indice_inodo_carpeta, const std::string& nombre);

// agrega una entrada dentro del bloque carpeta indicado. 
bool agregar_entrada_carpeta(const std::string& ruta_disco, const SuperBloque& sb, int indice_inodo_carpeta, const std::string& nombre, int indice_inodo_nuevo);

// resultado de navegar hasta el padre de la ruta final
struct ResultadoNavegacion {
    bool ok = false;
    std::string mensaje_error;
    int indice_inodo_padre = -1;   // inodo de la carpeta contenedora
    std::string nombre_final;      // el ultimo segmento (archivo o carpeta a crear)
};

// Navega la ruta hasta el padre del último segmento.
ResultadoNavegacion navegar_hasta_padre(const std::string& ruta_disco, SuperBloque& sb, const std::string& ruta_completa, bool crear_intermedias, int uid_actual, int gid_actual);

}