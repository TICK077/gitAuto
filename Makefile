CC = gcc
CFLAGS = -Iinclude -Wall -O2
OUT = build/gitAuto.exe

SRC = src/gitauto.c

all: $(OUT)

$(OUT): $(SRC)
	mkdir -p build
	$(CC) $(CFLAGS) $^ -o $@

clean:
	rm -rf build
