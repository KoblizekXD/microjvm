CC := gcc
LD := gcc

BUILD   := ../../build/microclassfile
OUTPUT  := $(BUILD)/microclassfile.so
SRC     := ..

CFLAGS  := -Wall -Wextra -shared -fPIC -g -O2 -std=gnu11 -I $(SRC) -I $(INCLUDE)
LDFLAGS := -g -shared -fPIC

CFILES := $(shell find -L * -type f -name '*.c' | LC_ALL=C sort)
OBJ    := $(addprefix $(BUILD)/,$(CFILES:.c=.c.o))

.PHONY: all clean

all: $(OUTPUT)

clean:
	@printf "\033[32m==> Clean up\033[0m\n"
	rm -rf $(BUILD)
	mkdir -p $(BUILD)

$(OUTPUT): $(OBJ)
	@printf "\033[32m==> Linking object files\033[0m\n"
	mkdir -p "$$(dirname $@)"
	$(LD) $(OBJ) $(LDFLAGS) -o $@

$(BUILD)/%.c.o: ./%.c
	mkdir -p "$$(dirname $@)"
	$(CC) $(CFLAGS) -c $< -o $@
