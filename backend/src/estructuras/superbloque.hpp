#pragma once
#include <ctime>

struct SuperBloque{
    int s_filesystem_type;      // 2 = EXT2
    int s_inodes_count;         // total de inodos
    int s_blocks_count;         // total de bloques 
    int s_free_blocks_count;    // total de bloques libres
    int s_free_inodes_count;    // total de inodos libres
    time_t s_mtime;             // ultima vez montado
    time_t s_umtime;            // ultima vez desmontado
    int s_mnt_count;            // cuantas veces se ha montado
    int s_magic;                // 0xEF53, valor que identifica el sistema
    int s_inode_s;              // tamaño de un inodo (bytes)
    int s_block_s;              // tamño de un bloque (bytes y siempre 64)
    int s_firts_ino;            // direccion del primer nodo libre
    int s_first_blo;            // direccion del primer bloque libre
    int s_bm_inode_start;       // byte donde inicia el bitmap de inodos
    int s_bm_block_start;       // byte donde inicia el bitman de bloques
    int s_inode_start;          // byte donde inicia la tabla de inodos
    int s_block_start;          // byte donde inicia la tabla de bloques
};