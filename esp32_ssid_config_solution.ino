#include <WiFi.h>
#include <WebServer.h>
#include <Preferences.h>

Preferences preferences;
WebServer server(80);

String htmlPage = R"rawliteral(
<!DOCTYPE HTML><html>
<head>
  <title>ESP32 WiFi Config</title>
</head>
<body>
  <h1>ESP32 WiFi Configuration</h1>
  <p>Current SSID: %CURRENT_SSID%</p>
  <form action="/setWiFi" method="POST">
    <label for="ssid">SSID:</label>
    <input type="text" id="ssid" name="ssid"><br><br>
    <label for="password">Password:</label>
    <input type="password" id="password" name="password"><br><br>
    <input type="submit" value="Submit">
  </form>
</body>
</html>
)rawliteral";

void handleRoot() {
  String currentSSID = WiFi.SSID();
  String page = htmlPage;
  page.replace("%CURRENT_SSID%", currentSSID);
  Serial.println("Serving root page...");
  server.send(200, "text/html", page);
}

void handleSetWiFi() {
  if (server.hasArg("ssid") && server.hasArg("password")) {
    String ssid = server.arg("ssid");
    String password = server.arg("password");

    preferences.putString("ssid", ssid);
    preferences.putString("password", password);

    server.send(200, "text/html", "Credentials saved. Rebooting...");
    Serial.println("Credentials saved. Rebooting...");

    delay(1000);
    ESP.restart();
  } else {
    server.send(400, "text/html", "Bad Request");
    Serial.println("Bad Request: Missing SSID or password.");
  }
}

void setup() {
  Serial.begin(115200);

  preferences.begin("wifi-config", false);

  String ssid = preferences.getString("ssid", "JioFiber-9tPde");
  String password = preferences.getString("password", "vjng9977");

  WiFi.begin(ssid.c_str(), password.c_str());

  Serial.print("Connecting to WiFi...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nConnected to WiFi");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  server.on("/", handleRoot);
  server.on("/setWiFi", HTTP_POST, handleSetWiFi);
  server.begin();
  Serial.println("Web server started.");
}

void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    server.handleClient();
  }
}
