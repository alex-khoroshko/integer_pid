PROJ_NAME := integer_pid

OPT_RELEASE := -Os -g0
OPT_DEBUG   := -O0 -g3

OPT := $(OPT_DEBUG)
CPP_FLAGS := -std=c++11\
             -Wall -Wextra -Wpedantic -pedantic-errors -Wduplicated-branches \
             -Wduplicated-cond -Wfloat-equal -Wshadow=compatible-local \
             -Wcast-qual -Wconversion -Wsign-conversion -Wlogical-op  \
             -Wcast-align #-Werror

BUILD_DIR := build

CC = $(GCC_PATH)gcc
LD = $(GCC_PATH)g++
SZ = $(GCC_PATH)size
OBJ = $(GCC_PATH)objcopy

CPP_FILES := $(shell find -maxdepth 10 -type f -name "*.cpp" \
			-not -path "test" -not -path "simpletest")
test: CPP_FILES += $(shell find -maxdepth 10 -type f -path test -path simpletest \
		-name "*.cpp")

O_CPP_FILES := $(addprefix $(BUILD_DIR)/obj/, $(CPP_FILES))
O_FILES := $(patsubst %.cpp, %.o, $(O_CPP_FILES))
DEP_FILES := $(shell find -maxdepth 10 -type f -path "./build/*" -name "*.d")

PROJ_DIRS  := $(shell find -maxdepth 100 -type d -name "*")
PROJ_PATHS := $(addprefix -I, $(PROJ_DIRS))

LDFLAGS := -fdata-sections -ffunction-sections -Wl,--gc-sections \
           -Wl,-Map=${BUILD_DIR}/${PROJ_NAME}.map 

$(BUILD_DIR)/obj/%.o: %.cpp Makefile 
	@echo [CPP] $<
	@mkdir -p ${BUILD_DIR}
	@mkdir -p $(dir $@)
	@$(CC) $(MC) $(OPT) $(CPP_FLAGS) $(PROJ_PATHS) -c $< -o $@  -MMD -MP -MF $(BUILD_DIR)/obj/$*.d

$(BUILD_DIR)/$(PROJ_NAME).elf: $(O_FILES)
	@$(LD) $(MC) $(LDFLAGS) $(O_FILES) -o $(BUILD_DIR)/$(PROJ_NAME).elf
	@echo 'Completed: $@'

size: $(BUILD_DIR)/$(PROJ_NAME).elf
	@$(SZ) $< 

all: $(BUILD_DIR)/$(PROJ_NAME).elf size

hex: $(BUILD_DIR)/$(PROJ_NAME).elf
	@$(OBJ) -j .text -j .data -O ihex $< $(BUILD_DIR)/$(PROJ_NAME).hex
	@echo 'Completed: $@'

clean:
	@if [ -d "$(BUILD_DIR)" ]; then rm -R $(BUILD_DIR); fi
	@echo 'Project cleaned'
	
$(BUILD_DIR)/test.elf: $(O_FILES)
	@echo [LD] $(BUILD_DIR)/test.elf
	@$(LD) $(MC) $(LDFLAGS) $(O_FILES) -o $(BUILD_DIR)/test.elf
	@echo 'Completed: $@'	
	
test: $(BUILD_DIR)/test.elf
	@$(SZ) $< 	
	
include $(DEP_FILES)
	