################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include utilities/subdir.mk
-include startup/subdir.mk
-include source/subdir.mk
-include freertos/portable/subdir.mk
-include drivers/subdir.mk
-include board/src/subdir.mk
-include board/subdir.mk
-include amazon-freertos/FreeRTOS/portable/subdir.mk
-include amazon-freertos/FreeRTOS/subdir.mk
-include CMSIS/subdir.mk
ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

OPTIONAL_TOOL_DEPS := \
$(wildcard ../makefile.defs) \
$(wildcard ../makefile.init) \
$(wildcard ../makefile.targets) \


BUILD_ARTIFACT_NAME := frdmkl46z_freertos_queue
BUILD_ARTIFACT_EXTENSION := axf
BUILD_ARTIFACT_PREFIX :=
BUILD_ARTIFACT := $(BUILD_ARTIFACT_PREFIX)$(BUILD_ARTIFACT_NAME)$(if $(BUILD_ARTIFACT_EXTENSION),.$(BUILD_ARTIFACT_EXTENSION),)

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all:
	+@$(MAKE) --no-print-directory main-build && $(MAKE) --no-print-directory post-build

# Main-build Target
main-build: frdmkl46z_freertos_queue.axf

# Tool invocations
frdmkl46z_freertos_queue.axf: $(OBJS) $(USER_OBJS) makefile $(OPTIONAL_TOOL_DEPS)
	@echo 'Building target: $@'
	@echo 'Invoking: MCU Linker'
	arm-none-eabi-gcc -nostdlib -Xlinker -Map="frdmkl46z_freertos_queue.map" -Xlinker --gc-sections -Xlinker -print-memory-usage -Xlinker --sort-section=alignment -Xlinker --cref -mcpu=cortex-m0plus -mthumb -T frdmkl46z_freertos_queue_Debug.ld -o "frdmkl46z_freertos_queue.axf" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) frdmkl46z_freertos_queue.axf
	-@echo ' '

post-build:
	-@echo 'Performing post-build steps'
	-arm-none-eabi-size "frdmkl46z_freertos_queue.axf"; # arm-none-eabi-objcopy -v -O binary "frdmkl46z_freertos_queue.axf" "frdmkl46z_freertos_queue.bin" ; # checksum -p MKL46Z256xxx4 -d "frdmkl46z_freertos_queue.bin";
	-@echo ' '

.PHONY: all clean dependents main-build post-build

-include ../makefile.targets
