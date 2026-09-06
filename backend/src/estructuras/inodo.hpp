#pragma once
#include <ctime>

struct Inodo{
    int i_uid;          // UID del usuario propietario del archivo o carpeta
    int i_gid;          // GID del grupo al que pertenece el archivo o carpeta
    int i_s;            // tamaño del archivo o carpeta en bytes
    time_t i_atime;     // ultima fecha en que se leyo el inodo sin modificarlo
    time_t i_ctime;     // fecha en la que se creo el inodo
    time_t i_mtime;     // ultima fecha en la que se modifico el inodo
    int i_block[15];    // 0-11: bloques directos, 12: bloque simple indirecto, 13: doble indirecto, 14: triple indirecto
    char i_type;        // 1 archivo, 0 carpeta
    char i_perm[3];     // guarda el permiso usando UGO en octal 
};