#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <WiFiUdp.h>
#include <WakeOnLan.h>
#include "secrets.h"

// Directive Preprocesor Depanare
#ifdef ENABLE_DEBUG
  #define DEBUG_BEGIN(baud) Serial.begin(baud)
  #define DEBUG_PRINT(x)    Serial.print(x)
  #define DEBUG_PRINTLN(x)  Serial.println(x)
  #define DEBUG_PRINTF(...) Serial.printf(__VA_ARGS__)
#else
  #define DEBUG_BEGIN(baud)
  #define DEBUG_PRINT(x)
  #define DEBUG_PRINTLN(x)
  #define DEBUG_PRINTF(...)
#endif


#define uS_TO_S_FACTOR 1000000ULL
#define TIME_TO_SLEEP  25

WiFiUDP UDP;
WakeOnLan WOL(UDP);

void setup() {
  DEBUG_BEGIN(115200);
  delay(500); // Latență minimă permisă exclusiv pentru inițializarea consolei
  DEBUG_PRINTLN("\n[SISTEM] Iesire Deep Sleep.");

  WiFi.mode(WIFI_STA);
  WiFi.config(LOCAL_IP, GATEWAY, SUBNET, DNS_PRIMARY);
  WiFi.begin(WIFI_SSID, WIFI_PASS, 0, BSSID_ROUTER);

  DEBUG_PRINT("[WIFI] Conectare");
  while (WiFi.status() != WL_CONNECTED) { 
    delay(50); 
    DEBUG_PRINT(".");
  }
  /* Raportare BSSID Router
  DEBUG_PRINTLN("\n[WIFI] Conectat.");
  DEBUG_PRINT("[BSSID CURENT]: ");
  DEBUG_PRINTLN(WiFi.BSSIDstr()); 
  */
  DEBUG_PRINTLN("\n[WIFI] Conectat.");

  HTTPClient http;
  http.begin(NTFY_TOPIC);
  
  DEBUG_PRINTLN("[HTTP] Initiere cerere GET.");
  int httpCode = http.GET();
  
  if (httpCode == HTTP_CODE_OK) {
    String payload = http.getString();
    DEBUG_PRINTF("[HTTP] Raspuns valid. Lungime: %d bytes.\n", payload.length());
    
    if (payload.indexOf("WAKE") != -1) {
      DEBUG_PRINTLN("[WOL] Comanda WAKE validata. Transmisie Magic Packet.");
      WOL.setRepeat(3, 100);
      WOL.calculateBroadcastAddress(LOCAL_IP, SUBNET);
      WOL.sendMagicPacket(TARGET_MAC);
    } else {
      DEBUG_PRINTLN("[WOL] Nicio comanda WAKE identificata in buffer.");
    }
  } else {
    DEBUG_PRINTF("[HTTP] Eroare interogare. Cod: %d\n", httpCode);
  }
  
  http.end();
  WiFi.disconnect(true);
  DEBUG_PRINTLN("[SISTEM] WiFi dezactivat. Intrare Deep Sleep.");

  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
  esp_deep_sleep_start();
}

void loop() {}