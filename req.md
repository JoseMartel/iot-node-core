# Documento de Requisitos y Arquitectura: IoT Node Core

## 1. Visión General del Sistema
El "IoT Node Core" es un dispositivo embebido genérico y modular basado en el microcontrolador **ESP32-C6 Mini** utilizando el framework **ESP-IDF**. Su propósito principal es adquirir datos de múltiples tipos de sensores y fuentes, procesarlos si es necesario, y transmitirlos de manera robusta y flexible hacia la nube.

## 2. Requisitos del Sistema
### 2.1 Requisitos Funcionales
- **Adquisición Multifuente:** Capacidad de leer datos de diversos sensores (analógicos, I2C, SPI, UART, GPIO).
- **Flujo de Datos Binario:** Uso de estructuras (`struct`) de C para el transporte interno de datos, minimizando el uso de memoria y stack.
- **Procesamiento de Datos:** Capa intermedia opcional para filtrar, promediar o transformar datos antes del envío.
- **Conectividad Flexible:** Soporte para transmisión de datos vía Wi-Fi (nativo del ESP32-C6) o Módems Celulares (2G/3G/4G/NB-IoT vía UART).
- **Protocolos de Nube:** Envío de telemetría a través de protocolos estándar como MQTT y HTTP/HTTPS.
- **Configuración Local:** Modo Access Point (AP) con un Web Server integrado (Captive Portal) para que el usuario pueda configurar parámetros del dispositivo mediante una interfaz web sencilla.
- **Metadatos de Sistema:** Cada paquete de datos debe incluir variables del estado del sistema (UID del mensaje, timestamp, nivel de batería, estado de conexión, memoria libre).
- **Parámetros Base de Operación:**
  - **Intervalo de Envío (Transmission Interval):** Tiempo configurable entre transmisiones de datos a la nube (en segundos).
  - **Intervalo de Lectura:** Frecuencia con la que se consultan los sensores.

### 2.2 Requisitos No Funcionales (Robustez y Confiabilidad)
- **Tolerancia a Fallos de Red (NVS Circular Buffer):** Si la conexión se pierde, los datos se almacenan en un buffer circular binario basado en NVS (Non-Volatile Storage), evitando el overhead de sistemas de archivos como SPIFFS.
- **Eficiencia de Energía:** Uso de modos de bajo consumo (Deep Sleep/Light Sleep) entre intervalos de transmisión.
- **Modularidad:** El código está desacoplado mediante componentes de ESP-IDF, permitiendo intercambiar drivers o protocolos sin afectar el core.

## 3. Estructura de Archivos (Modularidad ESP-IDF)
```text
iot-node-core/
├── components/
│   ├── config_manager/      # Gestión de NVS y parámetros (WiFi, Tiempos).
│   ├── sensor_manager/      # HAL de sensores y lógica de adquisición (Binaria).
│   ├── storage_manager/     # Buffer Circular Binario sobre NVS Blobs.
│   ├── network_manager/     # Abstracción de conectividad (WiFi/Celular).
│   ├── protocol_manager/    # Clientes Cloud (Conversión JIT a JSON/CBOR).
│   ├── web_server/          # Portal cautivo y configuración local.
│   └── system_utils/        # Tipos globales (iot_packet_t), logs y salud.
├── main/
│   ├── main.c               # Orquestación del flujo Sensor -> Storage -> Cloud.
│   ├── CMakeLists.txt
├── partitions.csv           # Tabla de particiones optimizada para NVS y OTA.
├── CMakeLists.txt           # Configuración de compilación del proyecto.
└── sdkconfig                # Configuración del hardware y RTOS.
```

## 4. Arquitectura de Módulos (Detalle)

### 4.1 `system_utils` (Core Types)
Define la estructura central `iot_packet_t`. Todos los módulos intercambian este tipo de dato binario. Incluye herramientas de monitoreo de heap y stack para garantizar la estabilidad.

### 4.2 `storage_manager` (Lean Storage)
Implementa un buffer circular sobre NVS.
- **Estructura:** Utiliza un índice `head` y `tail` persistentes.
- **Almacenamiento:** Los paquetes se guardan como Blobs con llaves dinámicas (ej. `d0`, `d1`).
- **Ventaja:** Bajo consumo de stack comparado con SPIFFS y protección nativa contra cortes de energía.

### 4.3 `protocol_manager` (JIT Conversion)
Responsable de la comunicación con la nube.
- **JIT JSON:** Convierte el `struct` binario a JSON (o CBOR) justo antes del envío HTTP/MQTT para ahorrar RAM en reposo.
- **Endpoints:** Maneja rutas separadas para `/data` (telemetría) y `/config` (parámetros).

### 4.4 `network_manager` (Connectivity FSM)
Abstrae la interfaz física. Expone una API genérica para que el sistema consulte el estado de conexión sin importar si es Wi-Fi, Ethernet o Módem Celular.

## 5. Flujo de Datos (Pipeline de Doble Capa)
El sistema utiliza una estrategia de buffer híbrido para balancear rendimiento y persistencia:
1. **Acquire:** El `sensor_manager` genera un `iot_packet_t` y lo deposita en una **Cola de RAM (FreeRTOS Queue)**.
2. **Consumo y Enrutamiento (Sender Task):**
   - **Modo Online (Prioridad NVS):** 
     - El sistema verifica primero si hay datos pendientes en **NVS** (históricos). Si los hay, los extrae y envía uno a uno.
     - Una vez vacía la NVS, procesa los datos en tiempo real de la **Cola de RAM**.
   - **Modo Offline (Persistencia Crítica):**
     - Los datos que llegan a la **Cola de RAM** son extraídos y guardados inmediatamente en **NVS** como Blobs binarios. Esto garantiza que la telemetría sobreviva a un reinicio accidental durante la desconexión.
3. **Optimización de Flash:** Solo se escribe en NVS cuando es estrictamente necesario (desconexión o vaciado de emergencia), extendiendo la vida útil de la memoria.

## 6. Plan de Ejecución (Estado Actual)
- [x] **Fase 1: Infraestructura Base**: `config_manager`, `system_utils`, `system_types`.
- [x] **Fase 2: Conectividad y Protocolo**: `network_manager` (WiFi), `protocol_manager` (HTTP JIT).
- [x] **Fase 3: Almacenamiento Robusto**: `storage_manager` (NVS Circular Buffer).
- [x] **Fase 4: Adquisición**: `sensor_manager` (Interface binaria).
- [ ] **Fase 5: Configuración Local**: `web_server` (Captive Portal), integración Frontend-NVS.
- [ ] **Fase 6: Integración y Sensores Reales**: Implementación de drivers I2C/ADC y pruebas de estrés.
