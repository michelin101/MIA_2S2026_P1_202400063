#pragma once

struct EBR {
    char part_mount;        // 0 o 1, si esta montada
    char part_fit;          // B F o w
    int part_start;         // byte del disco donde inicia la logica
    int part_s;             // tamaño en bytes
    int part_next;          // byte donde esta el siguiente EBR, -1 si no hay
    char part_name[16];     // nombre de la particion 
};