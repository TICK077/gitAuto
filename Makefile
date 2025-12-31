CC = gcc
CFLAGS = -Iinclude -Wall -Wextra -g

SRC = \
	src/main.c \
	src/context.c \
	src/config.c \
	src/git.c \
	src/ignore.c \
	src/pipeline.c \
	src/runtime.c \
	src/scheduler.c \
	platform/win32.c

OBJ = $(SRC:%.c=build/%.o)
OUT = build/gitAuto.exe

all: $(OUT)

# 编译规则（自动创建目录）
build/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

# 链接
$(OUT): $(OBJ)
	$(CC) $(OBJ) -o $@

clean:
	rm -rf build
