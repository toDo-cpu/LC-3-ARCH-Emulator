CC=gcc
TARGET=vm_main

all:
	$(CC) main.c includes/log.c includes/memory.c includes/helpers.c -o $(TARGET)
clean:
	rm $(TARGET)