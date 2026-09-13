import { useRef } from "react";

export default function EntradaComandos({ texto, alCambiarTexto, alCargarArchivo, nombreArchivo }) {
  const inputArchivoRef = useRef(null);

  const manejarSeleccionArchivo = (evento) => {
    const archivo = evento.target.files[0];
    if (!archivo) return;

    const lector = new FileReader();
    lector.onload = (e) => {
      alCargarArchivo(e.target.result, archivo.name);
    };
    lector.readAsText(archivo);
    evento.target.value = "";
  };

  const numeroLineas = texto.split("\n").length;

  return (
    <div className="panel panel-entrada">
      <div className="encabezado-panel">
        <span className="punto punto-entrada"></span>
        <label>Entrada</label>
        {nombreArchivo && <span className="etiqueta-archivo">{nombreArchivo}</span>}
      </div>

      <div className="barra-herramientas">
        <button className="boton boton-secundario" onClick={() => inputArchivoRef.current.click()}>
          Cargar script .smia
        </button>
        <input
          ref={inputArchivoRef}
          type="file"
          accept=".smia"
          style={{ display: "none" }}
          onChange={manejarSeleccionArchivo}
        />
        <span className="contador-lineas">{numeroLineas} líneas</span>
      </div>

      <textarea
        className="area-texto"
        value={texto}
        onChange={(e) => alCambiarTexto(e.target.value)}
        spellCheck={false}
        placeholder="mkdisk -size=10 -unit=M -path=/home/user/disco1.mia&#10;fdisk -size=300 -path=/home/user/disco1.mia -name=Part1&#10;..."
      />
    </div>
  );
}