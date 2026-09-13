#include "texto_dot.hpp"

namespace texto_dot {

std::string sanear_para_html(const std::string& entrada) {
    std::string resultado;
    for (char c : entrada) {
        if (c == '\0') break; 
        switch (c) {
            case '&':  resultado += "&amp;";  break;
            case '<':  resultado += "&lt;";   break;
            case '>':  resultado += "&gt;";   break;
            case '"':  resultado += "&quot;"; break;
            case '\n': resultado += "<br/>";  break;
            case '\r': break; 
            default:   resultado += c;
        }
    }
    return resultado;
}

} 