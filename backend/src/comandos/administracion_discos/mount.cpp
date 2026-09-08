#include "mount.hpp"
#include "../../estructuras/mbr.hpp"
#include "../../utilidades/manejo_binario.hpp"
#include "../../sesion/particiones_montadas.hpp"
#include "../../configuracion.hpp"
#include <cstring>
#include <algorithm>

namespace comandos{

std::string ejecutar_mount(const std::map<std::string, std::string>& parametros){
    // path (obligatorio)
    auto it_path = parametros.find("path");
    if(it_path == parametros.end()){
        return "ERROR: MOUNT - el parametro -path es obligatorio";
    }
    std::string ruta = it_path->second;

    // name (obligatorio)
    auto it_name = parametros.find("name");
    if(it_name == parametros.end()){
        return "ERROR: MOUNT - el parametro -names es obligatorio";
    }
    std::string nombre = it_name->second;

    // leer el MBR del disco
    MBR mbr;
    if(!manejo_binario::leer_mbr(ruta, mbr)){
        return "ERROR: MOUNT - el disco '" + ruta + "' no existe";
    }

    // buscar la particion por nombre
    int indice_encontrado = -1;
    for(int i = 0; i <4; i++){
        if(mbr.mbr_partitions[i].part_s > 0 && mbr.mbr_partitions[i].part_type == 'P' && std::string(mbr.mbr_partitions[i].part_name) == nombre){
            indice_encontrado = i;
            break;
        }
    }

    if(indice_encontrado == -1){
        return "ERROR: MOUNT - no existe la particion primaria '" + nombre + "' en el disco indicado";
    }

    Partition& particion  = mbr.mbr_partitions[indice_encontrado];

    // validacion que ve que no este y amontada
    std::string id_existente;
    if (sesion::GestorMontajes::instancia().ya_montada(ruta, nombre, id_existente)) {
        return "ERROR: MOUNT - la partición '" + nombre + "' ya se encuentra montada en esta sesión (id=" + id_existente + ").";
    }

    // genera el id usando el signature unico del disco
    std::string id_generado = sesion::GestorMontajes::instancia().generar_id(mbr.mbr_dsk_signature, configuracion::ULTIMOS_DOS_CARNET);

    particion.part_status = '1';

    int correlativo = 0;
    for(int i = 0; i < 4; i++){
        if(mbr.mbr_partitions[i].part_status == '1') correlativo++;
    }
    particion.part_correlative = correlativo;

    memset(particion.part_id, 0, sizeof(particion.part_id));
    memcpy(particion.part_id, id_generado.c_str(), std::min(sizeof(particion.part_id), id_generado.size()));

    // persiste el cambio en el disco
    if(!manejo_binario::escribir_mbr(ruta, mbr)){
        return "ERROR: MOUNT - no se pudo actualizar el MBR en el disco";
    }

    // registrar el montaje en la sesion ram
    sesion::InfoMontaje info;
    info.ruta_disco = ruta;
    info.nombre_particion = nombre;
    info.indice_particion_en_mbr = indice_encontrado;
    sesion::GestorMontajes::instancia().registrar_montaje(id_generado, info);
    return "MOUNT: particon '" + nombre + "' montada exitosamente con id="+ id_generado;
}
}