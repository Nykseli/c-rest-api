# Makefile for building. It expects variables to be passed in for:
#
# MODE         "debug" or "release".
# NAME         Name of the output executable (and object file directory).
# SOURCE_DIR   Directory where source files and headers are found.
CC=gcc
CFLAGS := -std=c99 -pthread -Wall -Wextra -Werror -Wno-unused-parameter -Wno-unused-function
LD=ld
SOURCE_DIR = src

ifndef NAME
	NAME=restapi
endif

# Mode configuration.
ifeq ($(MODE),debug)
	CFLAGS += -O0 -DDEBUG -g
	BUILD_DIR := build/debug
else
	CFLAGS += -O3 -flto
	BUILD_DIR := build/release
endif

# Files.
HEADERS := $(wildcard $(SOURCE_DIR)/*.h)
HEADERS += $(wildcard $(SOURCE_DIR)/*/*.h)
SOURCES := $(wildcard $(SOURCE_DIR)/*.c)
SOURCES += $(wildcard $(SOURCE_DIR)/*/*.c)
OBJECTS := $(addprefix $(BUILD_DIR)/$(NAME)/, $(notdir $(SOURCES:.c=.o)))
# Targets ---------------------------------------------------------------------

# Link the interpreter.
build/$(NAME): $(OBJECTS)
	@ printf "%8s %-40s %s\n" $(LD) $@.o
	@ mkdir -p build
	@ $(LD) -r $^ -o build/$(NAME).o

# Compile object files.
$(BUILD_DIR)/$(NAME)/%.o: $(SOURCE_DIR)/%.c $(HEADERS)
	@ printf "%8s %-40s %s\n" $(CC) $<
	@ mkdir -p $(BUILD_DIR)/$(NAME)
	@ $(CC) -c $(CFLAGS) -o $@ $<

# Build subdirectory folders
# TODO: is there cleaner way to do this?
$(BUILD_DIR)/$(NAME)/%.o: $(SOURCE_DIR)/utils/%.c $(HEADERS)
	@ printf "%8s %-40s %s\n" $(CC) $<
	@ mkdir -p $(BUILD_DIR)/$(NAME)
	@ $(CC) -c $(CFLAGS) -o $@ $<

$(BUILD_DIR)/$(NAME)/%.o: $(SOURCE_DIR)/requests/%.c $(HEADERS)
	@ printf "%8s %-40s %s\n" $(CC) $<
	@ mkdir -p $(BUILD_DIR)/$(NAME)
	@ $(CC) -c $(CFLAGS) -o $@ $<

clean:
	find . -type f -name "*.o" | xargs rm -v

.PHONY: default
