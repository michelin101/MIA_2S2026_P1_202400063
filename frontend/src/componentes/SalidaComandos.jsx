function obtenerClaseLinea(linea) {
  const limpia = linea.trim();
  if (limpia.startsWith("ERROR")) return "linea-error";
  if (limpia.startsWith("#")) return "linea-comentario";
  if (limpia === "") return "linea-vacia";
  // cualquier línea que empiece con el nombre de un comando en mayúsculas
  // seguido de ":" la tratamos como resultado exitoso
  if (/^[A-Z_]+:/.test(limpia)) return "linea-exito";
  return "linea-normal";
}

export default function SalidaComandos({ texto }) {
  const lineas = texto.split("\n");

  return (
    <div className="panel panel-salida">
      <div className="encabezado-panel">
        <span className="punto punto-salida"></span>
        <label>Salida</label>
      </div>
      <div className="area-salida">
        {texto === "" ? (
          <p className="placeholder-salida">
            # Aquí se mostrarán los mensajes de la ejecución
          </p>
        ) : (
          lineas.map((linea, indice) => (
            <div key={indice} className={`linea-salida ${obtenerClaseLinea(linea)}`}>
              {linea || "\u00A0"}
            </div>
          ))
        )}
      </div>
    </div>
  );
}