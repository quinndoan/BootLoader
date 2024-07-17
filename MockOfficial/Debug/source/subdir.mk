################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../source/MKL46Z4_MockProject.c \
../source/MOCK_Driver.c \
../source/mtb.c \
../source/semihost_hardfault.c \
../source/tpm_input_capture.c 

C_DEPS += \
./source/MKL46Z4_MockProject.d \
./source/MOCK_Driver.d \
./source/mtb.d \
./source/semihost_hardfault.d \
./source/tpm_input_capture.d 

OBJS += \
./source/MKL46Z4_MockProject.o \
./source/MOCK_Driver.o \
./source/mtb.o \
./source/semihost_hardfault.o \
./source/tpm_input_capture.o 


# Each subdirectory must supply rules for building sources it contributes
source/%.o: ../source/%.c source/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -std=gnu99 -D__REDLIB__ -DCPU_MKL46Z256VLL4_cm0plus -DCPU_MKL46Z256VLL4 -DFRDM_KL46Z -DFREEDOM -DSDK_DEBUGCONSOLE=1 -DCR_INTEGER_PRINTF -DPRINTF_FLOAT_ENABLE=0 -D__MCUXPRESSO -D__USE_CMSIS -DDEBUG -I"C:\Users\Nitro Tiger\Documents\MCUXpressoIDE_11.9.1_2170\workspace\MockOfficial\source" -I"C:\Users\Nitro Tiger\Documents\MCUXpressoIDE_11.9.1_2170\workspace\MockOfficial" -I"C:\Users\Nitro Tiger\Documents\MCUXpressoIDE_11.9.1_2170\workspace\MockOfficial\CMSIS" -I"C:\Users\Nitro Tiger\Documents\MCUXpressoIDE_11.9.1_2170\workspace\MockOfficial\drivers" -I"C:\Users\Nitro Tiger\Documents\MCUXpressoIDE_11.9.1_2170\workspace\MockOfficial\startup" -I"C:\Users\Nitro Tiger\Documents\MCUXpressoIDE_11.9.1_2170\workspace\MockOfficial\utilities" -I"C:\Users\Nitro Tiger\Documents\MCUXpressoIDE_11.9.1_2170\workspace\MockOfficial\board\src" -I"C:\Users\Nitro Tiger\Documents\MCUXpressoIDE_11.9.1_2170\workspace\MockOfficial\board" -O0 -fno-common -g -gdwarf-4 -Wall -c  -ffunction-sections  -fdata-sections  -ffreestanding  -fno-builtin -fmerge-constants -fmacro-prefix-map="$(<D)/"= -mcpu=cortex-m0plus -mthumb -D__REDLIB__ -fstack-usage -specs=redlib.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-source

clean-source:
	-$(RM) ./source/MKL46Z4_MockProject.d ./source/MKL46Z4_MockProject.o ./source/MOCK_Driver.d ./source/MOCK_Driver.o ./source/mtb.d ./source/mtb.o ./source/semihost_hardfault.d ./source/semihost_hardfault.o ./source/tpm_input_capture.d ./source/tpm_input_capture.o

.PHONY: clean-source

