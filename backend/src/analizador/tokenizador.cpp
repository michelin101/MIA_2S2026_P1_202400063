#include "tokenizador.hpp"
#include <cctype>

namespace analizador{
static std::string a_minusculas(const std::string& s){
    std::string r = s;
    for(auto& c : r) c = tolower(c);
    return r;
}

ComandoParseado parsear_linea(const std::string& linea){
    ComandoParseado resultado;
    size_t i = 0;
    size_t n = linea.size();

    // salta espacios iniciales
    while (i < n && isspace(linea[i])) i++;

    // leer el nombre del comando
    size_t inicio = i;
    while(i < n && !isspace(linea[i])) i++;
    resultado.nombre_comando = a_minusculas(linea.substr(inicio, i - inicio));

    // busca el resto de parametros
    while(i < n){
        while(i < n && isspace(linea[i])) i++;
        if( i >= n) break;

        if(linea[i] != '-'){
            resultado.error = true;
            resultado.mensaje_error = "Se esperaba un parametro que inicia con '-'";
            return resultado;
        }
        i++;

        // lee el nombre del parametro hasta =
        size_t inicio_clave = i;
        while(i < n && linea[i] != '=') i++;
        if(i >= n){
            resultado.error = true;
            resultado.mensaje_error = "Parametro sin valor, falta '='";
            return resultado;
        }
        std::string clave = a_minusculas(linea.substr(inicio_clave, i - inicio_clave));
        i++; 

        // lee el valor 
        std::string valor;
        if(i < n && linea[i] == '"'){
            i++;
            size_t inicio_valor = i;
            while(i < n && linea[i] != '"') i++;
            valor = linea.substr(inicio_valor, i - inicio_valor);
            if(i < n) i++;
        }else{
            size_t inicio_valor = i;
            while(i < n && !isspace(linea[i])) i++;
            valor = linea.substr(inicio_valor, i - inicio_valor);
        }
        resultado.parametros[clave] = valor;
    }
    return resultado;
}
}