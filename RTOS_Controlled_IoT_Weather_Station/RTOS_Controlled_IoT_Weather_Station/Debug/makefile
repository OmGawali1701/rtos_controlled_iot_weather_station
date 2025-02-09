################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include Middleware/SEGGER/SEGGER/Syscalls/subdir.mk
-include Middleware/SEGGER/SEGGER/subdir.mk
-include Middleware/SEGGER/FreeRTOSV11/subdir.mk
-include Middleware/SEGGER/Config/subdir.mk
-include Middleware/FreeRTOS/portable/MemMang/subdir.mk
-include Middleware/FreeRTOS/portable/GCC/ARM_CM4F/subdir.mk
-include Middleware/FreeRTOS/subdir.mk
-include Drivers/STM32F4xx_HAL_Driver/Src/subdir.mk
-include Core/Startup/subdir.mk
-include Core/Src/subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

OPTIONAL_TOOL_DEPS := \
$(wildcard ../makefile.defs) \
$(wildcard ../makefile.init) \
$(wildcard ../makefile.targets) \


BUILD_ARTIFACT_NAME := RTOS_Controlled_IoT_Weather_Station
BUILD_ARTIFACT_EXTENSION := elf
BUILD_ARTIFACT_PREFIX :=
BUILD_ARTIFACT := $(BUILD_ARTIFACT_PREFIX)$(BUILD_ARTIFACT_NAME)$(if $(BUILD_ARTIFACT_EXTENSION),.$(BUILD_ARTIFACT_EXTENSION),)

# Add inputs and outputs from these tool invocations to the build variables 
EXECUTABLES += \
RTOS_Controlled_IoT_Weather_Station.elf \

MAP_FILES += \
RTOS_Controlled_IoT_Weather_Station.map \

SIZE_OUTPUT += \
default.size.stdout \

OBJDUMP_LIST += \
RTOS_Controlled_IoT_Weather_Station.list \


# All Target
all: main-build

# Main-build Target
main-build: RTOS_Controlled_IoT_Weather_Station.elf secondary-outputs

# Tool invocations
RTOS_Controlled_IoT_Weather_Station.elf RTOS_Controlled_IoT_Weather_Station.map: $(OBJS) $(USER_OBJS) C:\Users\ADMIN\STM32CubeIDE\workspace_1.16.1\RTOS_Controlled_IoT_Weather_Station\STM32F407VGTX_FLASH.ld makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-gcc -o "RTOS_Controlled_IoT_Weather_Station.elf" @"objects.list" $(USER_OBJS) $(LIBS) -mcpu=cortex-m4 -T"C:\Users\ADMIN\STM32CubeIDE\workspace_1.16.1\RTOS_Controlled_IoT_Weather_Station\STM32F407VGTX_FLASH.ld" --specs=nosys.specs -Wl,-Map="RTOS_Controlled_IoT_Weather_Station.map" -Wl,--gc-sections -static -u _printf_float --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -Wl,--start-group -lc -lm -Wl,--end-group
	@echo 'Finished building target: $@'
	@echo ' '

default.size.stdout: $(EXECUTABLES) makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-size  $(EXECUTABLES)
	@echo 'Finished building: $@'
	@echo ' '

RTOS_Controlled_IoT_Weather_Station.list: $(EXECUTABLES) makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-objdump -h -S $(EXECUTABLES) > "RTOS_Controlled_IoT_Weather_Station.list"
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) RTOS_Controlled_IoT_Weather_Station.elf RTOS_Controlled_IoT_Weather_Station.list RTOS_Controlled_IoT_Weather_Station.map default.size.stdout
	-@echo ' '

secondary-outputs: $(SIZE_OUTPUT) $(OBJDUMP_LIST)

fail-specified-linker-script-missing:
	@echo 'Error: Cannot find the specified linker script. Check the linker settings in the build configuration.'
	@exit 2

warn-no-linker-script-specified:
	@echo 'Warning: No linker script specified. Check the linker settings in the build configuration.'

.PHONY: all clean dependents main-build fail-specified-linker-script-missing warn-no-linker-script-specified

-include ../makefile.targets
