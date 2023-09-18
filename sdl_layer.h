#ifndef SDL_LAYER
#define SDL_LAYER

#include <stdint.h>

void sdl_delay(uint32_t);
int sdl_layer_init(const char *, int, int, int);
void sdl_handle_keystroke(uint8_t *, uint8_t *);
void sdl_layer_draw(uint32_t *, uint16_t, uint8_t);
void sdl_layer_destroy();

#endif /* SDL_LAYER */
