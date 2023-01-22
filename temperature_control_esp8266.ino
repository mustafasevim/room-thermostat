#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

const char* ssid = "";
const char* password = "";

int relayPin = D1;
unsigned long lastHeartbeat = 0;
unsigned long heartbeatTimeout = 600000;
bool relayStatus = false;

ESP8266WebServer server(80);

void setup() {
    pinMode(relayPin, OUTPUT);
    digitalWrite(relayPin, LOW);

    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.println("Connecting to WiFi...");
    }
    Serial.println("Connected to WiFi");
    if (MDNS.begin("esp8266")) {
        Serial.println("MDNS responder started");
    }

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
    if (millis() - lastHeartbeat > heartbeatTimeout) {
        digitalWrite(relayPin, LOW);
        relayStatus = false;
    }
    MDNS.update();
    server.handleClient();
}
