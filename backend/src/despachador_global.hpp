#pragma once
#include <string>

// recibe la linea de comando y retorna el mensaje de resultado
std::string procesar_comando(const std::string& linea);

// procesa un bloque de texto con multiples lineas de comando, respetando comentario y lineas en blanco
std::string procesar_bloque(const std::string& texto_completo);