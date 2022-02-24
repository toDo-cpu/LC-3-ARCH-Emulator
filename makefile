CC=gcc
TARGET=vm_main

all:
	$(CC) main.c src/log.c src/memory.c src/helpers.c -o $(TARGET)
clean:
	rm $(TARGET)