/*
 * service_api.c
 *
 *  Created on: Feb 8, 2025
 *      Author: ADMIN
 */
#include "service_api.h"
#include "FreeRTOS.h"
#include "semphr.h"
#include "SEGGER_SYSVIEW.h"
#include "struct.h"
#include "string.h"

#define DEBUG 1

extern UART_HandleTypeDef huart5;

extern SemaphoreHandle_t SensorDataMutex;
extern Sensordata_t SensorData;

// 🔹 **Service API for UART Transmission**
void UART_Transmit_Service( uint8_t *data, uint16_t len)
{
    if (HAL_UART_Transmit(&huart5, data, len, 50) != HAL_OK)

    {
        SEGGER_SYSVIEW_PrintfHost("UART Transmission Failed");
    }
	#ifdef DEBUG
        SEGGER_SYSVIEW_PrintfHost("UART Data Sent Successfully");
	#endif
}

// 🔹 **Service API for UART Reception**
uint8_t UART_Receive_Service(uint8_t *rxBuffer, uint16_t len)
{
    for (uint8_t attempt = 0; attempt < 3; attempt++)  // Retry up to 3 times
    {
        // Receive Data from ESP32
        if (HAL_UART_Receive(&huart5, rxBuffer, len, portMAX_DELAY) == HAL_OK)
        {
            SEGGER_SYSVIEW_PrintfHost("Data Received from ESP32");

            // Send "ACK" to ESP32
            const char *ackMsg = "ACK\n";
            HAL_UART_Transmit(&huart5, (uint8_t *)ackMsg, strlen(ackMsg), 50);

            // Wait for "ACK" Confirmation from ESP32 (Max 100ms timeout)
            uint8_t ackBuffer[4] = {0};
            if (HAL_UART_Receive(&huart5, ackBuffer, 3, 50) == HAL_OK)
            {
                ackBuffer[3] = '\0';  // Null-terminate for safety
                if (strcmp((char *)ackBuffer, "ACK") == 0)
                {
                    SEGGER_SYSVIEW_PrintfHost("ESP32 ACK Received: Sending Next Data");
                    return 1; // Success, move to next data
                }
            }

            SEGGER_SYSVIEW_PrintfHost("ESP32 ACK Not Received: Retrying...");
        }
        else
        {
            SEGGER_SYSVIEW_PrintfHost("UART Receive Failed: Retrying...");
        }
    }

    SEGGER_SYSVIEW_PrintfHost("Failed to Receive ACK After 3 Attempts");
    return 0; // Final Failure
}



// 🔹 **Service API for ADC Read**
float Read_ADC_Value(ADC_HandleTypeDef *hadc)
{
    volatile uint32_t adcSum = 0;
    const uint8_t numSamples = 10;

    for (uint8_t i = 0; i < numSamples; i++)
    {
        HAL_ADC_Start(hadc);
        if (HAL_ADC_PollForConversion(hadc, 10) == HAL_OK)
        {
            adcSum += HAL_ADC_GetValue(hadc);
        }
        HAL_ADC_Stop(hadc);
        HAL_Delay(1);
    }

    return (adcSum / numSamples);
}


// 🔹 **Service API for Writing Sensor Data**
void Write_SensorData(Sensordata_t *data)
{
    xSemaphoreTake(SensorDataMutex, portMAX_DELAY);
    SensorData = *data;
    xSemaphoreGive(SensorDataMutex);
}

// 🔹 **Service API for Reading Sensor Data**
void Read_SensorData(Sensordata_t *data)
{
    xSemaphoreTake(SensorDataMutex, portMAX_DELAY);
    *data = SensorData;
    xSemaphoreGive(SensorDataMutex);
}
/*
// 🔹 **Service API for Button Processing**
void Button_Process_Service(void) {
    static uint32_t lastPressTime = 0;

    if (HAL_GetTick() - lastPressTime > 100) {  // Debounce check
        buttonPressed++;
        if (buttonPressed > 5) buttonPressed = 1;
        HAL_Delay(50);  // Extra debounce delay
    }
    lastPressTime = HAL_GetTick();
}

*/

