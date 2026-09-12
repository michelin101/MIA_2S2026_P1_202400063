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

        // lee el nombre del parametro hasta '=' O hasta un espacio
        // (si encuentra espacio primero, es un flag sin valor, ej: -r, -p)
        size_t inicio_clave = i;
        while(i < n && linea[i] != '=' && !isspace(linea[i])) i++;
        std::string clave = a_minusculas(linea.substr(inicio_clave, i - inicio_clave));

        // lee el valor solo si lo que sigue es '='; si no, el valor queda vacio
        std::string valor;
        if(i < n && linea[i] == '='){
            i++; // saltar el '='

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
        }
        // si no había '=', simplemente seguimos: clave queda registrada con valor vacio (flag)
        resultado.parametros[clave] = valor;
    }
    return resultado;
} 

}