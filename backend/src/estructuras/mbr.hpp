#pragma once
#include <ctime>

struct Partition{
    char part_status;               // indica si esta montada = 1, o inactiva = 0
    char part_type;                 // tipo de particion P = primaria, E = extendida
    char part_fit;                  // tipo de ajuste de la particon, B, F, W
    int part_start;                 // byte donde inicia la particion del disco
    int part_s;                     // tamaño en bytes
    char part_name[16];             // nombre de la particion
    int part_correlative;           // -1 hasta que se monte
    char part_id[4];                // id generado al montar
};

struct MBR{
    int mbr_tamano;                 // tamaño total del disco en bytes
    time_t mbr_fecha_creacion;      // fecha/hora de creacion
    int mbr_dsk_signature;          // numero random unico
    char dsk_fit;                   // ajuste del disco, B, F, W
    Partition mbr_partitions[4];    // 4 particiones
};
