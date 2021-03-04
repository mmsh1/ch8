CFLAGS = -Wall -Werror -Wextra -std=c89 -pedantic
LIBS = -lSDL2
INCLUDE = -I/usr/include/

all: options chip8

options:
	@echo chip8 build options:
	@echo "CFLAGS  = $(CFLAGS)"
	@echo "CC      = $(CC)"

chip8: chip8.o sdl_layer.o
	$(CC) -o $@ $^ $(INCLUDE) $(LIBS)

sdl_layer.o: sdl_layer.c
	$(CC) -c $(CFLAGS) -o $@ $<

chip8.o: chip8.c
	$(CC) -c $(CFLAGS) -o $@ $<

clean:
	rm -f chip8
	rm -f *.o
