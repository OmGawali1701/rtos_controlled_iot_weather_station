/*
 * service_api.h
 *
 *  Created on: Feb 8, 2025
 *      Author: ADMIN
 */

#ifndef SERVICE_API_H
#define SERVICE_API_H

#include "main.h"
#include "struct.h"


// UART APIs
void UART_Transmit_Service(uint8_t *data, uint16_t len);
uint8_t UART_Receive_Service(uint8_t *rxBuffer, uint16_t len);
// GPIO APIs
//void Button_Process_Service(void);

// ADC & Sensor APIs
float Read_ADC_Value(ADC_HandleTypeDef *hadc);
void Read_SensorData(Sensordata_t *data);
void Write_SensorData(Sensordata_t *data);

uint8_t Calculate_CRC8(const uint8_t *data, uint8_t len);


#endif // SERVICE_API_H


