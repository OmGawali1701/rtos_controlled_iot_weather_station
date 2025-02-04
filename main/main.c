#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include "bme280.h"
#include "esp_http_client.h"
#include "sdkconfig.h"

// ThingSpeak settings
#define THINGSPEAK_CHANNEL_ID "2826741"
#define THINGSPEAK_API_KEY "HZGS5EPJDD8PE5VV"
#define THINGSPEAK_URL "http://api.thingspeak.com/update"

// BME280 settings
#define BME280_I2C_ADDR 0x76 // I2C address for the BME280

// Create a BME280 object
bme280_t bme280;

// Create an I2C driver instance
i2c_port_t i2c_num = I2C_NUM_0;

esp_http_client_handle_t client;

// Function to initialize the I2C and BME280 sensor
esp_err_t bme280_init() {
    i2c_config_t conf;
    conf.mode = I2C_MODE_MASTER;
    conf.sda_io_num = GPIO_NUM_21;  // SDA pin
    conf.scl_io_num = GPIO_NUM_22;  // SCL pin
    conf.sda_pullup_en = GPIO_PULLUP_ENABLE;
    conf.scl_pullup_en = GPIO_PULLUP_ENABLE;
    conf.master.clk_speed = 100000;  // 100kHz
    esp_err_t err = i2c_param_config(i2c_num, &conf);
    if (err != ESP_OK) return err;
    err = i2c_driver_install(i2c_num, I2C_MODE_MASTER, 0, 0, 0);
    if (err != ESP_OK) return err;
    return bme280_init_desc(&bme280, BME280_I2C_ADDR, i2c_num);
}

// Function to read data from BME280
esp_err_t read_bme280_data(float* temperature, float* humidity, float* pressure) {
    struct bme280_data bme_data;
    esp_err_t err = bme280_measure(&bme280, &bme_data);
    if (err != ESP_OK) return err;

    *temperature = bme_data.temperature;
    *humidity = bme_data.humidity;
    *pressure = bme_data.pressure;
    return ESP_OK;
}

// Function to send data to ThingSpeak
esp_err_t send_data_to_thingspeak(float temperature, float humidity, float pressure) {
    char post_data[256];
    snprintf(post_data, sizeof(post_data), "api_key=%s&field1=%.2f&field2=%.2f&field3=%.2f",
             THINGSPEAK_API_KEY, temperature, humidity, pressure);

    esp_http_client_config_t config = {
        .url = THINGSPEAK_URL,
    };
    client = esp_http_client_init(&config);

    esp_http_client_set_url(client, THINGSPEAK_URL);
    esp_http_client_set_post_field(client, post_data, strlen(post_data));

    esp_err_t err = esp_http_client_perform(client);
    if (err == ESP_OK) {
        ESP_LOGI("ThingSpeak", "Data sent successfully.");
    } else {
        ESP_LOGE("ThingSpeak", "Failed to send data: %s", esp_err_to_name(err));
    }

    esp_http_client_cleanup(client);
    return err;
}

void app_main() {
    ESP_LOGI("APP", "Starting BME280 and ThingSpeak");

    // Initialize BME280 sensor
    if (bme280_init() != ESP_OK) {
        ESP_LOGE("BME280", "Failed to initialize BME280");
        return;
    }

    // Variables for sensor data
    float temperature, humidity, pressure;

    // Main loop to read data and send to ThingSpeak every 10 seconds
    while (1) {
        // Read data from BME280
        if (read_bme280_data(&temperature, &humidity, &pressure) == ESP_OK) {
            ESP_LOGI("BME280", "Temp: %.2f C, Humidity: %.2f%%, Pressure: %.2f hPa",
                     temperature, humidity, pressure);
            
            // Send data to ThingSpeak
            send_data_to_thingspeak(temperature, humidity, pressure);
        } else {
            ESP_LOGE("BME280", "Failed to read data from BME280");
        }

        // Delay 10 seconds before the next reading
        vTaskDelay(10000 / portTICK_PERIOD_MS);
    }
}
