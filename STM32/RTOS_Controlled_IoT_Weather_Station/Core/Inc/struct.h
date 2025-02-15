/*
 * struct.h
 *
 *  Created on: Feb 4, 2025
 *      Author: ADMIN
 */

#ifndef INC_STRUCT_H_
#define INC_STRUCT_H_

typedef struct SENSOR{
	float Temperature;
	float Pressure;
	float Humidity;
	uint16_t ADC_Value;
	float mq135_voltage;
	float Rs;
	float Co2;
	float NH3;
	float Ethanol;
	float AQI;
}Sensordata_t;

extern Sensordata_t SD;//Structure Variable to store different sensor values
extern Sensordata_t SensorData;//Structure Variable to store different sensor values




#endif /* INC_STRUCT_H_ */
