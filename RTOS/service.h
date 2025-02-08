#include "service.h"

float temperature;
uint8_t message[50];

int main(void) {
    HAL_Init();
    SystemClock_Config();

    while (1) {
        // Read Temperature
        SensorService_ReadTemperature(&temperature);

        // Send Data Over UART
        sprintf((char *)message, "Temp: %.2f C\n", temperature);
        UartService_SendData(message, strlen((char *)message));

        // Control LED based on Temperature
        GpioService_ControlLED(temperature > 30.0 ? 1 : 0);

        HAL_Delay(1000);
    }
}