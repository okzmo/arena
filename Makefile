# compiler and flags
CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -Werror -g -O0 -fsanitize=address -fno-omit-frame-pointer

# directories
SRC = src
INC = include
BUILD = build

# source files
SRCS = $(SRC)/main.c $(INC)/okzarena.c

# objects
OBJS = $(SRCS:$(SRC)/%.c=$(BUILD)/%.o)

BIN = app

$(BIN): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS)

run: $(BIN)
	@./$(BIN)

$(BUILD)/%.o: $(SRC)/%.c $(INC)/okzarena.h
	@mkdir -p $(BUILD)
	$(CC) $(CFLAGS) -I$(INC) -c $< -o $@

clean:
	rm -rf $(BUILD) $(BIN)
