#ifndef SDL_LAYER
#define SDL_LAYER

/* #define SDL_DISABLE_IMMINTRIN_H 1 */ /* for TCC compiler */

/* gray:        88888888 */
/* dark gray:   22222222 */
/* light green: BBCCFFCC */
/* blue:        00EE8800 */
/* teal:        00AA8800 */


#define BG_COLOR 0x0AAA8800
#define FG_COLOR 0xFFFFFFFF

#include <SDL2/SDL.h>


int sdl_layer_init(const char *, int, int, int);

void sdl_handle_keystroke(uint8_t *, uint8_t *);

void sdl_layer_draw(uint32_t *, uint16_t, uint8_t);

void sdl_layer_destroy();

#endif /* SDL_LAYER */
