################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Middleware/SEGGER/SEGGER/SEGGER_RTT.c \
../Middleware/SEGGER/SEGGER/SEGGER_RTT_printf.c \
../Middleware/SEGGER/SEGGER/SEGGER_SYSVIEW.c 

S_UPPER_SRCS += \
../Middleware/SEGGER/SEGGER/SEGGER_RTT_ASM_ARMv7M.S 

OBJS += \
./Middleware/SEGGER/SEGGER/SEGGER_RTT.o \
./Middleware/SEGGER/SEGGER/SEGGER_RTT_ASM_ARMv7M.o \
./Middleware/SEGGER/SEGGER/SEGGER_RTT_printf.o \
./Middleware/SEGGER/SEGGER/SEGGER_SYSVIEW.o 

S_UPPER_DEPS += \
./Middleware/SEGGER/SEGGER/SEGGER_RTT_ASM_ARMv7M.d 

C_DEPS += \
./Middleware/SEGGER/SEGGER/SEGGER_RTT.d \
./Middleware/SEGGER/SEGGER/SEGGER_RTT_printf.d \
./Middleware/SEGGER/SEGGER/SEGGER_SYSVIEW.d 


# Each subdirectory must supply rules for building sources it contributes
Middleware/SEGGER/SEGGER/%.o Middleware/SEGGER/SEGGER/%.su Middleware/SEGGER/SEGGER/%.cyclo: ../Middleware/SEGGER/SEGGER/%.c Middleware/SEGGER/SEGGER/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F407xx -c -I../Core/Inc -I"C:/Users/ADMIN/STM32CubeIDE/workspace_1.16.1/RTOS_Controlled_IoT_Weather_Station/Middleware/FreeRTOS/portable/GCC/ARM_CM4F" -I"C:/Users/ADMIN/STM32CubeIDE/workspace_1.16.1/RTOS_Controlled_IoT_Weather_Station/Middleware/FreeRTOS/include" -I"C:/Users/ADMIN/STM32CubeIDE/workspace_1.16.1/RTOS_Controlled_IoT_Weather_Station/Middleware/FreeRTOS" -I"C:/Users/ADMIN/STM32CubeIDE/workspace_1.16.1/RTOS_Controlled_IoT_Weather_Station/Middleware/SEGGER/SEGGER" -I"C:/Users/ADMIN/STM32CubeIDE/workspace_1.16.1/RTOS_Controlled_IoT_Weather_Station/Middleware/SEGGER/FreeRTOSV11" -I"C:/Users/ADMIN/STM32CubeIDE/workspace_1.16.1/RTOS_Controlled_IoT_Weather_Station/Middleware/SEGGER/Config" -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Middleware/SEGGER/SEGGER/%.o: ../Middleware/SEGGER/SEGGER/%.S Middleware/SEGGER/SEGGER/subdir.mk
	arm-none-eabi-gcc -mcpu=cortex-m4 -g3 -DDEBUG -c -x assembler-with-cpp -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@" "$<"

clean: clean-Middleware-2f-SEGGER-2f-SEGGER

clean-Middleware-2f-SEGGER-2f-SEGGER:
	-$(RM) ./Middleware/SEGGER/SEGGER/SEGGER_RTT.cyclo ./Middleware/SEGGER/SEGGER/SEGGER_RTT.d ./Middleware/SEGGER/SEGGER/SEGGER_RTT.o ./Middleware/SEGGER/SEGGER/SEGGER_RTT.su ./Middleware/SEGGER/SEGGER/SEGGER_RTT_ASM_ARMv7M.d ./Middleware/SEGGER/SEGGER/SEGGER_RTT_ASM_ARMv7M.o ./Middleware/SEGGER/SEGGER/SEGGER_RTT_printf.cyclo ./Middleware/SEGGER/SEGGER/SEGGER_RTT_printf.d ./Middleware/SEGGER/SEGGER/SEGGER_RTT_printf.o ./Middleware/SEGGER/SEGGER/SEGGER_RTT_printf.su ./Middleware/SEGGER/SEGGER/SEGGER_SYSVIEW.cyclo ./Middleware/SEGGER/SEGGER/SEGGER_SYSVIEW.d ./Middleware/SEGGER/SEGGER/SEGGER_SYSVIEW.o ./Middleware/SEGGER/SEGGER/SEGGER_SYSVIEW.su

.PHONY: clean-Middleware-2f-SEGGER-2f-SEGGER

