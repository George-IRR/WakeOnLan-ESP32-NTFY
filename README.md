# ESP32 WOL Bridge

An ultra-low-power gateway designed to trigger **Wake-on-LAN (WOL)** magic packets remotely using the **ntfy.sh** infrastructure. This project is optimized for extreme battery efficiency by minimizing the time the WiFi radio is active.

---

## 🚀 Key Features

* **Ultra-Fast WiFi Association:** Uses **BSSID Locking** to skip channel scanning and **Static IP** to bypass the DHCP DORA process.
* **Deep Sleep Optimization:** Leverages the ESP32 RTC timer to enter deep sleep between polling cycles, reducing consumption to microamps.
* **Hardware-Level Power Saving:** Forcefully shuts down the WiFi radio hardware before sleep to prevent residual power leakage.
* **ntfy.sh Integration:** Uses intermittent polling with cache downloading to minimize "on-air" time.
* **Production-Ready Debugging:** Includes conditional compilation to strip all Serial/UART overhead from the production binary for faster execution.

---

## 🛠️ Hardware & Software Requirements

* **Microcontroller:** ESP32 (DevKit V1 or similar).
* **Framework:** Arduino (via PlatformIO).
* **Libraries:** * `a7md0/WakeOnLan@^1.1.7`
* Standard `WiFi`, `HTTPClient`, and `WiFiUdp` libraries.



---

## 📋 Configuration

Before flashing, you must configure your local network settings.

1. Rename `include/secrets.h.example` to `include/secrets.h`.


2. Update the following parameters with your specific data:

| Parameter | Description | Example Value |
| --- | --- | --- |
| `WIFI_SSID` | Your WiFi Name |`"Your_SSID"` 
| `NTFY_TOPIC` | Your private ntfy topic | `"https://ntfy.sh/my_private_wol_topic"` 
| `TARGET_MAC` | MAC Address of the PC to wake | `"AA:BB:CC:DD:EE:FF"` 
| `LOCAL_IP` | Fixed IP for the ESP32 | `192.168.1.200` 
| `GATEWAY` | Router IP address | `192.168.1.1` 
| `SUBNET` | Network Mask | `255.255.255.0` 

---

## ⏱️ Performance Specifications

Based on version 1.0.0 optimizations:

* **Active Time:** < 4 seconds per cycle (from wake to sleep).
* **Deep Sleep Duration:** 25 seconds.
* **Total Cycle Time:** ~29 seconds.
* **Reception Reliability:** Uses `since=30s` on the ntfy API to ensure no messages are missed while the device is sleeping.

---

## 💾 Installation & Flashing

### Using PlatformIO

To build and flash the project:

1. Open the project in **VS Code** with the **PlatformIO** extension.
2. (Optional) Enable debugging by uncommenting `-D ENABLE_DEBUG` in `platformio.ini`.
3. Click **Upload**.

