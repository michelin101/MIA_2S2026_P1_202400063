#include "mkfs.hpp"
#include "../../estructuras/mbr.hpp"
#include "../../estructuras/superbloque.hpp"
#include "../../estructuras/inodo.hpp"
#include "../../estructuras/bloques.hpp"
#include "../../utilidades/manejo_binario.hpp"
#include "../../utilidades/bitmap.hpp"
#include "../../sesion/particiones_montadas.hpp"
#include <cstring>
#include <cmath>
#include <fstream>
#include <sstream>

namespace comandos{
// escribe un inodo especifico en la tabla de indos
static void escribir_inodo(const std::string& ruta, const SuperBloque& sb, int indice, const Inodo& ino){
    std::fstream archivo(ruta, std::ios::in | std::ios::out | std::ios::binary);
    int posicion = sb.s_inode_start + indice * sizeof(Inodo);
    archivo.seekp(posicion);
    archivo.write(reinterpret_cast<const char*>(&ino), sizeof(Inodo));
    archivo.close();
}

// escribe un bloque especifico en la tabla de bloques
static void escribir_bloque(const std::string& ruta, const SuperBloque& sb, int indice, const Bloque& blo){
    std::fstream archivo(ruta, std::ios::in | std::ios::out | std::ios::binary);
    int posicion = sb.s_block_start + indice * sizeof(Bloque);
    archivo.seekp(posicion);
    archivo.write(reinterpret_cast<const char*>(&blo), sizeof(Bloque));
    archivo.close();
}

std::string ejecutar_mkfs(const std::map<std::string, std::string>& parametros){
    // id (obligatorio)
    auto it_id = parametros.find("id");
    if(it_id == parametros.end()){
        return "ERROR: MFKS - el parametro -id es obligatorio";
    }
    std::string id = it_id->second;

    // type (opcional, default full), no se valida otro valor porque solo hay una opcion existente

    // busca la particon montada por ese id
    sesion::InfoMontaje info;
    if (!sesion::GestorMontajes::instancia().buscar_por_id(id, info)) {
        return "ERROR: MKFS - no existe ninguna particion montada con el id '" + id + "'";
    }
    
    MBR mbr;
    if(!manejo_binario::leer_mbr(info.ruta_disco, mbr)){
        return "ERROR: MKFS - no se pudo leer el disco";
    }
    Partition& particion = mbr.mbr_partitions[info.indice_particion_en_mbr];

    // calcular n (numero de inodos) despejando la formula
    int tam_sb = sizeof(SuperBloque);
    int tam_inodo = sizeof(Inodo);
    int tam_bloque = sizeof(Bloque);

    // tamaño_particion = tam_sb + n + 3n + n*tam_inodo + 3n*tam_bloque
    // n = (tamaño_particion - tam_sb) / (4 + tam_inodo + 3*tam_bloque)
    double numerador = particion.part_s - tam_sb;
    double denominador = 4 + tam_inodo + 3*tam_bloque;
    int n = static_cast<int>(std::floor(numerador/denominador));

    if(n <= 0){
        return "ERROR: MKFS - la particion es demasiado pequeña para crear el sistema de archivos";
    }

    int cantidad_inodos = n;
    int cantidad_bloques = 3*n;

    // calcular las posiciones (offsets) de cada estructura dentro del particion
    int inicio_particion = particion.part_start;
    int pos_bm_inode = inicio_particion + tam_sb;
    int pos_bm_block = pos_bm_inode + cantidad_inodos; // n bytes (1 byte por inodo)
    int pos_inodos = pos_bm_block + cantidad_bloques; // 3n bytes (1 byte por bloque)
    int pos_bloques = pos_inodos + cantidad_inodos * tam_inodo;

    // construir el superbloque
    SuperBloque sb;
    memset(&sb, 0, sizeof(SuperBloque));
    sb.s_filesystem_type = 2; // EXT2
    sb.s_inodes_count = cantidad_inodos;
    sb.s_blocks_count = cantidad_bloques;
    sb.s_free_blocks_count = cantidad_bloques - 1; // reserva 1 para la raiz
    sb.s_free_inodes_count = cantidad_inodos - 1; // reserva 1 para la raiz
    sb.s_mtime = time(nullptr);
    sb.s_umtime = 0;
    sb.s_mnt_count = 1;
    sb.s_magic = 0xEF53;
    sb.s_inode_s = tam_inodo;
    sb.s_block_s = tam_bloque;
    sb.s_firts_ino = 1; // el inodo 0 lo usa la raiz, el siguiente libre es el 1
    sb.s_first_blo = 1; // el bloque 0 lo usa la raiz, el siguiente libre es el 1
    sb.s_bm_inode_start = pos_bm_inode;
    sb.s_bm_block_start = pos_bm_block;
    sb.s_inode_start = pos_inodos;
    sb.s_block_start = pos_bloques;

    // escribir el superbloque en el disco
    std::fstream archivo(info.ruta_disco, std::ios::in | std::ios::out | std::ios::binary);
    archivo.seekp(inicio_particion);
    archivo.write(reinterpret_cast<char*>(&sb), sizeof(SuperBloque));
    archivo.close();

    // inicializar ambos bitmaps en 0 (libre)
    bitmap::inicializar(info.ruta_disco, sb.s_bm_inode_start, cantidad_inodos);
    bitmap::inicializar(info.ruta_disco, sb.s_bm_block_start, cantidad_bloques);
    
    // crear el inodo 0: carpta raiz
    bitmap::ocupar_primera_libre(info.ruta_disco, sb.s_bm_inode_start, cantidad_inodos); // marca inodo 0
    bitmap::ocupar_primera_libre(info.ruta_disco, sb.s_bm_block_start, cantidad_bloques); // marca bloque 0
   
    Inodo inodo_raiz;
    memset(&inodo_raiz, 0, sizeof(Inodo));
    inodo_raiz.i_uid = 1;
    inodo_raiz.i_gid = 1;
    inodo_raiz.i_s = 0;
    inodo_raiz.i_atime = inodo_raiz.i_ctime = inodo_raiz.i_mtime = time(nullptr);
    for(int i = 0; i < 15; i++){
        inodo_raiz.i_block[i] = -1; 
    }
    inodo_raiz.i_block[0] = 0; // apunta al bloque 0 (cloque carpeta)
    inodo_raiz.i_type = 0; // carpeta
    strncpy(inodo_raiz.i_perm, "664", 3);

    Bloque bloque_raiz;
    memset(&bloque_raiz, 0, sizeof(Bloque));
    strncpy(bloque_raiz.carpeta.b_content[0].b_name, ".", 12);
    bloque_raiz.carpeta.b_content[0].b_inodo = 0; // se apunta a si misma
    strncpy(bloque_raiz.carpeta.b_content[1].b_name, "..", 12);
    bloque_raiz.carpeta.b_content[1].b_inodo = 0; // la raiz es su propio padre
    bloque_raiz.carpeta.b_content[2].b_inodo = -1; // slots vacíos
    bloque_raiz.carpeta.b_content[3].b_inodo = -1;

    escribir_inodo(info.ruta_disco, sb, 0, inodo_raiz);
    escribir_bloque(info.ruta_disco, sb, 0, bloque_raiz);

    // crear users.txt: inodo 1, bloque 1
    int indice_inodo_users = bitmap::ocupar_primera_libre(info.ruta_disco, sb.s_bm_inode_start, cantidad_inodos);
    int indice_bloque_users = bitmap::ocupar_primera_libre(info.ruta_disco, sb.s_bm_block_start, cantidad_bloques);

    std::string contenido_users = "1, G, root\n1, U, root, root, 123\n";

    Inodo inodo_users;
    memset(&inodo_users, 0, sizeof(Inodo));
    inodo_users.i_uid = 1;
    inodo_users.i_gid = 1;
    inodo_users.i_s = static_cast<int>(contenido_users.size());
    inodo_users.i_atime = inodo_users.i_ctime = inodo_users.i_mtime = time(nullptr);
    for(int i = 0; i < 15; i++){
        inodo_users.i_block[i] = -1;
    }
    inodo_users.i_block[0] = indice_bloque_users;
    inodo_users.i_type = '1'; // archivo
    strncpy(inodo_users.i_perm, "664", 3);

    Bloque bloque_users;
    memset(&bloque_users, 0, sizeof(Bloque));
    strncpy(bloque_users.archivo.b_content, contenido_users.c_str(), 64);

    escribir_inodo(info.ruta_disco, sb, indice_inodo_users, inodo_users);
    escribir_bloque(info.ruta_disco, sb, indice_bloque_users, bloque_users);

    // enlazar users.txt dentro de la carpeta raiz
    Bloque raiz_actualizado;
    std::ifstream lectura(info.ruta_disco, std::ios::binary);
    lectura.seekg(sb.s_block_start + 0 * sizeof(Bloque));
    lectura.read(reinterpret_cast<char*>(&raiz_actualizado), sizeof(Bloque));
    lectura.close();

    strncpy(raiz_actualizado.carpeta.b_content[2].b_name, "users.txt", 12);
    raiz_actualizado.carpeta.b_content[2].b_inodo = indice_inodo_users;

    escribir_bloque(info.ruta_disco, sb, 0, raiz_actualizado);

    return "MKFS: sistema de archivos EXT2 creado exitosamente en id=" + id +
           " (inodos=" + std::to_string(cantidad_inodos) +
           ", bloques=" + std::to_string(cantidad_bloques) + ")";
}

}