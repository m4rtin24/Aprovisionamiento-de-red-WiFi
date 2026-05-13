# Aprovisionamiento-de-red-WiFi

## Descripción del proyecto

Este proyecto implementa un sistema de aprovisionamiento WiFi para un microcontrolador ESP32. El objetivo es permitir que el usuario configure la red WiFi del dispositivo sin necesidad de modificar el código fuente cada vez que cambie de red.

Cuando el ESP32 no tiene credenciales WiFi guardadas, inicia automáticamente en modo Access Point y crea una red propia. El usuario puede conectarse a esa red desde un celular o computador, ingresar a una página web local y escribir el SSID y la contraseña de la red WiFi a la que desea conectar el dispositivo.

Una vez ingresadas las credenciales, el ESP32 las guarda en memoria no volátil y se reinicia para conectarse automáticamente a la red configurada.

---

## Objetivo

Diseñar e implementar un sistema de aprovisionamiento WiFi para ESP32 que permita:

- Configurar SSID y contraseña desde una interfaz web.
- Guardar las credenciales en memoria no volátil.
- Reconectarse automáticamente a la red guardada.
- Consultar el estado de conexión del dispositivo.
- Restablecer la configuración WiFi cuando sea necesario.

---

## Materiales utilizados

| Elemento | Descripción |
|---|---|
| ESP32 | Microcontrolador principal del proyecto |
| Cable USB | Conexión entre el ESP32 y el computador |
| Computador | Programación del ESP32 mediante Arduino IDE |
| Celular o PC | Acceso al portal de configuración WiFi |
| Red WiFi | Red a la cual se conectará el ESP32 |

---

## Herramientas y librerías utilizadas

| Herramienta / Librería | Uso |
|---|---|
| Arduino IDE | Entorno de desarrollo para programar el ESP32 |
| WiFi.h | Manejo de conexión WiFi y modo Access Point |
| WebServer.h | Creación del servidor web local |
| Preferences.h | Almacenamiento de credenciales en memoria no volátil |

---

## Funcionamiento general

El sistema funciona de la siguiente manera:

1. El ESP32 inicia.
2. Revisa si existen credenciales WiFi guardadas en memoria.
3. Si no existen credenciales, el ESP32 inicia en modo Access Point.
4. El usuario se conecta a la red creada por el ESP32.
5. El usuario abre la página de configuración en el navegador.
6. El usuario ingresa el SSID y la contraseña de su red WiFi.
7. El ESP32 guarda las credenciales en memoria no volátil.
8. El ESP32 se reinicia.
9. En el siguiente inicio, intenta conectarse automáticamente a la red configurada.
10. Si la conexión falla, vuelve al modo Access Point para permitir una nueva configuración.

---

## Red de configuración del ESP32

Cuando el ESP32 no tiene credenciales guardadas, crea una red WiFi propia con los siguientes datos:

| Parámetro | Valor |
|---|---|
| SSID | ESP32_MOS |
| Contraseña | 12345678 |
| Dirección del portal | http://192.168.4.1 |

---

## Instrucciones de uso

### 1. Cargar el código al ESP32

Primero se debe abrir el código en Arduino IDE, seleccionar la placa correspondiente al ESP32 y cargar el programa al microcontrolador.

Configuración recomendada:

| Opción | Valor |
|---|---|
| Board | ESP32 Dev Module |
| Upload Speed | 115200 |
| CPU Frequency | 240 MHz |
| Flash Frequency | 40 MHz |
| Flash Mode | DIO |
| Partition Scheme | Default 4MB |

---

### 2. Conectarse a la red del ESP32

Si el dispositivo no tiene credenciales WiFi guardadas, creará una red llamada:

```text
ESP32_MOS
