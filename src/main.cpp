#include <Arduino.h>
#include <WiFi.h>
#include <ESPmDNS.h>
#include <ArduinoOTA.h>

#include "config/Config.h"
#include "config/Credentials.h"
#include "motor/BTS7960Driver.h"
#include "door/DoorController.h"
#include "network/DoorWebServer.h"

#if ENCODER_ENABLED
#include "encoder/Encoder.h"
#endif

// ---------------------------------------------------------
BTS7960Driver motor(PIN_RPWM, PIN_LPWM,
                    PIN_R_EN, PIN_L_EN,
                    PIN_R_IS, PIN_L_IS);

#if ENCODER_ENABLED
Encoder encoder(PIN_ENCODER_A, PIN_ENCODER_B, ENCODER_CPR);
DoorController door(motor, PIN_ENDSTOP_OPEN, PIN_ENDSTOP_CLOSE, &encoder);
#else
DoorController door(motor, PIN_ENDSTOP_OPEN, PIN_ENDSTOP_CLOSE);
#endif

DoorWebServer webServer(door, WEB_SERVER_PORT);

// ---------------------------------------------------------
static void connectWiFi() {
    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

    Serial.print("[WiFi] Connecting");
    uint32_t start = millis();
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
        if (millis() - start > 15000) {
            Serial.println("\n[WiFi] Connection failed — restarting");
            ESP.restart();
        }
    }
    Serial.printf("\n[WiFi] Connected: %s\n", WiFi.localIP().toString().c_str());
}

// ---------------------------------------------------------
void setup() {
    Serial.begin(115200);
    Serial.println("\n=============================");
    Serial.println("  Door Manager v2.0");
    Serial.println("=============================");

    connectWiFi();

    if (MDNS.begin(MDNS_HOSTNAME)) {
        Serial.printf("[mDNS] http://%s.local\n", MDNS_HOSTNAME);
    }

    ArduinoOTA.setHostname(MDNS_HOSTNAME);
    ArduinoOTA.onStart([]() { Serial.println("[OTA] Update starting..."); });
    ArduinoOTA.onEnd([]()   { Serial.println("\n[OTA] Update complete"); });
    ArduinoOTA.onError([](ota_error_t error) { Serial.printf("[OTA] Error %u\n", error); });
    ArduinoOTA.begin();

    door.begin();
    webServer.begin();

    Serial.println("[Door Manager] Ready\n");
}

// ---------------------------------------------------------
void loop() {
    ArduinoOTA.handle();
    webServer.handle();
    door.update();

    static uint32_t lastWifiCheck = 0;
    if (millis() - lastWifiCheck > 10000) {
        lastWifiCheck = millis();
        if (WiFi.status() != WL_CONNECTED) {
            Serial.println("[WiFi] Reconnecting...");
            connectWiFi();
        }
    }

    delay(5);
}
