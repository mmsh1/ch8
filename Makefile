CFLAGS = -Wall -Werror -Wextra -std=c99 -pedantic -lSDL2 -I/usr/include/
#TODO rewrite Makefile to compiler independence

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

