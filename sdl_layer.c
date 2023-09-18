#include "sdl_layer.h"

#include <SDL2/SDL.h>

SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;
SDL_Texture *texture = NULL;

static uint32_t
create_argb(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{
    return (b << 24) | (g << 16) | (r << 8) | a;
}

void
sdl_delay(uint32_t ms)
{
	SDL_Delay(ms);
}

int
sdl_layer_init(const char *wname, int width, int height, int scale)
{
    if (SDL_Init(SDL_INIT_VIDEO|SDL_INIT_AUDIO) != 0) {
        fprintf(stderr, "Error: SDL_Init: %s\n", SDL_GetError());
        goto error;
    }

    window = SDL_CreateWindow(wname, SDL_WINDOWPOS_CENTERED,
                              SDL_WINDOWPOS_CENTERED, width * scale,
                              height * scale, SDL_WINDOW_SHOWN);
    if (!window) {
        fprintf(stderr, "Error: window creation: %s\n", SDL_GetError());
		goto window_error;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        fprintf(stderr, "Error: renderer creation: %s\n", SDL_GetError());
        goto renderer_error;
    }

    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB32,
                                SDL_TEXTUREACCESS_STREAMING, width, height);
    if (!texture) {
        fprintf(stderr, "Error: texture creation: %s\n", SDL_GetError());
    	goto texture_error;
    }

    return 0;

texture_error:
	SDL_DestroyRenderer(renderer);
renderer_error:
	SDL_DestroyWindow(window);
window_error:
	SDL_Quit();
error:
	return -1;
}

void
sdl_handle_keystroke(uint8_t *keys, uint8_t *quit_flag)
{
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
                *quit_flag = 1;
                break;
            case SDL_KEYDOWN: {
                switch (event.key.keysym.sym) {
                    case SDLK_1: keys[1] = 1; break;
                    case SDLK_2: keys[2] = 1; break;
                    case SDLK_3: keys[3] = 1; break;
                    case SDLK_4: keys[12] = 1; break;

                    case SDLK_q: keys[4] = 1; break;
                    case SDLK_w: keys[5] = 1; break;
                    case SDLK_e: keys[6] = 1; break;
                    case SDLK_r: keys[13] = 1; break;

                    case SDLK_a: keys[7] = 1; break;
                    case SDLK_s: keys[8] = 1; break;
                    case SDLK_d: keys[9] = 1; break;
                    case SDLK_f: keys[14] = 1; break;

                    case SDLK_z: keys[10] = 1; break;
                    case SDLK_x: keys[0] = 1; break;
                    case SDLK_c: keys[11] = 1; break;
                    case SDLK_v: keys[15] = 1; break;
                }
                break;
            }
            case SDL_KEYUP: {
                switch (event.key.keysym.sym) {
                    case SDLK_1: keys[1] = 0; break;
                    case SDLK_2: keys[2] = 0; break;
                    case SDLK_3: keys[3] = 0; break;
                    case SDLK_4: keys[12] = 0; break;

                    case SDLK_q: keys[4] = 0; break;
                    case SDLK_w: keys[5] = 0; break;
                    case SDLK_e: keys[6] = 0; break;
                    case SDLK_r: keys[13] = 0; break;

                    case SDLK_a: keys[7] = 0; break;
                    case SDLK_s: keys[8] = 0; break;
                    case SDLK_d: keys[9] = 0; break;
                    case SDLK_f: keys[14] = 0; break;

                    case SDLK_z: keys[10] = 0; break;
                    case SDLK_x: keys[0] = 0; break;
                    case SDLK_c: keys[11] = 0; break;
                    case SDLK_v: keys[15] = 0; break;
                }
                break;
            }
        }
    }
}

void
sdl_layer_draw(uint32_t *output, uint16_t size, uint8_t width)
{
    int i;
    for (i = 0; i < size; i++) {
        if (output[i]) {
            output[i] = create_argb(251, 248, 190, 0);
        } else {
            output[i] = create_argb(35, 78, 112, 0);
        }
    }
    SDL_UpdateTexture(texture, NULL, output, sizeof(output[0]) * width);

    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, texture, NULL, NULL);
    SDL_RenderPresent(renderer);
}

void
sdl_layer_destroy()
{
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window); /* TODO test on NULL */
    SDL_Quit();
}
