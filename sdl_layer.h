#ifndef SDL_LAYER
#define SDL_LAYER

/* #define SDL_DISABLE_IMMINTRIN_H 1 */ /* for TCC compiler */
/* TODO add bg and fg colors */

#include <SDL2/SDL.h>


int sdl_layer_init(const char *, int, int, int);

void sdl_handle_keystroke(uint8_t *, uint8_t *);

void sdl_layer_draw(uint32_t *, uint8_t);

void sdl_layer_destroy();

#endif /* SDL_LAYER */
