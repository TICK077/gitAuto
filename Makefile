CC = gcc
CFLAGS = -Wall -Wextra -O2 -Iinclude

BUILD_DIR = build
OBJ_DIR   = $(BUILD_DIR)/obj
TARGET    = $(BUILD_DIR)/gitauto.exe

SRCS = \
 src/main.c \
 src/git.c \
 src/config.c \
 src/ignore.c \
 src/manual_push.c \
 src/auto_push.c \
 src/platform_win32.c

OBJS = $(SRCS:src/%.c=$(OBJ_DIR)/%.o)

# ==============================

all: $(TARGET)

# 链接
$(TARGET): $(OBJS)
	mkdir -p $(BUILD_DIR)
	$(CC) $(OBJS) -o $@

# 编译
$(OBJ_DIR)/%.o: src/%.c
	mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# 清理
clean:
	rm -rf $(BUILD_DIR)

.PHONY: all clean
