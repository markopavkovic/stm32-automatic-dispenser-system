# stm32-automatic-dispenser-system
Precision STM32-based automatic fluid dispenser featuring ultrasonic proximity detection, OLED UI, keypad volume control, MOSFET pump actuation, and MQTT cloud telemetry.
Evo osveženog Markdown koda za README.md sa ubacirom tabelom za video demonstraciju na istom mestu i u istom stilu kao na prošlom projektu.

Markdown
# STM32 Smart Automatic Fluid Dispenser with IoT Telemetry 🧴📊

An energy-efficient, touchless automatic fluid dispensing system built on the **STM32F103C8T6 (ARM Cortex-M3)** platform. The system enables users to enter exact target fluid volumes via a 4x4 matrix keypad, displays real-time status on an OLED screen, ensures contactless container verification using ultrasonic sensing, and streams real-time operation telemetry to the cloud using MQTT.

---

## 🎯 Project Overview & Objective

Manual fluid pouring often leads to inaccuracies, spills, and unnecessary physical contact with containers. This project realizes an embedded, touchless liquid dispensing solution designed for hygiene, precision, and automation. 

By integrating container distance verification and dynamic acoustic feedback, the system prevents dry activation and ensures accurate volumetric delivery driven via a quiet MOSFET power stage.

---

## 📹 Video Demonstrations

| Operational Feature | Description | Video Link |
| :--- | :--- | :--- |
| **Fluid Dispensing** | Automated MOSFET pump actuation & precise volume delivery | [▶️ Watch Pouring Demo](https://youtube.com/shorts/SWPjnbBznVo) |
| **OLED Interface** | Real-time status messages, volume entry, and system state | [▶️ Watch Display Demo](https://youtube.com/shorts/pc-9uzefjEM) |
| **Keypad + Display** | User target volume input (mL) with backspace/confirm logic | [▶️ Watch Keypad Demo](https://youtube.com/shorts/lIATlxD6Ejc) |
| **Ultrasonic Sensor** | Contactless container detection (< 12 cm) & proximity beeper | [▶️ Watch Sensor Demo](https://youtube.com/shorts/zN9fJVXfnVQ) |

---

## 🚀 Key Features & Operating Modes

* **Custom Volume Input:** Precise volume target entry (1–9999 mL) using a 4x4 membrane keypad with real-time editing (`*` for backspace, `#` for confirmation).
* **Ultrasonic Container Detection:** Touchless distance measurement via HC-SR04 to ensure liquid is dispensed only when a container is within optimal range (< 12 cm).
* **Dynamic Acoustic Proximity Alert:** Piezo buzzer functions as a "parking sensor," dynamically shortening beep intervals as a cup approaches the nozzle.
* **Solid-State Pump Actuation:** High-speed MOSFET module switching for a 3–5V DC submersible pump, eliminating mechanical relay noise and contacts arcing.
* **Non-Blocking Logic:** Fluid measurement and display polling managed asynchronously using timing loops (`millis()`) to ensure uninterrupted UI updates.
* **IoT Cloud Telemetry (MQTT):** Real-time bridge sending operational states and poured volume statistics to HiveMQ Cloud over secure TLS (Port 8883).

---

## 🛠️ Hardware Components & Technical Specifications

| Component | Function / Specification | STM32 Interface / Protocol |
| :--- | :--- | :--- |
| **STM32 MCU** | STM32F103C8T6 (32-bit ARM Cortex-M3 @ 72 MHz) | Core Microcontroller |
| **OLED Display** | 0.91" Monochrome SSD1306 (128x32 px) | I²C Bus (`PB6` SCL, `PB7` SDA) |
| **Ultrasonic Sensor** | HC-SR04 Distance Sensor (Range up to 12 cm) | GPIO / Timer Pulse (`PA0` TRIG, `PA1` ECHO) |
| **Actuator Driver** | MOSFET Switching Module | GPIO Output (`PB5`) |
| **Liquid Pump** | Mini Submersible DC Pump (3–5V DC, 80–100 L/h) | Driven via MOSFET Output Stage |
| **User Input** | 4x4 Matrix Membrane Keypad | GPIO Scan Rows (`PB12–PB15`), Cols (`PB8,PB9,PB3,PB4`) |
| **Acoustic Feedback** | Piezo Buzzer (Keypress confirmation & proximity alert) | GPIO Output (`PB0`) |
| **IoT Telemetry Interface**| CH340 USB-to-UART Serial Transceiver | USART1 (`PA9` TX, `PA10` RX @ 9600 Baud) |

---

## 📍 Pin Mapping & Signal Topology

To eliminate SWD hardware debugging conflicts with the **ST-Link/V2 programmer on Port A**, all keypad scan lines and audio output pins were re-routed to **Port B**:

```text
+-------------------------------------------------------------------------+
|                         STM32F103C8T6 (Blue Pill)                       |
+-------------------+-------------------+-------------------+-------------+
| I²C OLED (SSD1306)| Ultrasonic Sensor | Actuator & Audio  | Keypad 4x4  |
|  - SCL: PB6       |  - TRIG: PA0      |  - MOSFET: PB5    |  - Rows:    |
|  - SDA: PB7       |  - ECHO: PA1      |  - Buzzer: PB0    |    PB12-PB15|
|                   |                   |                   |  - Columns: |
|                   |                   |                   |    PB8,9,3,4|
+-------------------+-------------------+-------------------+-------------+
                                   |
                                   v (USART1 Serial @ 9600 Baud)
                       [ CH340 USB-to-UART Module ]
                                   |
                                   v (Python Gateway)
                     [ HiveMQ MQTT Cloud Broker ]
