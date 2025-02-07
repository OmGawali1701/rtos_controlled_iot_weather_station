"#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include "esp_wifi.h"

// OLED Display Configuration
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_MOSI   23
#define OLED_CLK    18
#define OLED_DC     16
#define OLED_CS     5
#define OLED_RESET  17

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, OLED_MOSI, OLED_CLK, OLED_DC, OLED_RESET, OLED_CS);

// Wi-Fi Credentials (MAC-Restricted)
const char* ssid = "acts";
const uint8_t allowedMAC[] = {0xF4, 0x96, 0x34, 0x9D, 0xE5, 0xA5};

// ThingSpeak Configuration
const char* server = "http://api.thingspeak.com/update";
const char* apiKey = "J1SBHRHOQADAMPPR";

// UART Configuration
#define UART_NUM 0  // Using UART0 for STM32 Communication
#define RX_PIN 3    // GPIO3 (RX)
#define TX_PIN 1    // GPIO1 (TX)
#define BAUD_RATE 115200

// Sensor Data Structure
typedef struct {
    float temperature;
    float humidity;
    float pressure;
    float co2;
    float aqi;
} SensorData_t;

SensorData_t sensorData;

// Function Prototypes
void connectToWiFi();
void updateDisplay();
void processUartData(String data);
void sendDataToThingSpeak();

unsigned long lastThingSpeakUpdate = 0;

void setup() {
    Serial.begin(BAUD_RATE);
    
    // Initialize UART0
    Serial1.begin(BAUD_RATE, SERIAL_8N1, RX_PIN, TX_PIN);

    // Set ESP32 MAC address to allowed one
    esp_wifi_set_mac(WIFI_IF_STA, (uint8_t*)allowedMAC);

    // Connect to Wi-Fi
    connectToWiFi();

    // Initialize OLED Display
    if (!display.begin(SSD1306_SWITCHCAPVCC)) {
        Serial.println("SSD1306 initialization failed!");
        while (1);
    }
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(WHITE);
}

void loop() {
    // Read UART data from STM32
    if (Serial1.available()) {
        String receivedData = Serial1.readStringUntil('\n'); // Read until newline
        processUartData(receivedData);
        updateDisplay(); // Update OLED when new data is received
    }

    // Send data to ThingSpeak every 15 seconds
    if (millis() - lastThingSpeakUpdate >= 15000) {
        sendDataToThingSpeak();
        lastThingSpeakUpdate = millis();
    }

    delay(1000); // Update loop every second
}

// Function to connect ESP32 to WiFi
void connectToWiFi() {
    WiFi.begin(ssid); // Open network (No password)
    Serial.print("Connecting to Wi-Fi");
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nWi-Fi connected!");
}

// Function to parse UART data and update sensorData structure
void processUartData(String data) {
    Serial.println("Received Data: " + data);

    // Expected format: "@Temp,Hum,Press,CO2,AQI@"
    data.replace("@", ""); // Remove '@' symbols

    float values[5]; // Array to hold parsed values
    int index = 0;
    char* token = strtok((char*)data.c_str(), ",");

    while (token != NULL && index < 5) {
        values[index++] = atof(token);
        token = strtok(NULL, ",");
    }

    // Assign values to sensor data structure
    sensorData.temperature = values[0];
    sensorData.humidity = values[1];
    sensorData.pressure = values[2];
    sensorData.co2 = values[3];
    sensorData.aqi = values[4];

    Serial.printf("Parsed: T=%.2f, H=%.2f, P=%.2f, CO2=%.2f, AQI=%.2f\n",
                   sensorData.temperature, sensorData.humidity, 
                   sensorData.pressure, sensorData.co2, sensorData.aqi);
}

// Function to update OLED display with sensor data
void updateDisplay() {
    display.clearDisplay();
    display.setTextSize(2);
    display.setCursor(10, 5);
    display.println("Weather Data");
    display.drawLine(0, 22, 128, 22, WHITE);
    
    display.setCursor(10, 30);
    display.print("T: "); display.print(sensorData.temperature); display.println("C");
    display.setCursor(10, 50);
    display.print("H: "); display.print(sensorData.humidity); display.println("%");

    display.display();
}

// Function to send data to ThingSpeak
void sendDataToThingSpeak() {
    if (WiFi.status() == WL_CONNECTED) {
        HTTPClient http;
        String url = String(server) + "?api_key=" + apiKey + 
                     "&field1=" + String(sensorData.temperature) + 
                     "&field2=" + String(sensorData.humidity) + 
                     "&field3=" + String(sensorData.pressure) + 
                     "&field4=" + String(sensorData.co2) + 
                     "&field5=" + String(sensorData.aqi);

        http.begin(url);
        int httpResponseCode = http.GET();
        
        if (httpResponseCode > 0) {
            Serial.print("ThingSpeak Response: ");
            Serial.println(httpResponseCode);
        } else {
            Serial.print("Error sending data: ");
            Serial.println(httpResponseCode);
        }
        http.end();
    } else {
        Serial.println("Wi-Fi not connected! Data not sent.");
    }
}
"