#pragma once
#include <map>
#include <string>
#include <vector>

namespace sesion{
struct InfoMontaje{
    std::string ruta_disco;
    std::string nombre_particion;
    int indice_particion_en_mbr;
};

class GestorMontajes{
public:
    static GestorMontajes& instancia();
    // genera el id para montar una particion de un disco
    std::string generar_id(int signature_disco, const std::string& ultimos_dos_carnet);
    // registra una particion como montada
    void registrar_montaje(const std::string& id, const InfoMontaje& info);
    // busca si un id existe y retorna su infor
    bool buscar_por_id(const std::string& id, InfoMontaje& info_salida);
    // retorna todos los ids montados actuales
    std::vector<std::string> listar_ids();

private:
    GestorMontajes() = default;
    // letra que se le asigno
    std::map<int, char> letra_por_disco_;
    char siguiente_letra_disponible_ = 'A';
    // siguiente numero de particion a usar en ese dico
    std::map<int, int> siguiente_numero_por_disco_;
    // info de la particion montada
    std::map<std::string, InfoMontaje> montajes_;
};

}