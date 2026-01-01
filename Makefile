CC = gcc
CFLAGS = -Wall -O2 -Iinclude

SRC_DIR = src
OBJ_DIR = build/obj
BIN_DIR = build

TARGET = $(BIN_DIR)/gitauto.exe

SRC = \
 $(SRC_DIR)/main.c \
 $(SRC_DIR)/config.c \
 $(SRC_DIR)/git.c

OBJ = \
 $(OBJ_DIR)/main.o \
 $(OBJ_DIR)/config.o \
 $(OBJ_DIR)/git.o

all: $(TARGET)

$(TARGET): $(OBJ)
	@mkdir -p $(BIN_DIR)
	$(CC) $(OBJ) -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c include/gitauto.h
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf build
