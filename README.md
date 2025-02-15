# RTOS-Controlled IoT Weather Station

## 📌 Project Overview
This is my **CDAC PG-DESD final project**, where I applied most of the concepts learned in the curriculum. The **RTOS-Controlled IoT Weather Station** monitors real-time environmental parameters such as **temperature, humidity, pressure, and air quality index (AQI)**.

The system uses **STM32** for sensor interfacing and data processing, and **ESP32** for cloud communication and display updates. **FreeRTOS** is used to handle real-time task scheduling efficiently.

## 🚀 Key Features
✔ **FreeRTOS-Based Architecture**: Manually integrated non-CMSIS FreeRTOS on STM32.
✔ **Service APIs**: Modular implementation for UART, ADC, and shared data handling.
✔ **Real-Time Sensor Data Collection**: BME280 (I2C) & MQ135 (ADC) sensors.
✔ **UART Communication with CRC-8 Validation**: Ensures reliable data exchange between STM32 & ESP32.
✔ **Cloud Data Logging**: ESP32 uploads data to **ThingSpeak via HTTP**.
✔ **OLED Display (SSD1306, SPI)**: Real-time visualization of sensor values.
✔ **MAC Filtering for Wi-Fi**: Option to restrict ESP32 to specific networks.
✔ **Real-Time Alerts**: Threshold-based alerts displayed dynamically.
✔ **Debugging & Error Handling**: SEGGER SystemView, J-Link Ozone Debugger, CRC-8 validation.

---

## 📡 System Architecture
### **1️⃣ Hardware Components**
- **STM32 (Sensor Node)**
  - BME280 (Temperature, Humidity, Pressure) via **I2C**
  - MQ135 (Air Quality - CO2, NH3, Ethanol) via **ADC**
  - UART Communication with ESP32
- **ESP32 (Cloud Gateway + Display Unit)**
  - OLED Display (SSD1306) via **SPI**
  - Wi-Fi Connectivity for **ThingSpeak Cloud**
  - MAC Filtering Feature for network security

### **2️⃣ Software Implementation**
#### **FreeRTOS Task Management on STM32**
- **Sensor Task**: Reads BME280 & MQ135 sensor data.
- **Processing Task**: Filters and processes data.
- **Shared Data Protection**: **Mutex and Semaphore** mechanisms.
- **UART Task**: Data exchange with ESP32 (**ACK-based retry mechanism**).
- **OLED Update Task**: Real-time display updates.

#### **Cloud Integration & Data Logging (ESP32)**
- **Receives data via UART, verifies it using CRC-8, and uploads to ThingSpeak.**
- **Automatic Wi-Fi Reconnection** ensures uninterrupted cloud updates.
- **HTTP Requests** for cloud communication.

#### **Error Handling & Debugging**
- **SEGGER SystemView**: Task execution tracking & debugging.
- **Ozone Debugger (J-Link)**: Flashing & debugging STM32 firmware.
- **CRC-8 Checksum**: Ensures error-free UART data transmission.

---

## 🛠️ Installation & Setup
### **Prerequisites**
1. **Software Tools Required:**
   - STM32CubeIDE (STM32 firmware development)
   - Arduino IDE / PlatformIO (ESP32 development)
   - SEGGER SystemView (RTOS Debugging)
   - Ozone Debugger (Flashing STM32 via J-Link)

2. **Hardware Setup:**
   - Connect **BME280** to STM32 via **I2C**
   - Connect **MQ135** to STM32 via **ADC**
   - Connect **STM32 UART (TX, RX)** to ESP32
   - Connect **SSD1306 OLED** to ESP32 via **SPI**

### **Flashing the Firmware**
#### **For STM32:**
- Open STM32CubeIDE → Compile & Flash using **J-Link Ozone**.
- Ensure `FreeRTOSConfig.h` is correctly set.

#### **For ESP32:**
- Use **Arduino IDE / PlatformIO**.
- Select the correct **COM port & Board (ESP32 Dev Module)**.
- Compile and upload firmware.

---

## 🎯 Usage Instructions
1. **Power on the system** → STM32 starts collecting sensor data.
2. **FreeRTOS tasks manage sensor data, processing, and UART transmission.**
3. **ESP32 receives data, updates OLED, and sends it to the cloud.**
4. **Monitor sensor values & alerts on OLED display and ThingSpeak dashboard.**
5. **If an alert is triggered (e.g., CO₂ > 500 ppm), the OLED shows the alert screen.**

---

## 📊 Real-Time Data Visualization
📌 **ThingSpeak Dashboard:** Sensor values are logged and displayed online.  
📌 **OLED Display:** Updates dynamically with real-time data.  
📌 **SystemView Logs:** Provides real-time execution insights for debugging.  

---

## 🔥 Contributors
👨‍💻 **Om Anil Gawali** (Embedded RTOS & Task Management)  
👨‍💻 **Kuldeep Patel**  
👨‍💻 **Om Chavan**  
👨‍💻 **Prathamesh Ghongade**  
👨‍💻 **Tulsiram Sagar**  

### **Special Thanks To:**
👨‍🏫 **Arafat Khan** (Project Guide, CDAC)  
👩‍🏫 **Gayle Fiona Fernandes** (RTOS Implementation Support)  

---

## 📝 License
This project is **open-source** and available under the **MIT License**. Feel free to modify and contribute! 🚀

