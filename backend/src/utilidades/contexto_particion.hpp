// backend/src/utilidades/contexto_particion.hpp
#pragma once
#include <string>
#include "../estructuras/superbloque.hpp"

namespace contexto{

// agrupa todo lo que un comando necesita para trabajar sobres esa particion donde esta la sesion activa
struct ContextoParticion {
    bool ok = false;
    std::string mensaje_error;

    std::string ruta_disco;
    SuperBloque sb;
    int indice_inodo_users = -1;
};

// valida la sesion activa, localiza la particion montada y su superbloque, y ubica el inodo de user.txt
ContextoParticion obtener_contexto_sesion_activa();

}