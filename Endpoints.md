# Documentación de Endpoints — ESP32 WiFi Provisioning
 
Base URL (modo AP):  `http://192.168.4.1`  
Base URL (modo STA): `http://<IP_asignada_por_router>`
 
---
 
## `GET /`
 
Retorna la interfaz web de configuración WiFi.
 
| Campo   | Valor |
|---------|-------|
| Método  | `GET` |
| URL     | `/` |
| Auth    | Ninguna |
 
**Headers de solicitud**
 
| Header | Valor | Requerido |
|--------|-------|-----------|
| `Accept` | `text/html` | No |
 
**Query params:** Ninguno  
**Request body:** Ninguno
 
**Respuesta exitosa — `200 OK`**
 
```
Content-Type: text/html; charset=utf-8
```
 
```html
<!DOCTYPE html>
<html lang='es'>
  <head>...</head>
  <body>
    <div class='card'>
      <h2>Configuración WiFi</h2>
      <form action='/guardar' method='POST'>
        <input type='text'     name='ssid'     placeholder='Nombre de la red WiFi' required>
        <input type='password' name='password' placeholder='Contraseña WiFi'>
        <button type='submit'>Guardar y conectar</button>
      </form>
      <form action='/reset' method='POST'>
        <button class='danger' type='submit'>Borrar configuración</button>
      </form>
    </div>
  </body>
</html>
```
 
---
 
## `POST /guardar`
 
Recibe las credenciales WiFi ingresadas por el usuario, las persiste en memoria no volátil (NVS mediante `Preferences.h`) y reinicia el dispositivo para intentar la conexión.
 
| Campo   | Valor |
|---------|-------|
| Método  | `POST` |
| URL     | `/guardar` |
| Auth    | Ninguna |
 
**Headers de solicitud**
 
| Header | Valor | Requerido |
|--------|-------|-----------|
| `Content-Type` | `application/x-www-form-urlencoded` | Sí |
 
**Request body — form-data**
 
| Campo      | Tipo   | Requerido | Descripción |
|------------|--------|-----------|-------------|
| `ssid`     | string | Sí        | Nombre de la red WiFi (SSID) a la que se conectará el ESP32 |
| `password` | string | No        | Contraseña de la red WiFi. Puede enviarse vacío para redes abiertas |
 
**Ejemplo de solicitud**
 
```
POST http://192.168.4.1/guardar
Content-Type: application/x-www-form-urlencoded
 
ssid=MiRedCasa&password=miContraseña123
```
 
**Respuesta exitosa — `200 OK`**
 
```
Content-Type: text/html; charset=utf-8
```
 
```html
<html>
  <body style='font-family:Arial;text-align:center;padding:30px;'>
    <h2>Credenciales guardadas</h2>
    <p>El ESP32 intentará conectarse a la red WiFi.</p>
    <p>Reiniciando...</p>
  </body>
</html>
```
 
> El dispositivo ejecuta `ESP.restart()` tras 2 segundos. La conexión HTTP se cierra.
 
**Respuesta de error — `400 Bad Request`**
 
Ocurre cuando no se envía el campo `ssid`.
 
```
Content-Type: text/plain
```
 
```
Faltan parámetros: ssid y password
```
 
---
 
## `GET /status`
 
Retorna el estado actual de conexión del ESP32 en formato JSON.
 
| Campo   | Valor |
|---------|-------|
| Método  | `GET` |
| URL     | `/status` |
| Auth    | Ninguna |
 
**Headers de solicitud**
 
| Header | Valor | Requerido |
|--------|-------|-----------|
| `Accept` | `application/json` | No |
 
**Query params:** Ninguno  
**Request body:** Ninguno
 
**Ejemplo de solicitud**
 
```
GET http://192.168.4.1/status
```
 
**Respuesta exitosa — `200 OK` (conectado)**
 
```
Content-Type: application/json
```
 
```json
{
  "conectado": true,
  "ip": "192.168.1.105",
  "ssid": "MiRedCasa"
}
```
 
**Respuesta exitosa — `200 OK` (no conectado / modo AP)**
 
```
Content-Type: application/json
```
 
```json
{
  "conectado": false,
  "ip": "0.0.0.0",
  "ssid": ""
}
```
 
| Campo       | Tipo    | Descripción |
|-------------|---------|-------------|
| `conectado` | boolean | `true` si el ESP32 está conectado a una red WiFi como cliente (modo STA) |
| `ip`        | string  | Dirección IP asignada por el router. `"0.0.0.0"` si no hay conexión |
| `ssid`      | string  | Nombre de la red a la que está conectado. Vacío si no hay conexión |
 
---
 
## `POST /reset`
 
Borra las credenciales WiFi almacenadas en NVS y reinicia el dispositivo. El ESP32 vuelve al modo Access Point (Flujo 1).
 
| Campo   | Valor |
|---------|-------|
| Método  | `POST` |
| URL     | `/reset` |
| Auth    | Ninguna |
 
**Headers de solicitud**
 
| Header | Valor | Requerido |
|--------|-------|-----------|
| `Content-Type` | `application/x-www-form-urlencoded` | No |
 
**Query params:** Ninguno  
**Request body:** Ninguno (el formulario HTML envía un POST vacío)
 
**Ejemplo de solicitud**
 
```
POST http://192.168.4.1/reset
```
 
**Respuesta exitosa — `200 OK`**
 
```
Content-Type: text/html; charset=utf-8
```
 
```html
<html>
  <body style='font-family:Arial;text-align:center;padding:30px;'>
    <h2>Configuración eliminada</h2>
    <p>El ESP32 volverá al modo Access Point.</p>
    <p>Reiniciando...</p>
  </body>
</html>
```
 
> El dispositivo ejecuta `preferences.clear()` seguido de `ESP.restart()` tras 2 segundos.
 
---
 
## Resumen de endpoints
 
| Método | URL       | Descripción                                      | Content-Type respuesta    |
|--------|-----------|--------------------------------------------------|---------------------------|
| GET    | `/`       | Interfaz web de configuración WiFi               | `text/html`               |
| POST   | `/guardar`| Guarda SSID y contraseña, reinicia el ESP32      | `text/html`               |
| GET    | `/status` | Estado de conexión en JSON                       | `application/json`        |
| POST   | `/reset`  | Borra credenciales y reinicia al modo AP         | `text/html`               |
 
---
 
## Notas de implementación
 
- El servidor web está implementado con la librería `WebServer.h` (ESP32 Arduino Core).
- Las credenciales se persisten usando `Preferences.h` bajo el namespace `"wifi"` con las claves `"ssid"` y `"password"`.
- Todos los endpoints funcionan tanto en **modo AP** (`192.168.4.1`) como en **modo STA** (IP dinámica asignada por el router).
- No se implementa autenticación dado que el servidor solo es accesible desde la red local o la red AP del propio dispositivo.
 
