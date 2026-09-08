#pragma once
#include <string>
#include "../estructuras/superbloque.hpp"

namespace gestor_users{
// busca el inodo de users.txt dentro de la partici donde el superbloque ya se leyo, retorna el indice del inodo o -1 sino se encontro
int buscar_inodo_users(const std::string& ruta_disco, const SuperBloque& sb);

// lee el contenido completo de users.txt como texto plano
std::string leer_contenido(const std::string& ruta_disco, const SuperBloque& sb, int indice_inodo);

// sobreescribe el contenido completo de users.txt
bool escribir_contenido(const std::string& ruta_disco, const SuperBloque& sb, int indice_inodo, const std::string& nuevo_contenido);

}