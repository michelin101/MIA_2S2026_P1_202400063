#include "particiones_montadas.hpp"

namespace sesion{

GestorMontajes& GestorMontajes::instancia(){
    static GestorMontajes unica_instancia;
    return unica_instancia;
}

std::string GestorMontajes::generar_id(int signature_disco, const std::string& ultimos_dos_carnet){
    auto it = letra_por_disco_.find(signature_disco);
    char letra;
    int numero;

    if(it != letra_por_disco_.end()){
        // disco ya montado, reutilizar su letra, incrementar su numero
        letra = it->second;
        numero = siguiente_numero_por_disco_[signature_disco];
        siguiente_numero_por_disco_[signature_disco] = numero + 1;
    }else{
        // disco nuevo
        letra = siguiente_letra_disponible_;
        siguiente_letra_disponible_++;
        letra_por_disco_[signature_disco] = letra;
        numero = 1;
        siguiente_numero_por_disco_[signature_disco] = 2;
    }
    return ultimos_dos_carnet + std::to_string(numero) + std::string(1, letra);
}

void GestorMontajes::registrar_montaje(const std::string& id, const InfoMontaje& info){
    montajes_[id] = info;
}

bool GestorMontajes::buscar_por_id(const std::string& id, InfoMontaje& info_salida){
    auto it = montajes_.find(id);
    if(it == montajes_.end()) return false;
    info_salida = it->second;
    return true;
}

std::vector<std::string> GestorMontajes::listar_ids(){
    std::vector<std::string> ids;
    for(const auto& par : montajes_){
        ids.push_back(par.first);
    }
    return ids;
}

bool GestorMontajes::ya_montada(const std::string& ruta_disco, const std::string& nombre_particion,
                                 std::string& id_salida) {
    for (const auto& par : montajes_) {
        if (par.second.ruta_disco == ruta_disco && par.second.nombre_particion == nombre_particion) {
            id_salida = par.first;
            return true;
        }
    }
    return false;
}

}