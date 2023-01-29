#-----------------------------------------------------------------------------------------
#	Defines
#-----------------------------------------------------------------------------------------
TARGET = STM32_LoRa_v2.0
CORE = cortex-m4
FLASHSTART = 0x08000000
INCLUDEDEFINE = \
-DSTM32L431xx

#-----------------------------------------------------------------------------------------
#	Output folders
#-----------------------------------------------------------------------------------------
BUILD_PATH = build
OUTPUT_PATH = output

#-----------------------------------------------------------------------------------------
#	Binaries
#-----------------------------------------------------------------------------------------
PREFIX 		= arm-none-eabi-
CC 			= $(PREFIX)gcc
CXX 		= $(PREFIX)g++
AS 			= $(PREFIX)gcc -x assembler-with-cpp
CP 			= $(PREFIX)objcopy
SZ 			= $(PREFIX)size
OBJDUMP 	= $(PREFIX)objdump
HEX 		= $(CP) -O ihex
BIN 		= $(CP) -O binary -S

# Пути к CMSIS, StdPeriph Lib
#-------------------------------------------------------------------------------
PERIPH_SRC_PATH	    = Periph/source
PERIPH_INC_PATH     = Periph/inc

CORTEX_SRC_PATH     = Cortex/source
CORTEX_INC_PATH     = Cortex/inc

MIDDLEWARE_SRC_PATH = Middleware/source
MIDDLEWARE_INC_PATH = Middleware/inc

EXTDEV_SRC_PATH     = ExternalDevices/source
EXTDEV_INC_PATH     = ExternalDevices/inc

# Пути поиска исходных файлов
#-------------------------------------------------------------------------------
HEADERS := Main/inc
HEADERS += $(PERIPH_INC_PATH)
HEADERS += $(CORTEX_INC_PATH)
HEADERS += $(MIDDLEWARE_INC_PATH)
HEADERS += $(EXTDEV_INC_PATH)

SOURCEDIRS := Main/code
SOURCEDIRS += $(PERIPH_SRC_PATH)
SOURCEDIRS += $(CORTEX_SRC_PATH)
SOURCEDIRS += $(MIDDLEWARE_SRC_PATH)
SOURCEDIRS += $(EXTDEV_SRC_PATH)

#-----------------------------------------------------------------------------------------
#	Defines
#-----------------------------------------------------------------------------------------
ASM_DEFINES =
CANDCPP_DEFINES = $(INCLUDEDEFINE)
#-----------------------------------------------------------------------------------------
#	Includes
#-----------------------------------------------------------------------------------------
INCLUDES = \
-Ix:/NextCloudStorage/ImportantData/VSCode/VSCode_portable/data/user-data/User/globalStorage/semyon-ivanov.cortex-builder/Core\
-Ix:/NextCloudStorage/ImportantData/VSCode/VSCode_portable/data/user-data/User/globalStorage/semyon-ivanov.cortex-builder/Include/stm32l431xx
INCLUDES += $(addprefix -I, $(HEADERS))

#-----------------------------------------------------------------------------------------
#	Sources
#-----------------------------------------------------------------------------------------
ASM_SOURCE = Startup.s

#-----------------------------------------------------------------------------------------
#	Linker
#-----------------------------------------------------------------------------------------
LDSCRIPT = Linker.ld
LDFLAGS += -mthumb -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=fpv4-sp-d16 -march=armv7e-m -lc -Wl,--print-memory-usage
LDFLAGS += -specs=nano.specs
LDFLAGS += -specs=nosys.specs
LDFLAGS += -T$(LDSCRIPT)
LDFLAGS += -Wl,--allow-multiple-definition
#-----------------------------------------------------------------------------------------
#	Source Flags
#-----------------------------------------------------------------------------------------
AFLAGS += -mthumb -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=fpv4-sp-d16 -g -Wa,--warn -x assembler-with-cpp -specs=nano.specs
CFLAGS += -mthumb -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=fpv4-sp-d16 -std=gnu11 -O0 -g -fstack-usage -Wall -specs=nano.specs -Wno-unused-variable -Wno-tautological-compare -Wformat=0

CFLAGS += $(INCLUDES)
CFLAGS += $(CANDCPP_DEFINES)
#-----------------------------------------------------------------------------------------
#	Objects list
#-----------------------------------------------------------------------------------------
OBJS += $(patsubst %.c, %.o, $(wildcard  $(addsuffix /*.c, $(SOURCEDIRS))))
# OBJS += $(patsubst %.s, %.o, $(ASM_SOURCE))  # abs/path/file_name.ext
OBJS += $(addprefix $(BUILD_PATH)/,$(notdir $(ASM_SOURCE:.s=.o)))
DEPS := $(OBJECTS:.o=.d)
-include $(DEPS)
OBJ_FILES := $(notdir $(OBJS))
OBJS := $(addprefix $(BUILD_PATH)/, $(OBJ_FILES))

ASMOUTPUTFILE = $(OBJDUMP) -DSG -t -marm -w --start-address=$(FLASHSTART) --show-raw-insn \
--visualize-jumps --inlines $(OUTPUT_PATH)/$(TARGET).elf \
-Mforce-thumb -Mreg-names-std > $(OUTPUT_PATH)/$(TARGET).s

VPATH += $(SOURCEDIRS)
#-----------------------------------------------------------------------------------------
# Компиляция
#-------------------------------------------------------------------------------
$(BUILD_PATH)/%.o: %.c
		@echo Compiling: $<
		@$(CC) $(CFLAGS) -Wall -MD -c $< -o $@

$(BUILD_PATH)/%.o: %.s
		@$(AS) $(AFLAGS) -c $< -o $@

# Линковка
#-------------------------------------------------------------------------------
$(OUTPUT_PATH)/$(TARGET).elf: $(OBJS)
		@echo "---------------------------------------------------"
		@echo "Linking: $(LDFLAGS)"
		@$(CC) $(LDFLAGS) $^ -o $@


$(OUTPUT_PATH)/%.hex: $(OUTPUT_PATH)/%.elf | $(OUTPUT_PATH)
	$(HEX) $< $@
	$(SZ) $< $@

$(OUTPUT_PATH)/%.bin: $(OUTPUT_PATH)/%.elf | $(OUTPUT_PATH)
	$(BIN) $< $@
	$(ASMOUTPUTFILE)


all: create_path $(OUTPUT_PATH)/$(TARGET).elf $(OUTPUT_PATH)/$(TARGET).hex $(OUTPUT_PATH)/$(TARGET).bin
	@echo "Build Completed."

clean:
	rm -rf $(OUTPUT_PATH)
	rm -rf $(BUILD_PATH)
	@echo "Clean Completed."

create_path:
	@mkdir -p "output"
	@mkdir -p "build"
#	End of Actions
#-----------------------------------------------------------------------------------------