#pragma once

// Bloque de tipo Carpeta
struct ContenidoCarpeta{
    char b_name[12];    // nombre de la carpeta o archivo
    int b_inodo;        // inodo de la carpeta o archivo   
};

struct BloqueCarpeta{
    ContenidoCarpeta b_content[4]; // 4 * (12 + 4) = 64 bytes, array con el contenido de la carpeta
};

// Bloque de tipo Archivo
struct BloqueArchivo{
    char b_content[64];
};

// Bloque de tipo Apuntadores (para indirectos)
struct BloqueApuntadores{
    int b_pointers[16];  // 16 * 4 = 64 bytes, array con los apuntadores
};

union Bloque{
    BloqueCarpeta carpeta;
    BloqueArchivo archivo;
    BloqueApuntadores apuntadores;
};