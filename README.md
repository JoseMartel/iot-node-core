# IoT Node Core: High-Performance Generic Firmware

A robust, modular, and memory-efficient IoT firmware built with **ESP-IDF** for the **ESP32-C6 Mini**. Designed for mission-critical applications where data integrity and professional user experience are paramount.

## 🚀 Key Features

*   **Binary-First Data Flow**: Internal communication uses C structures (`iot_packet_t`) instead of JSON to minimize RAM, CPU cycles, and stack usage.
*   **Hybrid LIFO Buffer Strategy**:
    *   **First Tier (RAM)**: Real-time LIFO queue for fast processing.
    *   **Second Tier (NVS)**: Persistent circular buffer in Flash (up to 1024 records) for offline resilience.
*   **"Zero-Touch" Provisioning**:
    *   **Smart Fallback**: Automatic switch to Access Point mode if saved WiFi credentials fail.
    *   **Captive Portal**: DNS Hijacking (spoofing) forces the configuration dashboard to open automatically upon connection.
*   **Dual-Interval Control**: Independent parameters for **Reading Frequency** (data resolution) and **Transmission Frequency** (battery/data optimization).
*   **Robust Transmission (ACK Logic)**:
    *   Full HTTP status code handling (200 OK, 4xx Malformed, 5xx/Connection Failures).
    *   Automatic data re-routing to NVS upon network instability.
*   **Over-The-Air (OTA) Ready**: Partitioned for dual-app updates with a 1.6MB application allowance.

---

## 📂 Project Structure (Modular ESP-IDF)

```text
iot-node-core/
├── components/
│   ├── config_manager/      # NVS storage for WiFi, intervals, and system params.
│   ├── network_manager/     # Connectivity FSM (STA + AP Fallback).
│   ├── storage_manager/     # Circular LIFO Buffer implementation on NVS Blobs.
│   ├── protocol_manager/    # Cloud Client with JIT (Just-In-Time) JSON conversion.
│   ├── sensor_manager/      # Binary data acquisition HAL.
│   ├── web_server/          # REST API, DNS Server, and embedded SPA Dashboard.
│   └── system_utils/        # Global types (iot_packet_t) and health monitoring.
├── main/
│   └── main.c               # Multi-threaded orchestration (FreeRTOS).
├── partitions.csv           # Custom 4MB Flash layout (128KB NVS, Dual OTA).
└── sdkconfig                # Optimized ESP32-C6 hardware configuration.
```

---

## 🔄 Data Pipeline (Robustness Architecture)

The system prioritizes **Freshness** and **Integrity** through an inverse-priority pipeline:

1.  **Acquisition**: `sensor_task` samples data at `reading_interval` and pushes it into a RAM Queue.
2.  **LIFO Routing**: 
    *   **Online**: `sender_task` extracts the **latest** data from RAM first. After RAM is empty, it drains the historical data from NVS.
    *   **Offline**: If network is lost, the RAM Queue is immediately flushed into the NVS Circular Buffer to prevent data loss.
3.  **ACK Confirmation**: Data is only removed from the system (Flash or RAM) if the server returns a `200 OK`. If the message is malformed (`4xx`), it is discarded to prevent buffer blockage.

---

## 🛠️ Configuration & Setup

### Requirements
*   ESP-IDF v5.2 or higher.
*   Hardware: ESP32-C6 Mini (4MB Flash).

### Menuconfig Settings
To ensure correct operation, the following `idf.py menuconfig` settings are required:
*   **Partition Table**: Custom CSV (`partitions.csv`), Offset `0x8000`.
*   **Flash Size**: 4 MB.
*   **HTTP Server**: Max Header/Content Length set to `1024`.
*   **WiFi**: Enable WiFi 6 Support for ESP32-C6 efficiency.

### Build & Flash
```bash
get_idf
idf.py set-target esp32c6
idf.py build
idf.py flash monitor
```

---

## 🌐 User Experience (Dashboard)

The device provides a professional **Single Page Application (SPA)** dashboard:
*   **Real-time Metrics**: Monitor Free Heap, Uptime, and NVS Buffer usage.
*   **Dynamic Config**: Update WiFi credentials and reporting intervals without flashing.
*   **Access**: Connect to `IoT-Node-Config` Wi-Fi. The Captive Portal will redirect you to `http://192.168.4.1`.

---

## 📝 Technical Specs
*   **NVS Buffer Capacity**: 1024 records.
*   **NVS Partition Size**: 128 KB.
*   **App Partition Size**: 1664 KB (Dual OTA).
*   **RAM Usage**: ~28% (with Wi-Fi and Web Server active).
