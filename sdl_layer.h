#ifndef SDL_LAYER
#define SDL_LAYER

#include <SDL2/SDL.h>

int sdl_layer_init(const char *wname, int width, int height, int scale);

void sdl_layer_draw(uint64_t *buffer, uint16_t size);

void sdl_layer_destroy();

#endif /* SDL_LAYER */
