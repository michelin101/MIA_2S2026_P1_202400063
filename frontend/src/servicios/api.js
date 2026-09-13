
const URL_BASE = "http://localhost:8080";

export async function ejecutarComandos(textoComandos) {
  const respuesta = await fetch(`${URL_BASE}/ejecutar`, {
    method: "POST",
    headers: { "Content-Type": "text/plain" },
    body: textoComandos,
  });

  if (!respuesta.ok) {
    throw new Error("El servidor respondió con un error.");
  }

  return await respuesta.text();
}

export async function verificarConexion() {
  try {
    const respuesta = await fetch(`${URL_BASE}/ejecutar`, {
      method: "POST",
      headers: { "Content-Type": "text/plain" },
      body: "",
    });
    return respuesta.ok;
  } catch {
    return false;
  }
}