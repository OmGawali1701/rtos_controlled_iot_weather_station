#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <WiFi.h>
#include <HTTPClient.h>

// SPI OLED Display Configuration
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_MOSI   23  
#define OLED_CLK    18  
#define OLED_DC     16  
#define OLED_CS      5  
#define OLED_RESET  17  

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, OLED_MOSI, OLED_CLK, OLED_DC, OLED_RESET, OLED_CS);

// Wi-Fi Credentials
const char* ssid = "infonet B-3";
const char* password = "rahul@b3";

// ThingSpeak Configuration
const char* server = "http://api.thingspeak.com/update";
const char* apiKey = "7RCOKAQJTAZI3L99";

// UART Configuration
#define RX_PIN 3    
#define TX_PIN 1    
#define BAUD_RATE 115200

// Sensor Data Structure (Updated to match STM32)
struct SensorData {
    float Temperature;
    float Pressure;
    float Humidity;
    float Co2;
    float NH3;
    float Ethanol;
    float AQI;
    uint8_t CRC;
};

// Function Prototypes
bool readSensorData(SensorData &data);
bool verifyCRC(const String &data, uint8_t receivedCRC);
void updateDisplay(const SensorData &data);
void sendToThingSpeak(const SensorData &data);
void checkAlerts(const SensorData &data);
void displayPage(int page, const SensorData &data);
void handleWiFiReconnection();

// Page Switching Variables
int currentPage = 0;
unsigned long lastPageSwitchTime = 0;
bool uartTriggerPageChange = false;

void setup() 
{
    Serial.begin(115200);
    Serial2.begin(BAUD_RATE, SERIAL_8N1, RX_PIN, TX_PIN);

    // Initialize SPI
    SPI.begin(OLED_CLK, -1, OLED_MOSI, OLED_CS);

    // Initialize OLED Display
    if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_CS)) 
    {  
        Serial.println("SSD1306 SPI initialization failed!");
        while (1);
    }

    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(10, 20);
    display.println("Initializing...");
    display.display();
    delay(1000);
    display.clearDisplay();

    // Connect to Wi-Fi
    WiFi.begin(ssid, password);
    Serial.print("Connecting to Wi-Fi");
    while (WiFi.status() != WL_CONNECTED) 
    {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nWi-Fi connected!");
}

void loop() 
{
    static unsigned long lastThingSpeakUpdate = 0;
    SensorData sensorData;

    handleWiFiReconnection();

    if (readSensorData(sensorData)) 
    {
        Serial.println("Updating OLED Display...");
        updateDisplay(sensorData);
        checkAlerts(sensorData);  // ✅ Restored Check Alerts Function
    }
    else 
    {
        Serial.println("Failed to read sensor data.");
    }

    // Send Data to ThingSpeak every 2 seconds
    if (millis() - lastThingSpeakUpdate >= 2000) 
    {
        sendToThingSpeak(sensorData);
        lastThingSpeakUpdate = millis();
    }

    // Handle Page Switching (Automatic or UART Triggered)
    if (uartTriggerPageChange || millis() - lastPageSwitchTime >= 5000) 
    {
        currentPage = (currentPage + 1) % 3;  
        lastPageSwitchTime = millis();
        uartTriggerPageChange = false;
        displayPage(currentPage, sensorData);
    }

    delay(500);
}

// Function to read sensor data from UART
bool readSensorData(SensorData &data) 
{
    if (Serial2.available()) 
    {
        String receivedData = Serial2.readStringUntil('\n');
        uint8_t receivedCRC;
        
        int parsed = sscanf(receivedData.c_str(), "@%f,%f,%f,%f,%f,%f,%f,%hhu,@",
                            &data.Temperature, &data.Pressure, &data.Humidity,
                            &data.Co2, &data.NH3, &data.Ethanol, &data.AQI, &receivedCRC);
        
        if (parsed == 8 && verifyCRC(receivedData, receivedCRC)) 
        {
            Serial2.println("ACK");  
            return true;
        }
        if (receivedData.indexOf("PAGE") != -1) 
        {
            uartTriggerPageChange = true;  
        }
    }
    return false;
}

// CRC Verification Function
bool verifyCRC(const String &data, uint8_t receivedCRC) 
{
    uint8_t computedCRC = 0;
    for (int i = 0; i < data.length() - 4; i++) 
    {
        computedCRC ^= data[i];
    }
    return (computedCRC == receivedCRC);
}

// Function to update OLED display
void updateDisplay(const SensorData &data) 
{
    displayPage(currentPage, data);
}

// Function to display pages
void displayPage(int page, const SensorData &data) 
{
    display.clearDisplay();
    display.setTextSize(1);
    display.setCursor(0, 0);
    
    if (page == 0) 
    {
        display.print("Temp: "); display.print(data.Temperature); display.println(" C");
        display.print("Humidity: "); display.print(data.Humidity); display.println(" %");
        display.print("Pressure: "); display.print(data.Pressure); display.println(" hPa");
    } 
    else if (page == 1) 
    {
        display.print("CO2: "); display.print(data.Co2); display.println(" ppm");
        display.print("NH3: "); display.print(data.NH3); display.println(" ppm");
        display.print("Ethanol: "); display.print(data.Ethanol); display.println(" ppm");
    } 
    else if (page == 2) 
    {
        display.print("AQI: "); display.print(data.AQI);
    }

    display.display();
    Serial.print("Page Changed to: ");
    Serial.println(page);
}

// Function to send data to ThingSpeak
void sendToThingSpeak(const SensorData &data) 
{
    if (WiFi.status() == WL_CONNECTED) 
    {
        HTTPClient http;
        String url = String(server) + "?api_key=" + apiKey +
                     "&field1=" + String(data.Temperature) +
                     "&field2=" + String(data.Humidity) +
                     "&field3=" + String(data.Pressure) +
                     "&field4=" + String(data.Co2) +
                     "&field5=" + String(data.NH3) +
                     "&field6=" + String(data.Ethanol) +                     
                     "&field7=" + String(data.AQI);

        http.begin(url);
        int httpResponseCode = http.GET();
        http.end();
    }
}

// ✅ Function to check for alerts
void checkAlerts(const SensorData &data) 
{
    bool alert = false;
    
    display.setTextSize(2);
    display.setTextColor(WHITE, BLACK);
    display.setCursor(0, 50);
    
    if (data.Temperature > 40) 
    {
        Serial2.println("ALERT: HIGH TEMP!");
        display.println("ALERT! TEMP HIGH");
        alert = true;
    }
    if (data.Humidity > 80) 
    {
        Serial2.println("ALERT: HIGH HUMIDITY!");
        display.println("ALERT! HUMID HIGH");
        alert = true;
    }
    if (data.Co2 > 1000) 
    {
        Serial2.println("ALERT: HIGH CO2!");
        display.println("ALERT! CO2 HIGH");
        alert = true;
    }
    if (data.AQI > 200) 
    {
        Serial2.println("ALERT: BAD AIR QUALITY!");
        display.println("ALERT! AQI BAD");
        alert = true;
    }

    if (alert) 
    {
        display.display();
        Serial.println("ALERT DISPLAYED!");
    }
}

// WiFi Reconnection Handler
void handleWiFiReconnection() 
{
    if (WiFi.status() != WL_CONNECTED) 
    {
        Serial.println("Reconnecting to Wi-Fi...");
        WiFi.disconnect();
        WiFi.reconnect();
    }
}