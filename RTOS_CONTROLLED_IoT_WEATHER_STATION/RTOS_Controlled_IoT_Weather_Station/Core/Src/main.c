/********************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "math.h"

#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "queue.h"
#include "semphr.h"
#include "event_groups.h"
#include "SEGGER_SYSVIEW.h"

#include "BME280_STM32.h"
#include "service_api.h"
#include "struct.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define DEBUG

#define ADC_READINGS 5
#define CRC8_POLYNOMIAL 0x07  // Standard SMBus CRC-8
#define CRC8_INIT 0xFF        // Initial CRC value
#define RL 10.0  // Load resistor in kÎ©

// CO2 Constants
#define A_CO2 116.6020682
#define B_CO2 -2.769034857
#define R0_CO2 47.50
// Ammonia (NH3) Constants
#define A_NH3 102.2
#define B_NH3 -2.473
#define R0_NH3 1.86
// Ethanol Constants
#define A_Ethanol 77.255
#define B_Ethanol -3.18
#define R0_Ethanol 3.75

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef hadc1;

I2C_HandleTypeDef hi2c1;

UART_HandleTypeDef huart5;

/* USER CODE BEGIN PV */

// Task Handles
//Sensordata_t SD;//Already defined in struct.h for actual data holding so no need to define here.
Sensordata_t SensorData;//defined in struct.h for data use in the tasks

SemaphoreHandle_t SensorDataMutex;

TaskHandle_t SensorTaskHandle;
TaskHandle_t DisplayTaskHandle;
TaskHandle_t UartTaskHandle;

volatile uint8_t buttonPressed = 0;

char uartBuff[100];

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_ADC1_Init(void);
static void MX_I2C1_Init(void);
static void MX_UART5_Init(void);
/* USER CODE BEGIN PFP */
void Task_SensorRead(void *pvParameters);
void Task_DisplayUpdate(void *pvParameters);
void Task_UARTSend(void *pvParameters);

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin);

uint8_t Calculate_CRC8(const uint8_t *data, uint8_t len);
void Write_SensorData(Sensordata_t *data);
void Read_SensorData(Sensordata_t *data);
void HAL_UART_Receive_Service(void);
float calculate_ppm(float, float, float, float);

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */


  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_ADC1_Init();
  MX_I2C1_Init();
  MX_UART5_Init();
  /* USER CODE BEGIN 2 */

  SEGGER_SYSVIEW_Conf();
  HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);	//ensure proper priority grouping for freeRTOS//?
  HAL_NVIC_SetPriority(SysTick_IRQn, 15, 0);//?


  // Initialize BME280
  BME280_Config(OSRS_2, OSRS_16, OSRS_1, MODE_NORMAL, T_SB_0p5, IIR_16);

  // Create RTOS Queue & Semaphore
  SensorDataMutex = xSemaphoreCreateMutex();
  assert_param(SensorDataMutex!=NULL);

  /* Create RTOS Tasks */
  assert_param(xTaskCreate(Task_SensorRead, "SensorTask", 1024, NULL, 3, &SensorTaskHandle) == pdPASS);
//  assert_param(xTaskCreate(Task_DisplayUpdate, "DisplayTask", 1024, NULL, 2, &DisplayTaskHandle) == pdPASS);
  assert_param(xTaskCreate(Task_UARTSend, "UartTask", 1024, NULL, 1, &UartTaskHandle) == pdPASS);

  /* Start Scheduler */
  vTaskStartScheduler();

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief ADC1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC1_Init(void)
{

  /* USER CODE BEGIN ADC1_Init 0 */

  /* USER CODE END ADC1_Init 0 */

  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC1_Init 1 */

  /* USER CODE END ADC1_Init 1 */

  /** Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion)
  */
  hadc1.Instance = ADC1;
  hadc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV2;
  hadc1.Init.Resolution = ADC_RESOLUTION_12B;
  hadc1.Init.ScanConvMode = DISABLE;
  hadc1.Init.ContinuousConvMode = DISABLE;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.NbrOfConversion = 1;
  hadc1.Init.DMAContinuousRequests = DISABLE;
  hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
  */
  sConfig.Channel = ADC_CHANNEL_1;
  sConfig.Rank = 1;
  sConfig.SamplingTime = ADC_SAMPLETIME_3CYCLES;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC1_Init 2 */

  /* USER CODE END ADC1_Init 2 */

}

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 100000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 236;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_10BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}

/**
  * @brief UART5 Initialization Function
  * @param None
  * @retval None
  */
static void MX_UART5_Init(void)
{

  /* USER CODE BEGIN UART5_Init 0 */

  /* USER CODE END UART5_Init 0 */

  /* USER CODE BEGIN UART5_Init 1 */

  /* USER CODE END UART5_Init 1 */
  huart5.Instance = UART5;
  huart5.Init.BaudRate = 115200;
  huart5.Init.WordLength = UART_WORDLENGTH_8B;
  huart5.Init.StopBits = UART_STOPBITS_1;
  huart5.Init.Parity = UART_PARITY_NONE;
  huart5.Init.Mode = UART_MODE_TX_RX;
  huart5.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart5.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart5) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN UART5_Init 2 */

  /* USER CODE END UART5_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15, GPIO_PIN_RESET);

  /*Configure GPIO pin : PA0 */
  GPIO_InitStruct.Pin = GPIO_PIN_0;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : PD12 PD13 PD14 PD15 */
  GPIO_InitStruct.Pin = GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI0_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI0_IRQn);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
void Task_SensorRead(void *pvParameters)
{
    while (1)
    {
	#ifdef DEBUG
        SEGGER_SYSVIEW_PrintfHost("Task_SensorRead is running");
    #endif

        BME280_Measure();

        SD.ADC_Value = Read_ADC_Value(&hadc1);

        SD.mq135_voltage = ((SD.ADC_Value * 5.0) / 4095.0);
        SD.Rs = ((5.0 - SD.mq135_voltage) / SD.mq135_voltage) * RL;

        SD.Co2     = calculate_ppm(SD.Rs, R0_CO2, A_CO2, B_CO2);
        SD.NH3     = calculate_ppm(SD.Rs, R0_NH3, A_NH3, B_NH3);
        SD.Ethanol = calculate_ppm(SD.Rs, R0_Ethanol, A_Ethanol, B_Ethanol);
        SD.AQI = ((SD.Co2 / 10) * 0.5) + ((SD.NH3 * 100) * 0.25) + ((SD.Ethanol * 100) * 0.25);

	#ifdef DEBUG
        SEGGER_SYSVIEW_PrintfHost("Sensor Read Collected");
	#endif

        Write_SensorData(&SD);

	#ifdef DEBUG
        SEGGER_SYSVIEW_PrintfHost("T=%.2f,P=%2f,H=.2f,Co2=%.2f,NH3=%2f,Eth=.2f,AQI=%.2f",SD.Temperature,SD.Pressure,SD.Humidity,SD.Co2,SD.NH3,SD.Ethanol,SD.AQI);
	#endif

	#ifdef DEBUG
        SEGGER_SYSVIEW_PrintfHost("Task_SensorRead Complete Before Delay");
	#endif


        vTaskDelay(500);
	#ifdef DEBUG
        SEGGER_SYSVIEW_PrintfHost("Task_SensorRead Complete After Delay");
	#endif
    }
}
/*
void Task_DisplayUpdate(void *pvParameters)
{
    while (1)
    {
	#ifdef DEBUG
        SEGGER_SYSVIEW_PrintfHost("Task_DisplayUpdate Running");
	#endif

        if (buttonPressed)
        {
            // Send button press event to ESP32
            char displayBuff[20];
            snprintf(displayBuff, sizeof(displayBuff), "PAGE=%d", buttonPressed);
            UART_Transmit_Service((uint8_t *)displayBuff, strlen(displayBuff));

	#ifdef DEBUG
            SEGGER_SYSVIEW_PrintfHost("Button Press Data Sent via UART");
	#endif

            buttonPressed = 0;  // Reset button flag
        }
	#ifdef DEBUG
        SEGGER_SYSVIEW_PrintfHost("Task_DisplayUpdate Complete Before Delay");
	#endif

        vTaskDelay(100);

	#ifdef DEBUG
        SEGGER_SYSVIEW_PrintfHost("Task_DisplayUpdate After Delay Complete");
	#endif
    }
}
*/
void Task_UARTSend(void *pvParameters)
{
    while (1)
    {
	#ifdef DEBUG
        SEGGER_SYSVIEW_PrintfHost("Task_UARTSend Running");
	#endif

        Sensordata_t tempData;
        Read_SensorData(&tempData);

        // Format sensor data as CSV
        snprintf(uartBuff, sizeof(uartBuff), "@%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f",
                 tempData.Temperature, tempData.Pressure, tempData.Humidity,
                 tempData.Co2, tempData.NH3, tempData.Ethanol, tempData.AQI);

        uint8_t crc = Calculate_CRC8((uint8_t *)uartBuff, strlen(uartBuff)); // Compute CRC
        snprintf(uartBuff + strlen(uartBuff), sizeof(uartBuff) - strlen(uartBuff), ",%02X,@", crc);

        UART_Transmit_Service((uint8_t *)uartBuff, strlen(uartBuff));

        uint8_t rxBuffer[4];
        if (!UART_Receive_Service(rxBuffer, sizeof(rxBuffer)))
        {
            SEGGER_SYSVIEW_PrintfHost("Retrying UART Transmission...");
            UART_Transmit_Service((uint8_t *)uartBuff, strlen(uartBuff));
        }

	#ifdef DEBUG
        SEGGER_SYSVIEW_PrintfHost("Task_UARTSend Complete Before Delay");
	#endif

        vTaskDelay(1000);

	#ifdef DEBUG
        SEGGER_SYSVIEW_PrintfHost("Task_UARTSend Complete After Delay");
	#endif
    }
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    static uint32_t lastPressTime = 0;

    if (GPIO_Pin == GPIO_PIN_0)  // Button on PA0
    {
        if (HAL_GetTick() - lastPressTime > 10)  // Debounce check
        {
            buttonPressed++;
            if (buttonPressed > 3)
            {
            	buttonPressed = 1;
            }

            HAL_Delay(50);  // Extra debounce delay
        }
        lastPressTime = HAL_GetTick();
    }
}

uint8_t Calculate_CRC8(const uint8_t *data, uint8_t len)
{
    uint8_t crc = CRC8_INIT;

    for (uint8_t i = 0; i < len; i++)
    {
        crc ^= data[i];  // XOR with input byte

        for (uint8_t j = 0; j < 8; j++)
        {  // Process each bit
            if (crc & 0x80)  // If MSB is 1
                crc = (crc << 1) ^ CRC8_POLYNOMIAL;
            else
                crc = (crc << 1);
        }
    }
	#ifdef DEBUG
        SEGGER_SYSVIEW_PrintfHost("CRC = %d",crc);
	#endif

    return crc;  // Return final CRC
}

float calculate_ppm(float Rs, float R0, float A, float B)
{
    return A * pow((Rs / R0), B);
}

/* USER CODE END 4 */

/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM1 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM1) {
    HAL_IncTick();
  }
  /* USER CODE BEGIN Callback 1 */


  /* USER CODE END Callback 1 */
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
	SEGGER_SYSVIEW_PrintfHost("Assertion Failed:file %s on line %d\r\n", file, line);
	while(1);

  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
