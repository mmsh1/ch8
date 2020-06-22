#ifndef SDL_LAYER
#define SDL_LAYER

#include <SDL2/SDL.h>

int sdl_layer_init(const char *wname, int width, int height, int scale);

void sdl_handle_keystroke(uint8_t *keys, uint8_t *quit_flag);

void sdl_layer_draw(uint64_t *buffer, uint32_t *pix, uint16_t size);

void sdl_layer_destroy();

#endif /* SDL_LAYER */
