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

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, OLED_MOSI, OLED_CLK, OLED_DC, OLED_RESET, OLED_CS);

// Wi-Fi Credentials
const char* ssid = "infonet B-3";
const char* password = "rahul@b3";

// ThingSpeak Configuration
const char* server = "http://api.thingspeak.com/update";
const char* apiKey = "J1SBHRHOQADAMPPR";

// Function to generate random sensor values
float getTemperature() { return random(200, 350) / 10.0; } // 20.0 to 35.0 °C
float getHumidity() { return random(300, 700) / 10.0; }     // 30.0 to 70.0 %
float getPressure() { return random(950, 1050); }          // 950 to 1050 hPa

void setup() {
    Serial.begin(115200);
    
    // Initialize OLED Display
    if (!display.begin(SSD1306_SWITCHCAPVCC)) {
        Serial.println("SSD1306 initialization failed!");
        while (1);
    }
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(WHITE);

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
    float temperature = getTemperature();
    float humidity = getHumidity();
    float pressure = getPressure();

    // Display Data on OLED
    display.clearDisplay();
    display.setCursor(0, 0);
    display.print("Temp: ");
    display.print(temperature);
    display.println(" C");

    display.print("Humidity: ");
    display.print(humidity);
    display.println(" %");

    display.print("Pressure: ");
    display.print(pressure);
    display.println(" hPa");

    display.display();
    
    // Send Data to ThingSpeak
    if (WiFi.status() == WL_CONNECTED) {
        HTTPClient http;
        String url = String(server) + "?api_key=" + apiKey + 
                     "&field1=" + String(temperature) + 
                     "&field2=" + String(humidity) + 
                     "&field3=" + String(pressure);
                     
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

    delay(5000); // Wait 15 seconds (ThingSpeak update limit)
}
