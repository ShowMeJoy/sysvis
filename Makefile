CC = gcc
CFLAGS = -Wall -Wextra -std=c17 -Isrc -O2
TARGET = tracer
SRC = src/main.c src/process.c src/syscalls.c src/tracer.c

all: build

build: $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET)

run: build
	./$(TARGET)

clean:
	rm -f $(TARGET)