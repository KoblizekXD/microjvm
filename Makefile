CC := gcc
LD := gcc

BUILD   := build
OUTPUT  := $(BUILD)/main
INCLUDE := include
SRC     := src

CFLAGS  := -Wall -fsanitize=address -larchive -Wextra -pg -g -O0 -std=gnu11 -I $(SRC) -I $(INCLUDE)
LDFLAGS := -g -fsanitize=address -pg -larchive

CFILES := $(shell cd $(SRC) && find -L * -type f -name '*.c' | LC_ALL=C sort)
OBJ    := $(addprefix $(BUILD)/,$(CFILES:.c=.c.o))

.PHONY: all run clean

all: $(OUTPUT)

run: $(OUTPUT)
	chmod +x $(OUTPUT)
	@printf "\033[32m==> Running $(OUTPUT)\033[0m\n"
	@./$(OUTPUT)

clean:
	@printf "\033[32m==> Clean up\033[0m\n"
	rm -rf $(BUILD)
	mkdir -p $(BUILD)

$(OUTPUT): $(OBJ)
	@printf "\033[32m==> Linking object files\033[0m\n"
	mkdir -p "$$(dirname $@)"
	$(LD) $(OBJ) $(LDFLAGS) -o $@

$(BUILD)/%.c.o: $(SRC)/%.c
	mkdir -p "$$(dirname $@)"
	$(CC) $(CFLAGS) -c $< -o $@
