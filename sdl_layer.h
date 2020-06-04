#ifndef SDL_LAYER
#define SDL_LAYER

#include <SDL2/SDL.h>

int sdl_layer_init(const char *wname, int width, int height, int scale);

void sdl_layer_draw(uint8_t *buffer, size_t size, int pitch);

void sdl_layer_destroy();

#endif /* SDL_LAYER */
