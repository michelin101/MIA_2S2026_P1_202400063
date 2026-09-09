#include "cat.hpp"
#include "../../utilidades/contexto_particion.hpp"
#include "../../utilidades/gestor_users_txt.hpp"
#include "../../estructuras/inodo.hpp"
#include "../../estructuras/bloques.hpp"
#include <vector>
#include <algorithm>
#include <fstream>
#include <sstream>

namespace comandos{

static int buscar_inodo_por_nombre_en_raiz(const std::string& ruta, const SuperBloque& sb, const std::string& nombre_archivo){
    Inodo raiz;
    std::ifstream archivo(ruta, std::ios::binary);
    archivo.seekg(sb.s_inode_start + 0 * sizeof(Inodo));
    archivo.read(reinterpret_cast<char*>(&raiz), sizeof(Inodo));

    for (int b = 0; b < 12; b++) {
        if (raiz.i_block[b] == -1) continue;
        Bloque bloque;
        archivo.seekg(sb.s_block_start + raiz.i_block[b] * sizeof(Bloque));
        archivo.read(reinterpret_cast<char*>(&bloque), sizeof(Bloque));
        for (int i = 0; i < 4; i++) {
            if (bloque.carpeta.b_content[i].b_inodo != -1 &&
                std::string(bloque.carpeta.b_content[i].b_name) == nombre_archivo) {
                archivo.close();
                return bloque.carpeta.b_content[i].b_inodo;
            }
        }
    }
    archivo.close();
    return -1;
}

std::string ejecutar_cat(const std::map<std::string, std::string>& parametros) {
    auto ctx = contexto::obtener_contexto_sesion_activa();
    if(!ctx.ok) return ctx.mensaje_error;

    // recolecta las parametros -fileN, ordenaod por su numero B
    std::vector<std::pair<int, std::string>> archivos_ordenados;
    for(const auto& par : parametros){
        if(par.first.rfind("file", 0) == 0){
            std::string numero_str = par.first.substr(4);
            try{
                int numero = std::stoi(numero_str);
                archivos_ordenados.push_back({numero, par.second});
            }catch(...){

            }
        }
    }
    if(archivos_ordenados.empty()){
        return "ERROR: CAT - debe especificar al menos un archivo con -file1, -file2, etc";
    }

    std::sort(archivos_ordenados.begin(), archivos_ordenados.end());

    std::ostringstream resultado;
    for(size_t i = 0; i < archivos_ordenados.size(); i++){
        std::string ruta_completa = archivos_ordenados[i].second;
        size_t ultima_barra = ruta_completa.find_last_of('/');
        std::string nombre_archivo = (ultima_barra == std::string::npos) ? ruta_completa : ruta_completa.substr(ultima_barra + 1);
        int indice_inodo = buscar_inodo_por_nombre_en_raiz(ctx.ruta_disco, ctx.sb, nombre_archivo);
        if(indice_inodo == -1){
            return "ERROR: CAT - el archivo '" + ruta_completa + "' no existe";
        }
        std::string contenido = gestor_users::leer_contenido(ctx.ruta_disco, ctx.sb, indice_inodo);
        resultado << contenido;
        if (i != archivos_ordenados.size() - 1) resultado << "\n";
    }
    return resultado.str();
}

}