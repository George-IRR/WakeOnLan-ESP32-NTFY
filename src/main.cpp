#include "secrets.h"

const char* ssid      = WIFI_SSID;
const char* password  = WIFI_PASS;
const char* topic_url = NTFY_TOPIC;
const char* targetMAC = TARGET_MAC;

#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <WiFiUdp.h>
#include <WakeOnLan.h>

#define uS_TO_S_FACTOR 1000000ULL
#define TIME_TO_SLEEP  25

WiFiUDP UDP;
WakeOnLan WOL(UDP);

void print_wakeup_reason() {
  esp_sleep_wakeup_cause_t wakeup_reason = esp_sleep_get_wakeup_cause();
  switch(wakeup_reason) {
    case ESP_SLEEP_WAKEUP_TIMER: Serial.println("Stare: Trezire din Deep Sleep (Temporizator RTC)."); break;
    default: Serial.printf("Stare: Pornire initiala / Reset hardware. Cod motiv: %d\n", wakeup_reason); break;
  }
}

void setup() {
  Serial.begin(9600);
  delay(1000); 
  Serial.println("\n\n--- Initializare Sistem ESP32 ---");
  print_wakeup_reason();

  Serial.print("Proces: Conectare WiFi [");
  Serial.print(ssid);
  Serial.print("]");
  
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) { 
    delay(500); 
    Serial.print(".");
  }
  Serial.print("\nStare: WiFi Conectat. IP Alocat: ");
  Serial.println(WiFi.localIP());

  Serial.println("Proces: Initiere interogare HTTP GET (ntfy.sh)...");
  HTTPClient http;
  http.begin(topic_url);
  
  int httpCode = http.GET();
  Serial.printf("Stare: Cod raspuns HTTP: %d\n", httpCode);

  if (httpCode == HTTP_CODE_OK) {
    String payload = http.getString();
    Serial.println("Proces: Evaluare buffer memorie cache...");
    Serial.print("Continut payload:\n[START]\n");
    Serial.print(payload);
    Serial.println("\n[STOP]");
    
    if (payload.indexOf("WAKE") != -1) {
      Serial.println("Decizie: Comanda WAKE detectata pozitiv.");
      Serial.println("Proces: Generare si transmisie Magic Packet...");
      WOL.setRepeat(3, 100);
      WOL.calculateBroadcastAddress(WiFi.localIP(), WiFi.subnetMask());
      WOL.sendMagicPacket(targetMAC);
      Serial.println("Stare: Magic Packet expediat pe subretea.");
    } else {
      Serial.println("Decizie: Comanda WAKE absenta. Nicio actiune executata.");
    }
  } else {
    Serial.printf("Eroare: Interogare HTTP eșuată. Detalii: %s\n", http.errorToString(httpCode).c_str());
  }
  
  http.end();
  WiFi.disconnect(true);
  Serial.println("Stare: Modul WiFi hardware dezactivat.");

  Serial.printf("Proces: Initiere mod Deep Sleep. Temporizator configurat la %d secunde.\n", TIME_TO_SLEEP);
  Serial.println("--- Suspendare Executie ---");
  Serial.flush(); 

  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
  esp_deep_sleep_start();
}

void loop() {}