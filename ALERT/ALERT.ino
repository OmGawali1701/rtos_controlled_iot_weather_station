#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <WiFi.h>
#include <HTTPClient.h>

// SPI OLED Display Configuration
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_MOSI   23  // SPI MOSI
#define OLED_CLK    18  // SPI Clock
#define OLED_DC     16  // Data/Command pin
#define OLED_CS      5  // Chip Select
#define OLED_RESET  17  // Reset

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, OLED_MOSI, OLED_CLK, OLED_DC, OLED_RESET, OLED_CS);

// Wi-Fi Credentials
const char* ssid = "infonet B-3";
const char* password = "rahul@b3";

// ThingSpeak Configuration
const char* server = "http://api.thingspeak.com/update";
const char* apiKey = "J1SBHRHOQADAMPPR";

// UART Configuration
#define RX_PIN 3    // GPIO3 (RX)
#define TX_PIN 1    // GPIO1 (TX)
#define BAUD_RATE 115200

// Sensor Data Structure
struct SensorData {
    float temperature;
    float humidity;
    float pressure;
    float co2;
    float aqi;
};

// Function Prototypes
bool readSensorData(SensorData &data);
void updateDisplay(const SensorData &data);
void sendToThingSpeak(const SensorData &data);

void setup() {
    Serial.begin(115200);
    Serial2.begin(BAUD_RATE, SERIAL_8N1, RX_PIN, TX_PIN);

    // Initialize SPI
    SPI.begin(OLED_CLK, -1, OLED_MOSI, OLED_CS);

    // Initialize OLED Display in SPI Mode
    if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_CS)) {  
        Serial.println("SSD1306 SPI initialization failed!");
        while (1);
    }

    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(10, 20);
    display.println("Initializing...");
    display.display();
    delay(2000);
    display.clearDisplay();

    // Connect to Wi-Fi
    WiFi.begin(ssid, password);
    Serial.print("Connecting to Wi-Fi");
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nWi-Fi connected!");
}

void loop() {
    static unsigned long lastThingSpeakUpdate = 0;
    SensorData sensorData;

    if (readSensorData(sensorData)) {
        Serial.println("Updating OLED Display...");
        updateDisplay(sensorData);
    } else {
        Serial.println("Failed to read sensor data.");
    }

    // Send Data to ThingSpeak every 5 seconds
    if (millis() - lastThingSpeakUpdate >= 5000) {
        sendToThingSpeak(sensorData);
        lastThingSpeakUpdate = millis();
    }
    delay(1000);
}

// Function to read sensor data from UART
bool readSensorData(SensorData &data) {
    if (Serial2.available()) {
        String receivedData = Serial2.readStringUntil('\n');
        
        int parsed = sscanf(receivedData.c_str(), "$@,%f,%f,%f,%f,%f,@",
                            &data.temperature, &data.humidity, &data.pressure,
                            &data.co2, &data.aqi);
        
        if (parsed == 5) {
            Serial.println("Sensor Data Received:");
            Serial.print("Temperature: "); Serial.println(data.temperature);
            Serial.print("Humidity: "); Serial.println(data.humidity);
            Serial.print("Pressure: "); Serial.println(data.pressure);
            Serial.print("CO2: "); Serial.println(data.co2);
            Serial.print("AQI: "); Serial.println(data.aqi);
            return true;
        }
    }
    return false;
}

// Function to update OLED display using SPI
void updateDisplay(const SensorData &data) {
    display.clearDisplay();
    display.setTextSize(1);
    display.setCursor(0, 0);
    display.print("Temp: "); display.print(data.temperature); display.println(" C");
    display.print("Humidity: "); display.print(data.humidity); display.println(" %");
    display.print("Pressure: "); display.print(data.pressure); display.println(" hPa");
    display.print("CO2: "); display.print(data.co2); display.println(" ppm");
    display.print("AQI: "); display.print(data.aqi);
    display.display();
    Serial.println("OLED Display Updated!");
}

// Function to send data to ThingSpeak
void sendToThingSpeak(const SensorData &data) {
    if (WiFi.status() == WL_CONNECTED) {
        HTTPClient http;
        String url = String(server) + "?api_key=" + apiKey +
                     "&field1=" + String(data.temperature) +
                     "&field2=" + String(data.humidity) +
                     "&field3=" + String(data.pressure) +
                     "&field4=" + String(data.co2) +
                     "&field5=" + String(data.aqi);

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
        Serial.println("Wi-Fi not connected!");
    }
}
