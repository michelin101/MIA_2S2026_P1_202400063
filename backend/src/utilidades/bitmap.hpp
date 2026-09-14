#pragma once
#include <string>

namespace bitmap{
    // inicializa un bitmap de cantidad posiciones, todas libres-0, escrbiendolo en el dispo a parte de byte_inicio
    void inicializar(const std::string& ruta_disco, int byte_inicio, int cantidad);
    // busca la primera poscion libre en el bitmap y la marca como ocupada-1, retorna indice encontrado o-1 si no hay espacio
    int ocupar_primera_libre(const std::string& ruta_disco, int byte_inicio, int cantidad);
    // marca una posicion especifica como libre-0 otra vez
    void liberar_posicion(const std::string& ruta_disco, int byte_inicio, int indice);

    int contar_libres(const std::string& ruta_disco, int byte_inicio, int cantidad);
}