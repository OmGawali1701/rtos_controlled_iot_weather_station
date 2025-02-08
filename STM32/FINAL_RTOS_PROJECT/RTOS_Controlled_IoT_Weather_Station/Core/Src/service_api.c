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

#define DEBUG 1

extern UART_HandleTypeDef huart5;

extern SemaphoreHandle_t SensorDataMutex;
extern Sensordata_t SensorData;

// 🔹 **Service API for UART Transmission**
void UART_Transmit_Service( uint8_t *data, uint16_t len)
{
    if (HAL_UART_Transmit_DMA(&huart5, data, len) != HAL_OK)

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
    if (HAL_UART_Receive(&huart5, rxBuffer, len, 1000) == HAL_OK)
    {
        uint8_t received_crc = rxBuffer[len - 1]; // Last byte is CRC
        uint8_t computed_crc = Calculate_CRC8(rxBuffer, len - 1);

        if (received_crc == computed_crc)
        {
            SEGGER_SYSVIEW_PrintfHost("CRC Matched: Valid Data");
            return 1; // Success
        }
        else
        {
            SEGGER_SYSVIEW_PrintfHost("CRC Mismatch: Data Corrupted");
            return 0; // CRC Failed
        }
    }
    SEGGER_SYSVIEW_PrintfHost("ESP32 Acknowledgment Failed");
    return 0; // Failure
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

