#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

const char* ssid = "";
const char* password = "";

const int relayPin = D1;
const unsigned long heartbeatTimeout = 600000; // 10 minutes
bool relayStatus = false;
unsigned long lastHeartbeat = 0;

ESP8266WebServer server(80);

void setup() {
    pinMode(relayPin, OUTPUT);
    digitalWrite(relayPin, LOW);
    relayStatus = false;

    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.println("Connecting to WiFi...");
    }
    Serial.println("Connected to WiFi");
    MDNS.begin("esp8266");
    Serial.println("MDNS responder started");

    server.on("/status", []() {
    String response = "<html><body>";
    response += "<h1>Relay and Wi-Fi Status</h1><br>";
    response += "Relay: ";
    response += relayStatus ? "Enabled<br>" : "Disabled<br>";
    response += "Wi-Fi: ";
    response += WiFi.status() == WL_CONNECTED ? "Connected<br>" : "Not Connected<br>";
    response += "</body></html>";
    server.send(200, "text/html", response);
    });

    server.on("/command/enable", []() {
        digitalWrite(relayPin, HIGH);
        relayStatus = true;
        lastHeartbeat = millis();
        server.send(200, "text/plain", "Relay enabled");
    });

    server.on("/command/disable", []() {
        digitalWrite(relayPin, LOW);
        relayStatus = false;
        lastHeartbeat = millis();
        server.send(200, "text/plain", "Relay disabled");
    });

    server.begin();
}

void loop() {
    if (WiFi.status() != WL_CONNECTED) {
        digitalWrite(relayPin, LOW);
        relayStatus = false;
        WiFi.begin(ssid, password);
        while (WiFi.status() != WL_CONNECTED) {
            delay(1000);
            Serial.println("Connecting to WiFi...");
        }
        Serial.println("Connected to WiFi");
    }

    if (relayStatus && (millis() - lastHeartbeat > heartbeatTimeout)) {
        digitalWrite(relayPin, LOW);
        relayStatus = false;
    }
    server.handleClient();
}
