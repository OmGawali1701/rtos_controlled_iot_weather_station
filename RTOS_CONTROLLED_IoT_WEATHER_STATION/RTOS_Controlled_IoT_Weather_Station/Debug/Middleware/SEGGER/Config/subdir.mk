################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Middleware/SEGGER/Config/SEGGER_SYSVIEW_Config_FreeRTOS.c 

OBJS += \
./Middleware/SEGGER/Config/SEGGER_SYSVIEW_Config_FreeRTOS.o 

C_DEPS += \
./Middleware/SEGGER/Config/SEGGER_SYSVIEW_Config_FreeRTOS.d 


# Each subdirectory must supply rules for building sources it contributes
Middleware/SEGGER/Config/%.o Middleware/SEGGER/Config/%.su Middleware/SEGGER/Config/%.cyclo: ../Middleware/SEGGER/Config/%.c Middleware/SEGGER/Config/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F407xx -c -I../Core/Inc -I"C:/Users/ADMIN/STM32CubeIDE/workspace_1.16.1/RTOS_Controlled_IoT_Weather_Station/Middleware/FreeRTOS/portable/GCC/ARM_CM4F" -I"C:/Users/ADMIN/STM32CubeIDE/workspace_1.16.1/RTOS_Controlled_IoT_Weather_Station/Middleware/FreeRTOS/include" -I"C:/Users/ADMIN/STM32CubeIDE/workspace_1.16.1/RTOS_Controlled_IoT_Weather_Station/Middleware/FreeRTOS" -I"C:/Users/ADMIN/STM32CubeIDE/workspace_1.16.1/RTOS_Controlled_IoT_Weather_Station/Middleware/SEGGER/SEGGER" -I"C:/Users/ADMIN/STM32CubeIDE/workspace_1.16.1/RTOS_Controlled_IoT_Weather_Station/Middleware/SEGGER/FreeRTOSV11" -I"C:/Users/ADMIN/STM32CubeIDE/workspace_1.16.1/RTOS_Controlled_IoT_Weather_Station/Middleware/SEGGER/Config" -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Middleware-2f-SEGGER-2f-Config

clean-Middleware-2f-SEGGER-2f-Config:
	-$(RM) ./Middleware/SEGGER/Config/SEGGER_SYSVIEW_Config_FreeRTOS.cyclo ./Middleware/SEGGER/Config/SEGGER_SYSVIEW_Config_FreeRTOS.d ./Middleware/SEGGER/Config/SEGGER_SYSVIEW_Config_FreeRTOS.o ./Middleware/SEGGER/Config/SEGGER_SYSVIEW_Config_FreeRTOS.su

.PHONY: clean-Middleware-2f-SEGGER-2f-Config

