#include "gestor_users_txt.hpp"
#include "../estructuras/inodo.hpp"
#include "../estructuras/bloques.hpp"
#include "../utilidades/bitmap.hpp"
#include <fstream>
#include <cstring>

namespace gestor_users{

static Inodo leer_inodo(const std::string& ruta, const SuperBloque& sb, int indice){
    Inodo ino;
    std::ifstream archivo(ruta, std::ios::binary);
    archivo.seekg(sb.s_inode_start + indice * sizeof(Inodo));
    archivo.read(reinterpret_cast<char*>(&ino), sizeof(Inodo));
    archivo.close();
    return ino;
}

static void escribir_inodo(const std::string& ruta, const SuperBloque& sb, int indice, const Inodo& ino){
    std::fstream archivo(ruta, std::ios::in | std::ios::out | std::ios::binary);
    archivo.seekp(sb.s_inode_start + indice * sizeof(Inodo));
    archivo.write(reinterpret_cast<const char*>(&ino), sizeof(Inodo));
    archivo.close(); 
}

static Bloque leer_bloque(const std::string& ruta, const SuperBloque& sb, int indice){
    Bloque blo;
    std::ifstream archivo(ruta, std::ios::binary);
    archivo.seekg(sb.s_block_start + indice * sizeof(Bloque));
    archivo.read(reinterpret_cast<char*>(&blo), sizeof(Bloque));
    archivo.close();
    return blo;
}

static void escribir_bloque(const std::string& ruta, const SuperBloque& sb, int indice, const Bloque& blo) {
    std::fstream archivo(ruta, std::ios::in | std::ios::out | std::ios::binary);
    archivo.seekp(sb.s_block_start + indice * sizeof(Bloque));
    archivo.write(reinterpret_cast<const char*>(&blo), sizeof(Bloque));
    archivo.close();
}

int buscar_inodo_users(const std::string& ruta_disco, const SuperBloque& sb){
    // el inodo 0 siempre es la raiz
    Inodo raiz = leer_inodo(ruta_disco, sb, 0);

    for(int b = 0; b < 12; b++){
        if(raiz.i_block[b] == -1) continue;
        Bloque bloque = leer_bloque(ruta_disco, sb, raiz.i_block[b]);
        for(int i = 0; i < 4; i++){
            if (bloque.carpeta.b_content[i].b_inodo != -1 && std::string(bloque.carpeta.b_content[i].b_name) == "users.txt") {
                return bloque.carpeta.b_content[i].b_inodo;
            }
        }
    }
    return -1;
}

std::string leer_contenido(const std::string& ruta_disco, const SuperBloque& sb, int indice_inodo){
    Inodo ino = leer_inodo(ruta_disco, sb, indice_inodo);
    std::string resultado;
    int bytes_restantes = ino.i_s;

    for(int b = 0; b < 12 && bytes_restantes > 0; b++){
        if(ino.i_block[b] == -1) break;
        Bloque bloque = leer_bloque(ruta_disco, sb, ino.i_block[b]);
        int a_leer = (bytes_restantes < 64) ? bytes_restantes : 64;
        resultado.append(bloque.archivo.b_content, a_leer);
        bytes_restantes -= a_leer;
    }
    return resultado;
}

bool escribir_contenido(const std::string& ruta_disco, const SuperBloque& sb, int indice_inodo, const std::string& nuevo_contenido){
    Inodo ino = leer_inodo(ruta_disco, sb, indice_inodo);
    int bloques_necesarios =  (nuevo_contenido.size() + 63) / 64; // Redondeo hacia arriba
    if(bloques_necesarios == 0) bloques_necesarios = 1; 
    if(bloques_necesarios > 12){
        return false;
    }

    size_t posicion = 0;
    for(int b = 0; b < bloques_necesarios; b++){
        // si este bloque directo aunto no existe, se reserva el bitmap
        if(ino.i_block[b] == -1){
            int nuevo_indice = bitmap::ocupar_primera_libre(ruta_disco, sb.s_bm_block_start, sb.s_blocks_count);
            if(nuevo_indice == -1) return false; // disco lleno
            ino.i_block[b] = nuevo_indice;
        }

        Bloque bloque;
        memset(&bloque, 0, sizeof(Bloque));
        size_t restante = nuevo_contenido.size() - posicion;
        size_t a_copiar = (restante < 64) ? restante : 64;
        memcpy(bloque.archivo.b_content, nuevo_contenido.c_str() + posicion, a_copiar);
        escribir_bloque(ruta_disco, sb, ino.i_block[b], bloque);
        posicion += a_copiar;
    }
    ino.i_s = static_cast<int>(nuevo_contenido.size());
    ino.i_mtime = time(nullptr);
    escribir_inodo(ruta_disco, sb, indice_inodo, ino);

    return true;
}
    

}