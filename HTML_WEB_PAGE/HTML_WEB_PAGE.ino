#include <WiFi.h>
#include <WebServer.h>

// WiFi Credentials
const char* ssid = "Your_SSID";
const char* password = "Your_PASSWORD";

// Web Server on port 80
WebServer server(80);

// Sample sensor data (Replace with real sensor readings)
float temperature = 25.5;
float humidity = 60.0;
float pressure = 1012.5;
float co2 = 450;
float aqi = 80;

// HTML Page
const char webpage[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>ESP32 RTOS Weather Station</title>
    <style>
        body { font-family: Arial, sans-serif; text-align: center; }
        .container { max-width: 600px; margin: auto; padding: 20px; }
        .data-box { padding: 15px; margin: 10px; border-radius: 10px; font-size: 20px; }
        .alert { background-color: red; color: white; }
        .normal { background-color: lightgreen; }
    </style>
</head>
<body>
    <h1>ESP32 Weather Data</h1>
    <div class="container">
        <div id="temperature" class="data-box">🌡️ Temperature: -- °C</div>
        <div id="humidity" class="data-box">💧 Humidity: -- %</div>
        <div id="pressure" class="data-box">📈 Pressure: -- hPa</div>
        <div id="co2" class="data-box">🌿 CO₂: -- ppm</div>
        <div id="nh3" class="data-box">☣️ NH₃: -- ppm</div>
        <div id="ethanol" class="data-box">🍷 Ethanol: -- ppm</div>
        <div id="aqi" class="data-box">🟢 AQI: --</div>
    </div>
    <script>
        async function fetchData() {
            let apiUrl = "https://api.thingspeak.com/channels/YOUR_CHANNEL_ID/feeds.json?results=1";
            let response = await fetch(apiUrl);
            let data = await response.json();
            let lastEntry = data.feeds[data.feeds.length - 1];
            
            document.getElementById("temperature").textContent = `🌡️ Temperature: ${lastEntry.field1} °C`;
            document.getElementById("humidity").textContent = `💧 Humidity: ${lastEntry.field2} %`;
            document.getElementById("pressure").textContent = `📈 Pressure: ${lastEntry.field3} hPa`;
            document.getElementById("co2").textContent = `🌿 CO₂: ${lastEntry.field4} ppm`;
            document.getElementById("nh3").textContent = `☣️ NH₃: ${lastEntry.field5} ppm`;
            document.getElementById("ethanol").textContent = `🍷 Ethanol: ${lastEntry.field6} ppm`;
            document.getElementById("aqi").textContent = `🟢 AQI: ${lastEntry.field7}`;
            
            if (parseFloat(lastEntry.field1) > 50) {
                document.getElementById("temperature").classList.add("alert");
            } else {
                document.getElementById("temperature").classList.add("normal");
            }
        }
        setInterval(fetchData, 5000);
        fetchData();
    </script>
</body>
</html>

)rawliteral";

// Handles the root URL ("/")
void handleRoot() {
    server.send(200, "text/html", webpage);
}

// Sends JSON sensor data
void handleData() {
    String json = "{";
    json += "\"temperature\":" + String(temperature) + ",";
    json += "\"humidity\":" + String(humidity) + ",";
    json += "\"pressure\":" + String(pressure) + ",";
    json += "\"co2\":" + String(co2) + ",";
    json += "\"aqi\":" + String(aqi);
    json += "}";
    server.send(200, "application/json", json);
}

void setup() {
    Serial.begin(115200);
    
    WiFi.begin(ssid, password);
    Serial.print("Connecting to WiFi...");
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nWiFi Connected!");
    Serial.print("ESP32 Server IP: ");
    Serial.println(WiFi.localIP());

    server.on("/", handleRoot);
    server.on("/data", handleData);
    
    server.begin();
    Serial.println("Server started!");
}

void loop() {
    server.handleClient();
}
