################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (9-2020-q2-update)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal.c \
../Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_adc.c \
../Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_adc_ex.c \
../Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_can.c \
../Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_cortex.c \
../Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_dma.c \
../Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_dma_ex.c \
../Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_exti.c \
../Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_flash.c \
../Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_flash_ex.c \
../Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_flash_ramfunc.c \
../Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_gpio.c \
../Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_i2c.c \
../Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_i2c_ex.c \
../Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_iwdg.c \
../Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_lptim.c \
../Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_pwr.c \
../Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_pwr_ex.c \
../Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_rcc.c \
../Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_rcc_ex.c \
../Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_rng.c \
../Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_rtc.c \
../Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_rtc_ex.c \
../Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_spi.c \
../Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_spi_ex.c \
../Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_tim.c \
../Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_tim_ex.c \
../Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_ll_dma.c \
../Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_ll_exti.c \
../Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_ll_gpio.c \
../Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_ll_rcc.c \
../Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_ll_usart.c \
../Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_ll_utils.c 

OBJS += \
./Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal.o \
./Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_adc.o \
./Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_adc_ex.o \
./Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_can.o \
./Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_cortex.o \
./Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_dma.o \
./Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_dma_ex.o \
./Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_exti.o \
./Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_flash.o \
./Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_flash_ex.o \
./Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_flash_ramfunc.o \
./Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_gpio.o \
./Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_i2c.o \
./Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_i2c_ex.o \
./Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_iwdg.o \
./Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_lptim.o \
./Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_pwr.o \
./Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_pwr_ex.o \
./Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_rcc.o \
./Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_rcc_ex.o \
./Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_rng.o \
./Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_rtc.o \
./Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_rtc_ex.o \
./Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_spi.o \
./Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_spi_ex.o \
./Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_tim.o \
./Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_tim_ex.o \
./Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_ll_dma.o \
./Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_ll_exti.o \
./Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_ll_gpio.o \
./Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_ll_rcc.o \
./Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_ll_usart.o \
./Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_ll_utils.o 

C_DEPS += \
./Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal.d \
./Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_adc.d \
./Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_adc_ex.d \
./Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_can.d \
./Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_cortex.d \
./Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_dma.d \
./Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_dma_ex.d \
./Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_exti.d \
./Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_flash.d \
./Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_flash_ex.d \
./Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_flash_ramfunc.d \
./Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_gpio.d \
./Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_i2c.d \
./Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_i2c_ex.d \
./Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_iwdg.d \
./Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_lptim.d \
./Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_pwr.d \
./Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_pwr_ex.d \
./Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_rcc.d \
./Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_rcc_ex.d \
./Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_rng.d \
./Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_rtc.d \
./Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_rtc_ex.d \
./Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_spi.d \
./Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_spi_ex.d \
./Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_tim.d \
./Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_tim_ex.d \
./Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_ll_dma.d \
./Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_ll_exti.d \
./Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_ll_gpio.d \
./Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_ll_rcc.d \
./Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_ll_usart.d \
./Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_ll_utils.d 


# Each subdirectory must supply rules for building sources it contributes
Drivers/STM32L4xx_HAL_Driver/Src/%.o: ../Drivers/STM32L4xx_HAL_Driver/Src/%.c Drivers/STM32L4xx_HAL_Driver/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DUSE_FULL_LL_DRIVER -DUSE_HAL_DRIVER -DSTM32L433xx -c -I../Core/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../Drivers/CMSIS/Include -I"D:/Agalya/D Drive/5. NPD_Projects/23_24/Q2/Terra_Motors/Workspace/Terra_Motors-main/with_user_MqttSubSperator/TerraMotor_Mini2G_L433_v0_1_X/BGauss_LIBRARY" -Og -ffunction-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Drivers-2f-STM32L4xx_HAL_Driver-2f-Src

clean-Drivers-2f-STM32L4xx_HAL_Driver-2f-Src:
	-$(RM) ./Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal.d ./Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal.o ./Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_adc.d ./Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_adc.o ./Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_adc_ex.d ./Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_adc_ex.o ./Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_can.d ./Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_can.o ./Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_cortex.d ./Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_cortex.o ./Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_dma.d ./Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_dma.o ./Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_dma_ex.d ./Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_dma_ex.o ./Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_exti.d ./Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_exti.o ./Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_flash.d ./Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_flash.o ./Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_flash_ex.d ./Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_flash_ex.o ./Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_flash_ramfunc.d ./Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_flash_ramfunc.o ./Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_gpio.d ./Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_gpio.o ./Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_i2c.d ./Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_i2c.o ./Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_i2c_ex.d ./Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_i2c_ex.o ./Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_iwdg.d ./Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_iwdg.o ./Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_lptim.d ./Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_lptim.o ./Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_pwr.d ./Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_pwr.o ./Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_pwr_ex.d ./Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_pwr_ex.o ./Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_rcc.d ./Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_rcc.o ./Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_rcc_ex.d ./Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_rcc_ex.o ./Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_rng.d ./Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_rng.o ./Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_rtc.d ./Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_rtc.o ./Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_rtc_ex.d ./Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_rtc_ex.o ./Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_spi.d ./Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_spi.o ./Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_spi_ex.d ./Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_spi_ex.o ./Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_tim.d ./Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_tim.o ./Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_tim_ex.d ./Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_tim_ex.o ./Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_ll_dma.d ./Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_ll_dma.o ./Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_ll_exti.d ./Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_ll_exti.o ./Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_ll_gpio.d ./Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_ll_gpio.o ./Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_ll_rcc.d ./Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_ll_rcc.o ./Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_ll_usart.d ./Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_ll_usart.o ./Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_ll_utils.d ./Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_ll_utils.o

.PHONY: clean-Drivers-2f-STM32L4xx_HAL_Driver-2f-Src

