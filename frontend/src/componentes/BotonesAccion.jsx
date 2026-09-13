export default function BotonesAccion({ alEjecutar, alLimpiar, ejecutando, estadoConexion }) {
  const infoEstado = {
    conectado: { texto: "Backend conectado", clase: "estado-ok" },
    desconectado: { texto: "Backend no disponible", clase: "estado-error" },
    verificando: { texto: "Verificando conexión...", clase: "estado-verificando" },
  }[estadoConexion];

  return (
    <div className="barra-acciones">
      <div className="indicador-estado">
        <span className={`punto-estado ${infoEstado.clase}`}></span>
        <span className="texto-estado">{infoEstado.texto}</span>
      </div>

      <div className="grupo-botones">
        <button className="boton boton-secundario" onClick={alLimpiar}>
          Limpiar todo
        </button>
        <button
          className="boton boton-primario"
          onClick={alEjecutar}
          disabled={ejecutando}
        >
          {ejecutando ? (
            <>
              <span className="spinner"></span> Ejecutando...
            </>
          ) : (
            <>▶ Ejecutar</>
          )}
        </button>
      </div>
    </div>
  );
}