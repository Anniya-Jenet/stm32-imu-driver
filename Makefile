# -------------------------------------------------------
# Makefile — STM32 IMU Driver
# Target  : STM32F411xE (Cortex-M4)
# Toolchain: arm-none-eabi-gcc
# -------------------------------------------------------

# ---- Toolchain ----
CC      = arm-none-eabi-gcc
AS      = arm-none-eabi-gcc
OBJCOPY = arm-none-eabi-objcopy
SIZE    = arm-none-eabi-size

# ---- Project name ----
TARGET = stm32-imu-driver

# ---- Directories ----
BUILD_DIR = build

# ---- Source files ----
C_SOURCES = \
    Core/main.c \
    Drivers/stm32_i2c.c \
    Drivers/imu.c

AS_SOURCES = startup_stm32.s

# ---- CMSIS include path ----
# Point this to where you downloaded cmsis-device-f4
CMSIS_PATH = cmsis-device-f4/Include

C_INCLUDES = \
    -IDrivers \
	-Icmsis-device-f4/Include \
    -ICMSIS_5/CMSIS/Core/Include
    

# ---- MCU flags ----
MCU = -mcpu=cortex-m4 -mthumb -mfpu=fpv4-sp-d16 -mfloat-abi=soft

# ---- Compiler flags ----
CFLAGS  = $(MCU) $(C_INCLUDES)
CFLAGS += -DSTM32F411xE
CFLAGS += -Wall -Wextra -Wpedantic
CFLAGS += -O2 -g
CFLAGS += -ffunction-sections -fdata-sections
CFLAGS += -std=c11

# ---- Assembler flags ----
ASFLAGS = $(MCU) -x assembler-with-cpp

# ---- Linker flags ----
LDFLAGS  = $(MCU)
LDFLAGS += -T linker.ld
LDFLAGS += -Wl,--gc-sections
LDFLAGS += -Wl,-Map=$(BUILD_DIR)/$(TARGET).map,--cref
LDFLAGS += -nostartfiles
LDFLAGS += -lm

# ---- Object files ----
OBJECTS  = $(addprefix $(BUILD_DIR)/, $(notdir $(C_SOURCES:.c=.o)))
OBJECTS += $(addprefix $(BUILD_DIR)/, $(notdir $(AS_SOURCES:.s=.o)))

# ---- VPATH (so make can find sources in subdirs) ----
vpath %.c $(sort $(dir $(C_SOURCES)))
vpath %.s $(sort $(dir $(AS_SOURCES)))

# -------------------------------------------------------
# Targets
# -------------------------------------------------------

.PHONY: all clean flash size

all: $(BUILD_DIR)/$(TARGET).elf $(BUILD_DIR)/$(TARGET).bin $(BUILD_DIR)/$(TARGET).hex

# Create build dir
$(BUILD_DIR):
	mkdir -p $@

# Compile C files
$(BUILD_DIR)/%.o: %.c | $(BUILD_DIR)
	$(CC) -c $(CFLAGS) $< -o $@

# Assemble startup file
$(BUILD_DIR)/%.o: %.s | $(BUILD_DIR)
	$(AS) -c $(ASFLAGS) $< -o $@

# Link
$(BUILD_DIR)/$(TARGET).elf: $(OBJECTS)
	$(CC) $(OBJECTS) $(LDFLAGS) -o $@
	$(SIZE) $@

# Binary output
$(BUILD_DIR)/$(TARGET).bin: $(BUILD_DIR)/$(TARGET).elf
	$(OBJCOPY) -O binary $< $@

# Intel HEX output
$(BUILD_DIR)/$(TARGET).hex: $(BUILD_DIR)/$(TARGET).elf
	$(OBJCOPY) -O ihex $< $@

# ---- Flash with OpenOCD (uncomment when you have hardware) ----
# flash: $(BUILD_DIR)/$(TARGET).bin
# 	openocd -f interface/stlink.cfg -f target/stm32f4x.cfg \
#	         -c "program $(BUILD_DIR)/$(TARGET).bin verify reset exit 0x08000000"

size: $(BUILD_DIR)/$(TARGET).elf
	$(SIZE) $<

clean:
	rm -rf $(BUILD_DIR)
