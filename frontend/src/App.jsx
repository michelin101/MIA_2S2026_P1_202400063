import { useState, useEffect } from "react";
import EntradaComandos from "./componentes/EntradaComandos";
import SalidaComandos from "./componentes/SalidaComandos";
import BotonesAccion from "./componentes/BotonesAccion";
import { ejecutarComandos, verificarConexion } from "./servicios/api";
import "./App.css";

function App() {
  const [textoEntrada, setTextoEntrada] = useState("");
  const [textoSalida, setTextoSalida] = useState("");
  const [ejecutando, setEjecutando] = useState(false);
  const [nombreArchivo, setNombreArchivo] = useState("");
  const [estadoConexion, setEstadoConexion] = useState("verificando");

  useEffect(() => {
    const revisar = async () => {
      const ok = await verificarConexion();
      setEstadoConexion(ok ? "conectado" : "desconectado");
    };
    revisar();
    const intervalo = setInterval(revisar, 8000); // revisa cada 8 segundos
    return () => clearInterval(intervalo);
  }, []);

  const manejarEjecutar = async () => {
    if (textoEntrada.trim() === "") return;

    setEjecutando(true);
    try {
      const resultado = await ejecutarComandos(textoEntrada);
      setTextoSalida(resultado);
      setEstadoConexion("conectado");
    } catch (error) {
      setTextoSalida(
        "ERROR: no se pudo conectar con el backend.\n" +
        "Verifica que el servidor esté corriendo (./extreamfs servidor) en el puerto 8080.\n\n" +
        "Detalle: " + error.message
      );
      setEstadoConexion("desconectado");
    } finally {
      setEjecutando(false);
    }
  };

  const manejarLimpiar = () => {
    setTextoEntrada("");
    setTextoSalida("");
    setNombreArchivo("");
  };

  const manejarCargaArchivo = (contenido, nombre) => {
    setTextoEntrada(contenido);
    setNombreArchivo(nombre);
  };

  return (
    <div className="app">
      <header className="encabezado-app">
        <div className="titulo-marca">
          <span className="icono-marca">⛁</span>
          <div>
            <h1>ExtreamFS</h1>
            <p className="subtitulo">Simulador de sistema de archivos EXT2</p>
          </div>
        </div>
      </header>

      <BotonesAccion
        alEjecutar={manejarEjecutar}
        alLimpiar={manejarLimpiar}
        ejecutando={ejecutando}
        estadoConexion={estadoConexion}
      />

      <main className="contenido-principal">
        <EntradaComandos
          texto={textoEntrada}
          alCambiarTexto={setTextoEntrada}
          alCargarArchivo={manejarCargaArchivo}
          nombreArchivo={nombreArchivo}
        />
        <SalidaComandos texto={textoSalida} />
      </main>
    </div>
  );
}

export default App;