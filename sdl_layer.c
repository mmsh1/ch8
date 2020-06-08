#include "sdl_layer.h"

SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;
SDL_Texture *texture = NULL;
//SDL_Rect pixel;

int
sdl_layer_init(const char *wname, int width, int height, int scale)
{
    SDL_Init(SDL_INIT_VIDEO|SDL_INIT_AUDIO);
    window = SDL_CreateWindow(wname,
                              SDL_WINDOWPOS_CENTERED,
                              SDL_WINDOWPOS_CENTERED,
                              width * scale,
                              height * scale,
                              SDL_WINDOW_SHOWN);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_STREAMING, width, height);
    return 0;
}

void
sdl_layer_draw(uint64_t *buffer, uint16_t size)
{
    uint32_t pix[64 * 32];
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
