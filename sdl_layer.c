#include "sdl_layer.h"

SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;
SDL_Texture *texture = NULL;

uint8_t keyboard[16] = {
    SDLK_1,
    SDLK_2,
    SDLK_3,
    SDLK_4,
    SDLK_q,
    SDLK_w,
    SDLK_e,
    SDLK_r,
    SDLK_a,
    SDLK_s,
    SDLK_d,
    SDLK_f,
    SDLK_z,
    SDLK_x,
    SDLK_c,
    SDLK_v
};

int
sdl_layer_init(const char *wname, int width, int height, int scale)
{
    if (SDL_Init(SDL_INIT_VIDEO|SDL_INIT_AUDIO) != 0) {
        fprintf(stderr, "Error: SDL_Init failed. %s\n", SDL_GetError());
        return -1;
    }
    window = SDL_CreateWindow(wname,
                              SDL_WINDOWPOS_CENTERED,
                              SDL_WINDOWPOS_CENTERED,
                              width * scale,
                              height * scale,
                              SDL_WINDOW_SHOWN);

    if (!window) {
        fprintf(stderr, "Error: window creation failed. %s\n", SDL_GetError());
        SDL_Quit();
        return -1;
    }
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        fprintf(stderr, "Error: renderer creation failed. %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }
    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_STREAMING, width, height);
    if (!texture) {
        fprintf(stderr, "Error: texture creation failed. %s\n", SDL_GetError());
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }
    return 0;
}

void
sdl_layer_draw(uint64_t *buffer, uint32_t *pix, uint16_t size)
{
    for(int i = 0; i < size; i++) {
        pix[i] = 0xFFFFFFFF * ((buffer[i / 64] >> (63 - i % 64)) & 1);
    }
    SDL_UpdateTexture(texture, NULL, pix, 256);
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
}
