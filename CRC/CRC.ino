#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <WiFi.h>
#include <HTTPClient.h>

// OLED Display Configuration
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_MOSI   23
#define OLED_CLK    18
#define OLED_DC     16
#define OLED_CS     5
#define OLED_RESET  17

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &SPI, OLED_DC, OLED_RESET, OLED_CS);

// Wi-Fi Credentials
const char* ssid = "infonet B-3";
const char* password = "rahul@b3";

// ThingSpeak Configuration
const char* server = "http://api.thingspeak.com/update";
const char* apiKey = "J1SBHRHOQADAMPPR";

// Button for manual page switching
#define BUTTON_PIN 4  // Change to actual GPIO pin number

// Page management
int currentPage = 1;
unsigned long lastPageSwitch = 0;

// Function to generate random sensor values
float getTemperature() { return random(200, 350) / 10.0; } // 20.0 to 35.0 °C
float getHumidity() { return random(300, 700) / 10.0; }     // 30.0 to 70.0 %
float getPressure() { return random(950, 1050); }          // 950 to 1050 hPa
float getCO2() { return random(300, 500); }                // 300 to 500 ppm
float getAQI() { return random(50, 200); }                 // AQI between 50-200

// CRC-8 Calculation
uint8_t calculateCRC8(uint8_t *data, size_t length) {
    uint8_t crc = 0x00;
    for (size_t i = 0; i < length; i++) {
        crc ^= data[i];
        for (uint8_t j = 0; j < 8; j++) {
            if (crc & 0x80)
                crc = (crc << 1) ^ 0x31;
            else
                crc <<= 1;
        }
    }
    return crc;
}

// Checksum Calculation
uint8_t calculateChecksum(float temperature, float humidity, float pressure, float co2, float aqi) {
    return ((int)temperature + (int)humidity + (int)pressure + (int)co2 + (int)aqi) % 256;
}

void setup() {
    Serial.begin(115200);

    // Initialize OLED Display
    if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
        Serial.println("SSD1306 initialization failed!");
        while (1);
    }

    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(10, 10);
    display.println("OLED Initialized!");
    display.display();  

    // Connect to Wi-Fi
    WiFi.begin(ssid, password);
    Serial.print("Connecting to Wi-Fi");
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nWi-Fi connected!");

    // Initialize button
    pinMode(BUTTON_PIN, INPUT_PULLUP);
}

void loop() {
    float temperature = getTemperature();
    float humidity = getHumidity();
    float pressure = getPressure();
    float co2 = getCO2();
    float aqi = getAQI();

    // Compute CRC and checksum
    uint8_t sensorData[5] = {(uint8_t)temperature, (uint8_t)humidity, (uint8_t)pressure, (uint8_t)co2, (uint8_t)aqi};
    uint8_t crc8 = calculateCRC8(sensorData, 5);
    uint8_t checksum = calculateChecksum(temperature, humidity, pressure, co2, aqi);

    // Cycle pages every 5 seconds
    if (millis() - lastPageSwitch > 5000) {
        currentPage++;
        if (currentPage > 4) currentPage = 1;
        lastPageSwitch = millis();
    }

    // Check button press to manually switch pages
    if (digitalRead(BUTTON_PIN) == LOW) {
        currentPage++;
        if (currentPage > 4) currentPage = 1;
        delay(300);
    }

    updateDisplay(temperature, humidity, pressure, co2, aqi, crc8, checksum);

    // Send Data to ThingSpeak
    if (WiFi.status() == WL_CONNECTED) {
        HTTPClient http;
        String url = String(server) + "?api_key=" + apiKey + 
                     "&field1=" + String(temperature) + 
                     "&field2=" + String(humidity) + 
                     "&field3=" + String(pressure) + 
                     "&field4=" + String(co2) +
                     "&field5=" + String(aqi) +
                     "&field6=" + String(crc8) +
                     "&field7=" + String(checksum);

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

    delay(5000);
}

// Function to update OLED display
void updateDisplay(float temperature, float humidity, float pressure, float co2, float aqi, uint8_t crc, uint8_t checksum) {
    display.clearDisplay();

    switch (currentPage) {
        case 1:  // Summary Page
            display.setTextSize(1);
            display.setCursor(0, 5);
            display.println("Weather Data:");
            display.println("----------------");
            display.print("Temp: "); display.print(temperature); display.println(" C");
            display.print("Humidity: "); display.print(humidity); display.println(" %");
            display.display();
            break;

        case 2:  // Air Quality Page
            display.setTextSize(1);
            display.setCursor(0, 5);
            display.println("Air Quality:");
            display.println("----------------");
            display.print("CO2: "); display.print(co2); display.println(" ppm");
            display.print("AQI: "); display.print(aqi);
            display.display();
            break;

        case 3:  // Alerts Page
            display.setTextSize(1);
            display.setCursor(0, 5);
            display.println("ALERTS:");
            display.println("----------------");
            if (temperature > 30) {
                display.println("High Temp Alert!");
            }
            if (aqi > 100) {
                display.println("Poor AQI Alert!");
            }
            if (temperature <= 30 && aqi <= 100) {
                display.println("No Alerts");
            }
            display.display();
            break;

        case 4:  // CRC & Checksum Page
            display.setTextSize(1);
            display.setCursor(0, 5);
            display.println("Data Integrity:");
            display.println("----------------");
            display.print("CRC-8: "); display.println(crc, HEX);
            display.print("Checksum: "); display.println(checksum);
            display.display();
            break;
    }
}
