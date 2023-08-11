################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (9-2020-q2-update)
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include startup/subdir.mk
-include Drivers/STM32L4xx_HAL_Driver/Src/subdir.mk
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


BUILD_ARTIFACT_NAME := TerraMotor_TorMini2G_CAN_MQTT_X
BUILD_ARTIFACT_EXTENSION := elf
BUILD_ARTIFACT_PREFIX :=
BUILD_ARTIFACT := $(BUILD_ARTIFACT_PREFIX)$(BUILD_ARTIFACT_NAME)$(if $(BUILD_ARTIFACT_EXTENSION),.$(BUILD_ARTIFACT_EXTENSION),)

# Add inputs and outputs from these tool invocations to the build variables 
EXECUTABLES += \
TerraMotor_TorMini2G_CAN_MQTT_X.elf \

SIZE_OUTPUT += \
default.size.stdout \

OBJDUMP_LIST += \
TerraMotor_TorMini2G_CAN_MQTT_X.list \

OBJCOPY_HEX += \
TerraMotor_TorMini2G_CAN_MQTT_X.hex \

OBJCOPY_BIN += \
TerraMotor_TorMini2G_CAN_MQTT_X.bin \


# All Target
all: main-build

# Main-build Target
main-build: TerraMotor_TorMini2G_CAN_MQTT_X.elf secondary-outputs

# Tool invocations
TerraMotor_TorMini2G_CAN_MQTT_X.elf: $(OBJS) $(USER_OBJS) ../STM32L433CBUx_FLASH.ld makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-gcc -o "TerraMotor_TorMini2G_CAN_MQTT_X.elf" @"objects.list" $(USER_OBJS) $(LIBS) -mcpu=cortex-m4 -T"../STM32L433CBUx_FLASH.ld" --specs=nosys.specs -Wl,-Map="TerraMotor_TorMini2G_CAN_MQTT_X.map" -Wl,--gc-sections -static -L"D:/Agalya/D Drive/5. NPD_Projects/23_24/Q2/Terra_Motors/Workspace/Terra_Motors-main/with_user_MqttSubSperator/TerraMotor_Mini2G_L433_v0_1_X/BGauss_LIBRARY" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -Wl,--start-group -lc -lm -Wl,--end-group
	@echo 'Finished building target: $@'
	@echo ' '

default.size.stdout: $(EXECUTABLES) makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-size  $(EXECUTABLES)
	@echo 'Finished building: $@'
	@echo ' '

TerraMotor_TorMini2G_CAN_MQTT_X.list: $(EXECUTABLES) makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-objdump -h -S $(EXECUTABLES) > "TerraMotor_TorMini2G_CAN_MQTT_X.list"
	@echo 'Finished building: $@'
	@echo ' '

TerraMotor_TorMini2G_CAN_MQTT_X.hex: $(EXECUTABLES) makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-objcopy  -O ihex $(EXECUTABLES) "TerraMotor_TorMini2G_CAN_MQTT_X.hex"
	@echo 'Finished building: $@'
	@echo ' '

TerraMotor_TorMini2G_CAN_MQTT_X.bin: $(EXECUTABLES) makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-objcopy  -O binary $(EXECUTABLES) "TerraMotor_TorMini2G_CAN_MQTT_X.bin"
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) TerraMotor_TorMini2G_CAN_MQTT_X.bin TerraMotor_TorMini2G_CAN_MQTT_X.elf TerraMotor_TorMini2G_CAN_MQTT_X.hex TerraMotor_TorMini2G_CAN_MQTT_X.list default.size.stdout
	-@echo ' '

secondary-outputs: $(SIZE_OUTPUT) $(OBJDUMP_LIST) $(OBJCOPY_HEX) $(OBJCOPY_BIN)

fail-specified-linker-script-missing:
	@echo 'Error: Cannot find the specified linker script. Check the linker settings in the build configuration.'
	@exit 2

warn-no-linker-script-specified:
	@echo 'Warning: No linker script specified. Check the linker settings in the build configuration.'

.PHONY: all clean dependents main-build fail-specified-linker-script-missing warn-no-linker-script-specified

-include ../makefile.targets
