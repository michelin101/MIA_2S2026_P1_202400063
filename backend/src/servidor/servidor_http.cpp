#include "servidor_http.hpp"
#include "../externo/httplib.h"
#include "../despachador_global.hpp"
#include <iostream>

namespace servidor{

void iniciar(int puerto){
    httplib::Server servidor;

    servidor.set_default_headers({
        {"Access-Control-Allow-Origin", "*"},
        {"Access-Control-Allow-Methods", "POST, GET, OPTIONS"},
        {"Access-Control-Allow-Headers", "Content-Type"}
    });

    servidor.Options(".*", [](const httplib::Request&, httplib::Response& res) {
        res.status = 200;
    });

    // endpoint principal, recibe el bloque de comandos como texto plano
    servidor.Post("/ejecutar", [](const httplib::Request& req, httplib::Response& res) {
        std::string resultado = procesar_bloque(req.body);
        res.set_content(resultado, "text/plain");
    });

    // endpoint para los reportes generados
    servidor.Get(R"(/reporte/(.+))", [](const httplib::Request& req, httplib::Response& res) {
        std::string ruta = req.matches[1];
        res.set_file_content(ruta);
    });

    std::cout << "Servidor escuchando en http://localhost:" << puerto << std::endl;
    servidor.listen("0.0.0.0", puerto);
}

}