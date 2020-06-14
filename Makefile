#CFLAGS = -Wall -Werror -Wextra -std=c99 -pedantic -ggdb -lSDL2 -I/usr/include/
CFLAGS = -lSDL2 -g3 -O0 -I/usr/include/
CC = clang

all: options chip8

options:
	@echo chip8 build options:
	@echo "CFLAGS  = $(CFLAGS)"
	@echo "CC      = $(CC)"

chip8: chip8.o sdl_layer.o
	$(CC) $(CFLAGS) -o chip8 chip8.o sdl_layer.o

sdl_layer.o: sdl_layer.c
	$(CC) -c $(CFLAGS) -o sdl_layer.o sdl_layer.c

chip8.o: chip8.c
	$(CC) -c $(CFLAGS) -o chip8.o chip8.c

clean:
	rm -rf *.o *.out chip8

