################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (9-2020-q2-update)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/adc.c \
../Core/Src/can.c \
../Core/Src/deviceinfo.c \
../Core/Src/dma.c \
../Core/Src/error_handling.c \
../Core/Src/gpio.c \
../Core/Src/gsmSim868.c \
../Core/Src/i2c.c \
../Core/Src/iwdg.c \
../Core/Src/lptim.c \
../Core/Src/main.c \
../Core/Src/payload.c \
../Core/Src/queue.c \
../Core/Src/rng.c \
../Core/Src/rtc.c \
../Core/Src/serial_comm.c \
../Core/Src/spi.c \
../Core/Src/stack.c \
../Core/Src/stm32l4xx_hal_msp.c \
../Core/Src/stm32l4xx_it.c \
../Core/Src/syscalls.c \
../Core/Src/system_reset.c \
../Core/Src/system_stm32l4xx.c \
../Core/Src/tim.c \
../Core/Src/usart.c \
../Core/Src/user_MqttSubSperator.c \
../Core/Src/user_adc.c \
../Core/Src/user_can.c \
../Core/Src/user_eeprom.c \
../Core/Src/user_flash.c \
../Core/Src/user_rtc.c \
../Core/Src/user_timer.c 

OBJS += \
./Core/Src/adc.o \
./Core/Src/can.o \
./Core/Src/deviceinfo.o \
./Core/Src/dma.o \
./Core/Src/error_handling.o \
./Core/Src/gpio.o \
./Core/Src/gsmSim868.o \
./Core/Src/i2c.o \
./Core/Src/iwdg.o \
./Core/Src/lptim.o \
./Core/Src/main.o \
./Core/Src/payload.o \
./Core/Src/queue.o \
./Core/Src/rng.o \
./Core/Src/rtc.o \
./Core/Src/serial_comm.o \
./Core/Src/spi.o \
./Core/Src/stack.o \
./Core/Src/stm32l4xx_hal_msp.o \
./Core/Src/stm32l4xx_it.o \
./Core/Src/syscalls.o \
./Core/Src/system_reset.o \
./Core/Src/system_stm32l4xx.o \
./Core/Src/tim.o \
./Core/Src/usart.o \
./Core/Src/user_MqttSubSperator.o \
./Core/Src/user_adc.o \
./Core/Src/user_can.o \
./Core/Src/user_eeprom.o \
./Core/Src/user_flash.o \
./Core/Src/user_rtc.o \
./Core/Src/user_timer.o 

C_DEPS += \
./Core/Src/adc.d \
./Core/Src/can.d \
./Core/Src/deviceinfo.d \
./Core/Src/dma.d \
./Core/Src/error_handling.d \
./Core/Src/gpio.d \
./Core/Src/gsmSim868.d \
./Core/Src/i2c.d \
./Core/Src/iwdg.d \
./Core/Src/lptim.d \
./Core/Src/main.d \
./Core/Src/payload.d \
./Core/Src/queue.d \
./Core/Src/rng.d \
./Core/Src/rtc.d \
./Core/Src/serial_comm.d \
./Core/Src/spi.d \
./Core/Src/stack.d \
./Core/Src/stm32l4xx_hal_msp.d \
./Core/Src/stm32l4xx_it.d \
./Core/Src/syscalls.d \
./Core/Src/system_reset.d \
./Core/Src/system_stm32l4xx.d \
./Core/Src/tim.d \
./Core/Src/usart.d \
./Core/Src/user_MqttSubSperator.d \
./Core/Src/user_adc.d \
./Core/Src/user_can.d \
./Core/Src/user_eeprom.d \
./Core/Src/user_flash.d \
./Core/Src/user_rtc.d \
./Core/Src/user_timer.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/%.o: ../Core/Src/%.c Core/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DUSE_FULL_LL_DRIVER -DUSE_HAL_DRIVER -DSTM32L433xx -c -I../Core/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../Drivers/CMSIS/Include -I"D:/Agalya/D Drive/5. NPD_Projects/23_24/Q2/Terra_Motors/Workspace/Terra_Motors-main/with_user_MqttSubSperator/TerraMotor_Mini2G_L433_v0_1_X/BGauss_LIBRARY" -Og -ffunction-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-Src

clean-Core-2f-Src:
	-$(RM) ./Core/Src/adc.d ./Core/Src/adc.o ./Core/Src/can.d ./Core/Src/can.o ./Core/Src/deviceinfo.d ./Core/Src/deviceinfo.o ./Core/Src/dma.d ./Core/Src/dma.o ./Core/Src/error_handling.d ./Core/Src/error_handling.o ./Core/Src/gpio.d ./Core/Src/gpio.o ./Core/Src/gsmSim868.d ./Core/Src/gsmSim868.o ./Core/Src/i2c.d ./Core/Src/i2c.o ./Core/Src/iwdg.d ./Core/Src/iwdg.o ./Core/Src/lptim.d ./Core/Src/lptim.o ./Core/Src/main.d ./Core/Src/main.o ./Core/Src/payload.d ./Core/Src/payload.o ./Core/Src/queue.d ./Core/Src/queue.o ./Core/Src/rng.d ./Core/Src/rng.o ./Core/Src/rtc.d ./Core/Src/rtc.o ./Core/Src/serial_comm.d ./Core/Src/serial_comm.o ./Core/Src/spi.d ./Core/Src/spi.o ./Core/Src/stack.d ./Core/Src/stack.o ./Core/Src/stm32l4xx_hal_msp.d ./Core/Src/stm32l4xx_hal_msp.o ./Core/Src/stm32l4xx_it.d ./Core/Src/stm32l4xx_it.o ./Core/Src/syscalls.d ./Core/Src/syscalls.o ./Core/Src/system_reset.d ./Core/Src/system_reset.o ./Core/Src/system_stm32l4xx.d ./Core/Src/system_stm32l4xx.o ./Core/Src/tim.d ./Core/Src/tim.o ./Core/Src/usart.d ./Core/Src/usart.o ./Core/Src/user_MqttSubSperator.d ./Core/Src/user_MqttSubSperator.o ./Core/Src/user_adc.d ./Core/Src/user_adc.o ./Core/Src/user_can.d ./Core/Src/user_can.o ./Core/Src/user_eeprom.d ./Core/Src/user_eeprom.o ./Core/Src/user_flash.d ./Core/Src/user_flash.o ./Core/Src/user_rtc.d ./Core/Src/user_rtc.o ./Core/Src/user_timer.d ./Core/Src/user_timer.o

.PHONY: clean-Core-2f-Src

