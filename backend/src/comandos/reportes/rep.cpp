#include "rep.hpp"
#include "reporte_mbr.hpp"
#include "../../sesion/particiones_montadas.hpp"
#include "../../utilidades/manejo_binario.hpp"
#include "../../utilidades/graphviz.hpp"
#include "../../estructuras/mbr.hpp"

namespace comandos{

std::string ejecutar_rep(const std::map<std::string, std::string>& parametros){
    auto it_name = parametros.find("name");
    auto it_path = parametros.find("path");
    auto it_id = parametros.find("id");

    if(it_name == parametros.end() || it_path == parametros.end() || it_id == parametros.end()){
        return "ERROR: REP - los parametros -name, -path e -id son obligatorios";
    }

    std::string tipo_reporte = it_name->second;
    std::string ruta_salida = it_path->second;
    std::string id = it_id->second;

    sesion::InfoMontaje info;
    if(!sesion::GestorMontajes::instancia().buscar_por_id(id, info)){
        return "ERROR: REP - no existe ninguna particion montada con el id '" + id + "'";
    }

    MBR mbr;
    if(!manejo_binario::leer_mbr(info.ruta_disco, mbr)){
        return "ERROR: REP - no se pudo leer el disco";
    }
    // crea carpetas padre de la ruta de salida si no existen
    manejo_binario::crear_carpetas_padre(ruta_salida);

    std::string contenido_dot;

    if(tipo_reporte == "mbr"){
        contenido_dot = reportes::generar_dot_mbr(mbr);
    }else{
        return "ERROR: REP - el tipo de reporte '" + tipo_reporte + "' no esta implementado";
    }

    if(!graphviz::generar_imagen(contenido_dot, ruta_salida)){
        return "ERROR: REP - no se pudo generar la imagen con Graphviz";
    }

    return "REP: reporte '" + tipo_reporte + "' generado exitosamente en " + ruta_salida;
}

}