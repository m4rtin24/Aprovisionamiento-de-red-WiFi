#include <WiFi.h>
#include <WebServer.h>
#include <Preferences.h>

WebServer server(80);
Preferences preferences;

String ssidGuardado;
String passwordGuardado;

const char* apSSID = "ESP32_MOS";
const char* apPassword = "12345678";

bool conectado = false;

// Página HTML principal
String paginaConfig() {
  String html = "";
  html += "<!DOCTYPE html>";
  html += "<html lang='es'>";
  html += "<head>";
  html += "<meta charset='UTF-8'>";
  html += "<meta name='viewport' content='width=device-width, initial-scale=1.0'>";
  html += "<title>Configuración WiFi ESP32</title>";
  html += "<style>";
  html += "body{font-family:Arial;background:#f4f4f4;text-align:center;padding:30px;}";
  html += ".card{background:white;padding:25px;border-radius:12px;max-width:400px;margin:auto;box-shadow:0 4px 12px rgba(0,0,0,0.2);}";
  html += "input{width:90%;padding:12px;margin:10px;border:1px solid #ccc;border-radius:8px;}";
  html += "button{padding:12px 20px;background:#007bff;color:white;border:none;border-radius:8px;cursor:pointer;}";
  html += "button:hover{background:#0056b3;}";
  html += ".danger{background:#dc3545;}";
  html += ".danger:hover{background:#a71d2a;}";
  html += "</style>";
  html += "</head>";
  html += "<body>";
  html += "<div class='card'>";
  html += "<h2>Configuración WiFi</h2>";
  html += "<p>Ingrese el SSID y la contraseña de la red WiFi.</p>";
  html += "<form action='/guardar' method='POST'>";
  html += "<input type='text' name='ssid' placeholder='Nombre de la red WiFi' required><br>";
  html += "<input type='password' name='password' placeholder='Contraseña WiFi'><br>";
  html += "<button type='submit'>Guardar y conectar</button>";
  html += "</form>";
  html += "<br>";
  html += "<form action='/reset' method='POST'>";
  html += "<button class='danger' type='submit'>Borrar configuración</button>";
  html += "</form>";
  html += "</div>";
  html += "</body>";
  html += "</html>";

  return html;
}

// Endpoint raíz
void handleRoot() {
  server.send(200, "text/html", paginaConfig());
}

// Endpoint para guardar credenciales
void handleGuardar() {
  if (server.hasArg("ssid") && server.hasArg("password")) {
    String nuevoSSID = server.arg("ssid");
    String nuevoPassword = server.arg("password");

    preferences.begin("wifi", false);
    preferences.putString("ssid", nuevoSSID);
    preferences.putString("password", nuevoPassword);
    preferences.end();

    String respuesta = "";
    respuesta += "<html><body style='font-family:Arial;text-align:center;padding:30px;'>";
    respuesta += "<h2>Credenciales guardadas</h2>";
    respuesta += "<p>El ESP32 intentará conectarse a la red WiFi.</p>";
    respuesta += "<p>Reiniciando...</p>";
    respuesta += "</body></html>";

    server.send(200, "text/html", respuesta);

    delay(2000);
    ESP.restart();
  } else {
    server.send(400, "text/plain", "Faltan parámetros: ssid y password");
  }
}

// Endpoint para borrar credenciales
void handleReset() {
  preferences.begin("wifi", false);
  preferences.clear();
  preferences.end();

  String respuesta = "";
  respuesta += "<html><body style='font-family:Arial;text-align:center;padding:30px;'>";
  respuesta += "<h2>Configuración eliminada</h2>";
  respuesta += "<p>El ESP32 volverá al modo Access Point.</p>";
  respuesta += "<p>Reiniciando...</p>";
  respuesta += "</body></html>";

  server.send(200, "text/html", respuesta);

  delay(2000);
  ESP.restart();
}

// Endpoint tipo API para ver estado
void handleStatus() {
  String json = "{";
  json += "\"conectado\":";
  json += conectado ? "true" : "false";
  json += ",";
  json += "\"ip\":\"";
  json += WiFi.localIP().toString();
  json += "\",";
  json += "\"ssid\":\"";
  json += WiFi.SSID();
  json += "\"";
  json += "}";

  server.send(200, "application/json", json);
}

// Iniciar servidor web
void iniciarServidor() {
  server.on("/", HTTP_GET, handleRoot);
  server.on("/guardar", HTTP_POST, handleGuardar);
  server.on("/reset", HTTP_POST, handleReset);
  server.on("/status", HTTP_GET, handleStatus);

  server.begin();
  Serial.println("Servidor web iniciado");
}

// Iniciar modo AP
void iniciarModoAP() {
  Serial.println("Iniciando modo AP...");

  WiFi.mode(WIFI_AP);
  WiFi.softAP(apSSID, apPassword);

  IPAddress ip = WiFi.softAPIP();

  Serial.print("Red creada: ");
  Serial.println(apSSID);
  Serial.print("Contraseña: ");
  Serial.println(apPassword);
  Serial.print("IP del portal: ");
  Serial.println(ip);

  iniciarServidor();
}

// Intentar conexión WiFi
bool conectarWiFi(String ssid, String password) {
  Serial.println("Intentando conectar a WiFi...");
  Serial.print("SSID: ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid.c_str(), password.c_str());

  int intentos = 0;

  while (WiFi.status() != WL_CONNECTED && intentos < 20) {
    delay(500);
    Serial.print(".");
    intentos++;
  }

  Serial.println();

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("Conectado correctamente");
    Serial.print("IP asignada: ");
    Serial.println(WiFi.localIP());
    conectado = true;
    return true;
  } else {
    Serial.println("No se pudo conectar a la red WiFi");
    conectado = false;
    return false;
  }
}

// Leer credenciales guardadas
void leerCredenciales() {
  preferences.begin("wifi", true);
  ssidGuardado = preferences.getString("ssid", "");
  passwordGuardado = preferences.getString("password", "");
  preferences.end();
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println();
  Serial.println("Sistema de aprovisionamiento WiFi ESP32");

  leerCredenciales();

  if (ssidGuardado == "") {
    Serial.println("No hay credenciales guardadas");
    iniciarModoAP();
  } else {
    Serial.println("Credenciales encontradas");

    bool ok = conectarWiFi(ssidGuardado, passwordGuardado);

    if (ok) {
      iniciarServidor();
    } else {
      Serial.println("Fallo la conexión. Iniciando modo AP...");
      iniciarModoAP();
    }
  }
}

void loop() {
  server.handleClient();
}