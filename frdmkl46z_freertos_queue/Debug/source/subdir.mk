################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../source/freertos_queue.c \
../source/mock.c \
../source/mock2Handle\ SREC.c \
../source/mock3AddHandleCheckSum.c \
../source/mtb.c \
../source/semihost_hardfault.c 

C_DEPS += \
./source/freertos_queue.d \
./source/mock.d \
./source/mock2Handle\ SREC.d \
./source/mock3AddHandleCheckSum.d \
./source/mtb.d \
./source/semihost_hardfault.d 

OBJS += \
./source/freertos_queue.o \
./source/mock.o \
./source/mock2Handle\ SREC.o \
./source/mock3AddHandleCheckSum.o \
./source/mtb.o \
./source/semihost_hardfault.o 


# Each subdirectory must supply rules for building sources it contributes
source/%.o: ../source/%.c source/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -std=gnu99 -D__REDLIB__ -DCPU_MKL46Z256VLL4_cm0plus -DCPU_MKL46Z256VLL4 -DFRDM_KL46Z -DFREEDOM -DFSL_RTOS_FREE_RTOS -DSDK_DEBUGCONSOLE=1 -DCR_INTEGER_PRINTF -DPRINTF_FLOAT_ENABLE=0 -D__MCUXPRESSO -D__USE_CMSIS -DDEBUG -I"C:\Users\Nitro Tiger\Documents\MCUXpressoIDE_11.9.1_2170\workspace\frdmkl46z_freertos_queue\source" -I"C:\Users\Nitro Tiger\Documents\MCUXpressoIDE_11.9.1_2170\workspace\frdmkl46z_freertos_queue" -I"C:\Users\Nitro Tiger\Documents\MCUXpressoIDE_11.9.1_2170\workspace\frdmkl46z_freertos_queue\CMSIS" -I"C:\Users\Nitro Tiger\Documents\MCUXpressoIDE_11.9.1_2170\workspace\frdmkl46z_freertos_queue\drivers" -I"C:\Users\Nitro Tiger\Documents\MCUXpressoIDE_11.9.1_2170\workspace\frdmkl46z_freertos_queue\startup" -I"C:\Users\Nitro Tiger\Documents\MCUXpressoIDE_11.9.1_2170\workspace\frdmkl46z_freertos_queue\utilities" -I"C:\Users\Nitro Tiger\Documents\MCUXpressoIDE_11.9.1_2170\workspace\frdmkl46z_freertos_queue\amazon-freertos\FreeRTOS\portable" -I"C:\Users\Nitro Tiger\Documents\MCUXpressoIDE_11.9.1_2170\workspace\frdmkl46z_freertos_queue\freertos\portable" -I"C:\Users\Nitro Tiger\Documents\MCUXpressoIDE_11.9.1_2170\workspace\frdmkl46z_freertos_queue\amazon-freertos\include" -I"C:\Users\Nitro Tiger\Documents\MCUXpressoIDE_11.9.1_2170\workspace\frdmkl46z_freertos_queue\board\src" -I"C:\Users\Nitro Tiger\Documents\MCUXpressoIDE_11.9.1_2170\workspace\frdmkl46z_freertos_queue\board" -O0 -fno-common -g -gdwarf-4 -Wall -c  -ffunction-sections  -fdata-sections  -ffreestanding  -fno-builtin -fmerge-constants -fmacro-prefix-map="$(<D)/"= -mcpu=cortex-m0plus -mthumb -D__REDLIB__ -fstack-usage -specs=redlib.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

source/mock2Handle\ SREC.o: ../source/mock2Handle\ SREC.c source/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -std=gnu99 -D__REDLIB__ -DCPU_MKL46Z256VLL4_cm0plus -DCPU_MKL46Z256VLL4 -DFRDM_KL46Z -DFREEDOM -DFSL_RTOS_FREE_RTOS -DSDK_DEBUGCONSOLE=1 -DCR_INTEGER_PRINTF -DPRINTF_FLOAT_ENABLE=0 -D__MCUXPRESSO -D__USE_CMSIS -DDEBUG -I"C:\Users\Nitro Tiger\Documents\MCUXpressoIDE_11.9.1_2170\workspace\frdmkl46z_freertos_queue\source" -I"C:\Users\Nitro Tiger\Documents\MCUXpressoIDE_11.9.1_2170\workspace\frdmkl46z_freertos_queue" -I"C:\Users\Nitro Tiger\Documents\MCUXpressoIDE_11.9.1_2170\workspace\frdmkl46z_freertos_queue\CMSIS" -I"C:\Users\Nitro Tiger\Documents\MCUXpressoIDE_11.9.1_2170\workspace\frdmkl46z_freertos_queue\drivers" -I"C:\Users\Nitro Tiger\Documents\MCUXpressoIDE_11.9.1_2170\workspace\frdmkl46z_freertos_queue\startup" -I"C:\Users\Nitro Tiger\Documents\MCUXpressoIDE_11.9.1_2170\workspace\frdmkl46z_freertos_queue\utilities" -I"C:\Users\Nitro Tiger\Documents\MCUXpressoIDE_11.9.1_2170\workspace\frdmkl46z_freertos_queue\amazon-freertos\FreeRTOS\portable" -I"C:\Users\Nitro Tiger\Documents\MCUXpressoIDE_11.9.1_2170\workspace\frdmkl46z_freertos_queue\freertos\portable" -I"C:\Users\Nitro Tiger\Documents\MCUXpressoIDE_11.9.1_2170\workspace\frdmkl46z_freertos_queue\amazon-freertos\include" -I"C:\Users\Nitro Tiger\Documents\MCUXpressoIDE_11.9.1_2170\workspace\frdmkl46z_freertos_queue\board\src" -I"C:\Users\Nitro Tiger\Documents\MCUXpressoIDE_11.9.1_2170\workspace\frdmkl46z_freertos_queue\board" -O0 -fno-common -g -gdwarf-4 -Wall -c  -ffunction-sections  -fdata-sections  -ffreestanding  -fno-builtin -fmerge-constants -fmacro-prefix-map="$(<D)/"= -mcpu=cortex-m0plus -mthumb -D__REDLIB__ -fstack-usage -specs=redlib.specs -MMD -MP -MF"source/mock2Handle SREC.d" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-source

clean-source:
	-$(RM) ./source/freertos_queue.d ./source/freertos_queue.o ./source/mock.d ./source/mock.o ./source/mock2Handle\ SREC.d ./source/mock2Handle\ SREC.o ./source/mock3AddHandleCheckSum.d ./source/mock3AddHandleCheckSum.o ./source/mtb.d ./source/mtb.o ./source/semihost_hardfault.d ./source/semihost_hardfault.o

.PHONY: clean-source

