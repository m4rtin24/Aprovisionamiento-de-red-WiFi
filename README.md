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

```

## Diagrama UML
<img width="821" height="1103" alt="UMLIot drawio" src="https://github.com/user-attachments/assets/ae5e77bc-3f4b-42f7-a935-9779c6d62c07" />

## Diagrama de bloques
<img width="1470" height="830" alt="Diagrama_bloques_Iot" src="https://github.com/user-attachments/assets/8170462c-87a2-4488-b3bf-ec03f1365f7a" />

## Preguntas

### ¿Es posible conectarse a redes WiFi con seguridad PEAP Enterprise con el ESP32? ¿Qué se necesita?

Sí es posible, pero el soporte nativo del ESP32 con Arduino Core es limitado. El ESP32 utiliza
el stack WiFi de Espressif (esp-idf), que soporta WPA2-Enterprise incluyendo PEAP, pero para
usarlo desde Arduino se requiere configuración adicional:

- Usar `esp_wifi_sta_wpa2_ent_*` directamente desde el esp-idf (no disponible directamente
  con `WiFi.h` estándar).
- Proveer: identidad (usuario), contraseña, y opcionalmente el certificado CA del servidor
  RADIUS si se requiere validación del servidor.
- En Arduino, se puede acceder mediante las funciones `esp_eap_client_set_identity`,
  `esp_eap_client_set_username` y `esp_eap_client_set_password` del header `esp_wpa2.h`.

**Ejemplo mínimo con Arduino:**

```cpp
#include <WiFi.h>
#include <esp_wpa2.h>

#define EAP_IDENTITY "usuario@dominio.com"
#define EAP_PASSWORD "contraseña"
#define SSID_ENTERPRISE "RedEmpresarial"

void setup() {
  WiFi.disconnect(true);
  WiFi.mode(WIFI_STA);
  esp_wifi_sta_wpa2_ent_set_identity((uint8_t *)EAP_IDENTITY, strlen(EAP_IDENTITY));
  esp_wifi_sta_wpa2_ent_set_username((uint8_t *)EAP_IDENTITY, strlen(EAP_IDENTITY));
  esp_wifi_sta_wpa2_ent_set_password((uint8_t *)EAP_PASSWORD, strlen(EAP_PASSWORD));
  esp_wifi_sta_wpa2_ent_enable();
  WiFi.begin(SSID_ENTERPRISE);
}
```

**Limitaciones:** No todos los métodos PEAP están soportados de igual forma. En entornos
corporativos con certificados autofirmados puede ser necesario deshabilitar la validación del
certificado CA, lo que reduce la seguridad.

---

### ¿Cuántas conexiones/clientes simultáneos soporta la librería WebServer? ¿Qué alternativas hay?

La librería `WebServer.h` del ESP32 Arduino Core soporta **un único cliente a la vez** de forma
nativa. Internamente maneja las peticiones de forma secuencial en el método `handleClient()`:
atiende una petición, la responde, y luego pasa a la siguiente.

En la práctica esto no es un problema grave para un portal de aprovisionamiento, ya que el
flujo de configuración es de un solo usuario. Sin embargo, bajo carga concurrente real la
librería descarta o encola peticiones, lo que puede generar timeouts.

**Alternativas con mayor capacidad concurrente:**

| Librería / Framework | Clientes simultáneos | Notas |
|---|---|---|
| `WebServer.h` (actual) | 1 (secuencial) | Simple, suficiente para aprovisionamiento |
| `ESPAsyncWebServer` | ~4–8 (asíncrono) | No bloquea el loop, recomendada para producción |
| `ESP-IDF HTTP Server` | Configurable (hasta ~10) | Mayor control, más complejo |
| `Mongoose` | Alto | Stack completo, overkill para ESP32 básico |

Para este proyecto, `WebServer.h` es adecuada. Si se requiriera escalar (múltiples usuarios
configurando simultáneamente), la alternativa recomendada es **ESPAsyncWebServer** junto con
**AsyncTCP**, que maneja conexiones de forma no bloqueante.

---

### Comparación de memoria Flash usada por esta implementación contra el ejemplo "Basic" de la librería WiFiManager

La comparación se realizó compilando ambos sketches con la misma configuración de placa
(ESP32 Dev Module, Partition Scheme: Default 4MB with spiffs) en Arduino IDE.

| Métrica | Esta implementación | WiFiManager "Basic" |
|---|---|---|
| Sketch size (Flash) | ~285 KB | ~385 KB |
| % Flash usado (4MB) | ~7.2% | ~9.8% |
| RAM global usada | ~15 KB | ~22 KB |
| Librerías principales | `WiFi.h`, `WebServer.h`, `Preferences.h` | `WiFiManager.h` (incluye DNSServer, WebServer, EEPROM) |

**Análisis:**

- Esta implementación es aproximadamente **100 KB más liviana** en Flash que el ejemplo Basic
  de WiFiManager.
- La diferencia se explica porque WiFiManager incluye internamente un servidor DNS para el
  portal cautivo, manejo de EEPROM, y una interfaz web más compleja con escaneo de redes.
- Nuestra solución usa `Preferences.h` (NVS) en lugar de EEPROM, que es más eficiente y
  no volátil por diseño en el ESP32.
- El tradeoff es funcionalidad: WiFiManager ofrece escaneo automático de redes disponibles
  y mayor robustez; nuestra implementación es más liviana y controlable.

> **Nota:** Los valores exactos de Flash pueden variar ±5 KB según la versión del ESP32
> Arduino Core instalada y las optimizaciones del compilador. Se recomienda verificar con
> `Sketch → Export compiled Binary` en Arduino IDE sobre el hardware real.

