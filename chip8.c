#include <stdlib.h>
#include <string.h>

#include "chip8.h"
#include "sdl_layer.h"

#define BYTE_TO_BINARY_PATTERN "%c%c%c%c%c%c%c%c\n"
#define BYTE_TO_BINARY(byte)  \
(byte & 0x80 ? '1' : '0'), \
(byte & 0x40 ? '1' : '0'), \
(byte & 0x20 ? '1' : '0'), \
(byte & 0x10 ? '1' : '0'), \
(byte & 0x08 ? '1' : '0'), \
(byte & 0x04 ? '1' : '0'), \
(byte & 0x02 ? '1' : '0'), \
(byte & 0x01 ? '1' : '0')

#define PRINTF_BINARY_PATTERN_INT8 "%c%c%c%c%c%c%c%c"
#define PRINTF_BYTE_TO_BINARY_INT8(i)    \
(((i) & 0x80ll) ? '1' : '0'), \
(((i) & 0x40ll) ? '1' : '0'), \
(((i) & 0x20ll) ? '1' : '0'), \
(((i) & 0x10ll) ? '1' : '0'), \
(((i) & 0x08ll) ? '1' : '0'), \
(((i) & 0x04ll) ? '1' : '0'), \
(((i) & 0x02ll) ? '1' : '0'), \
(((i) & 0x01ll) ? '1' : '0')

#define PRINTF_BINARY_PATTERN_INT16 \
PRINTF_BINARY_PATTERN_INT8              PRINTF_BINARY_PATTERN_INT8
#define PRINTF_BYTE_TO_BINARY_INT16(i) \
PRINTF_BYTE_TO_BINARY_INT8((i) >> 8),   PRINTF_BYTE_TO_BINARY_INT8(i)
#define PRINTF_BINARY_PATTERN_INT32 \
PRINTF_BINARY_PATTERN_INT16             PRINTF_BINARY_PATTERN_INT16
#define PRINTF_BYTE_TO_BINARY_INT32(i) \
PRINTF_BYTE_TO_BINARY_INT16((i) >> 16), PRINTF_BYTE_TO_BINARY_INT16(i)
#define PRINTF_BINARY_PATTERN_INT64    \
PRINTF_BINARY_PATTERN_INT32             PRINTF_BINARY_PATTERN_INT32
#define PRINTF_BYTE_TO_BINARY_INT64(i) \
PRINTF_BYTE_TO_BINARY_INT32((i) >> 32), PRINTF_BYTE_TO_BINARY_INT32(i)

void
chip8_init(chip8_t *c8)
{
    memset(c8, 0, sizeof(*c8));
    c8->interpreter.PC = 0x200;
    memcpy(c8->interpreter.font, sprites, 80);
}

void
chip8_emulatecycle(chip8_t *c8)
{
    struct internals *c8_in = &(c8->interpreter);

    /* fetch opcode */
    c8_in->opcode =
        c8->RAM[c8_in->PC] << 8 | c8->RAM[(c8_in->PC) + 1];

    /* increment program counter */
    c8_in->PC += 2;

    /* decode opcode */
    /* HUGE switch MUST be moved to separate function */
    /* check leftmost 4 bits */
    switch(c8_in->opcode & 0xF000) {
        case 0x0000:
            switch (c8_in->opcode & 0x000F) {
                case 0x00E0:
                    memset(c8_in->disp_mem, 0, sizeof(c8_in->disp_mem));
                    c8_in->draw_flag = 1;
                    break;
                case 0x00EE:
                    c8_in->PC = c8_in->stack[c8_in->SP];
                    c8_in->SP -= 1;
                    break;
            }
            break;
        case 0x1000:
            c8_in->PC = c8_in->opcode & 0x0FFF;
            break;
        case 0x2000:
            c8_in->SP += 1;
            c8_in->stack[c8_in->SP] = c8_in->PC;
            c8_in->PC = c8_in->opcode & 0x0FFF;
            break;
        case 0x3000: {
            uint8_t x = (c8_in->opcode & 0x0F00) >> 8;
            if (c8_in->V[x] == (c8_in->opcode & 0x00FF)) {
                c8_in->PC += 2;
            }
            break;
        }
        case 0x4000: {
            uint8_t x = (c8_in->opcode & 0x0F00) >> 8;
            if (c8_in->V[x] != (c8_in->opcode & 0x00FF)) {
                c8_in->PC += 2;
            }
            break;
        }
        case 0x5000: {
            uint8_t x = (c8_in->opcode & 0x0F00) >> 8;
            uint8_t y = (c8_in->opcode & 0x00F0) >> 4;
            if (c8_in->V[x] == c8_in->V[y]) {
                c8_in->PC += 2;
            }
            break;
        }
        case 0x6000: {
            uint8_t x = (c8_in->opcode & 0x0F00) >> 8;
            c8_in->V[x] = c8_in->opcode & 0x00FF;
            break;
        }
        case 0x7000: {
            uint8_t x = (c8_in->opcode & 0x0F00) >> 8;
            c8_in->V[x] += c8_in->opcode & 0x00FF;
            break;
        }
        case 0x8000:
            switch (c8_in->opcode & 0x000F) {
                case 0x0000: {
                    uint8_t x = (c8_in->opcode & 0x0F00) >> 8;
                    uint8_t y = (c8_in->opcode & 0x00F0) >> 4;
                    c8_in->V[x] = c8_in->V[y];
                    break;
                }
                case 0x0001: {
                    uint8_t x = (c8_in->opcode & 0x0F00) >> 8;
                    uint8_t y = (c8_in->opcode & 0x00F0) >> 4;
                    c8_in->V[x] |= c8_in->V[y];
                    break;
                }
                case 0x0002: {
                    uint8_t x = (c8_in->opcode & 0x0F00) >> 8;
                    uint8_t y = (c8_in->opcode & 0x00F0) >> 4;
                    c8_in->V[x] &= c8_in->V[y];
                    break;
                }
                case 0x0003: {
                    uint8_t x = (c8_in->opcode & 0x0F00) >> 8;
                    uint8_t y = (c8_in->opcode & 0x00F0) >> 4;
                    c8_in->V[x] ^= c8_in->V[y];
                    break;
                }
                case 0x0004: {
                    uint8_t x = (c8_in->opcode & 0x0F00) >> 8;
                    uint8_t y = (c8_in->opcode & 0x00F0) >> 4;
                    uint16_t sum = c8_in->V[x] + c8_in->V[y];
                    if (sum > 0xFF) {
                        c8_in->V[0xF] = 1;
                    } else {
                        c8_in->V[0xF] = 0;
                    }
                    c8_in->V[x] = sum & 0x00FF;
                    break;
                }
                case 0x0005: {
                    uint8_t x = (c8_in->opcode & 0x0F00) >> 8;
                    uint8_t y = (c8_in->opcode & 0x00F0) >> 4;
                    if (c8_in->V[x] > c8_in->V[y]) {
                        c8_in->V[0xF] = 1;
                    } else {
                        c8_in->V[0xF] = 0;
                    }
                    c8_in->V[x] = c8_in->V[x] - c8_in->V[y];
                    break;
                }
                case 0x0006: {
                    uint8_t x = (c8_in->opcode & 0x0F00) >> 8;
                    c8_in->V[0xF] = (c8_in->V[x] & 0x01);
                    c8_in->V[x] >>= 1;
                    break;
                }
                case 0x0007: {
                    uint8_t x = (c8_in->opcode & 0x0F00) >> 8;
                    uint8_t y = (c8_in->opcode & 0x00F0) >> 4;
                    if (c8_in->V[y] > c8_in->V[x]) {
                        c8_in->V[0xF] = 1;
                    } else {
                        c8_in->V[0xF] = 0;
                    }
                    c8_in->V[x] = c8_in->V[y] - c8_in->V[x];
                    break;
                }
                case 0x000E: {
                    uint8_t x = (c8_in->opcode & 0x0F00) >> 8;
                    c8_in->V[0xF] = (c8_in->V[x] & 0x80) >> 7;
                    c8_in->V[x] <<= 1;
                    break;
                }
            }
            break;
        case 0x9000: {
            uint8_t x = (c8_in->opcode & 0x0F00) >> 8;
            uint8_t y = (c8_in->opcode & 0x00F0) >> 4;
            if (c8_in->V[x] != c8_in->V[y]) {
                c8_in->PC += 2;
            }
            break;
        }
        case 0xA000:
            c8_in->I = c8_in->opcode & 0x0FFF;
            break;
        case 0xB000:
            c8_in->PC = (c8_in->opcode & 0x0FFF) + c8_in->V[0];
            break;
        case 0xC000: {
            uint8_t x = (c8_in->opcode & 0x0F00) >> 8;
            c8_in->V[x] = (rand() % 256) & (c8_in->opcode & 0x00FF);
            break;
        }
        case 0xD000: {
            //fprintf(stdout, "\nDRAWING CASE\n");
            uint8_t x = (c8_in->opcode & 0x0F00) >> 8;
            uint8_t y = (c8_in->opcode & 0x00F0) >> 4;
            uint8_t height = (c8_in->opcode & 0x000F);

            uint8_t top = c8_in->V[y];
            uint8_t left = c8_in->V[x] + 8;
            uint64_t flag = 0;

            c8_in->V[0xF] = 0;

            for (int row = 0; row < height; row++) {
                uint64_t *disp_row = &(c8_in->disp_mem[(top + row) % 32]);
                /*fprintf(stdout, "raw_sprite_row"
                        BYTE_TO_BINARY_PATTERN,
                        BYTE_TO_BINARY(c8->RAM[c8_in->I + row]));*/
                uint64_t sprite_row = __builtin_rotateright64((uint64_t)c8->RAM[c8_in->I + row], left);
                flag |= *disp_row & sprite_row;
                *disp_row ^= sprite_row;
                /*fprintf(stdout, "disp_row "
                        PRINTF_BINARY_PATTERN_INT64 "\n",
                        PRINTF_BYTE_TO_BINARY_INT64(*disp_row));*/

                if (flag)
                    c8_in->V[0xF] = 1;
                /*fprintf(stdout, "sprite_row"
                PRINTF_BINARY_PATTERN_INT64 "\n",
                PRINTF_BYTE_TO_BINARY_INT64(sprite_row));*/

            }
            c8_in->draw_flag = 1;
            break;
        }

        case 0xE000:
            switch (c8_in->opcode & 0x00FF) {
                case 0x009E: {
                    uint8_t x = (c8_in->opcode & 0x0F00) >> 8;
                    uint8_t keynum = c8_in->V[x];
                    if (c8_in->keys[keynum]) {
                        c8_in->PC += 2;
                    }
                    break;
                }
                case 0x00A1: {
                    uint8_t x = (c8_in->opcode & 0x0F00) >> 8;
                    uint8_t keynum = c8_in->V[x];
                    if (!c8_in->keys[keynum]) {
                        c8_in->PC += 2;
                    }
                    break;
                }
            }
            break;
        case 0xF000:
            switch (c8_in->opcode & 0x00FF) {
                case 0x0007: {
                    uint8_t x = (c8_in->opcode & 0x0F00) >> 8;
                    c8_in->V[x] = c8_in->delay_timer;
                    break;
                }
                case 0x000A: {
                    uint8_t x = (c8_in->opcode & 0x0F00) >> 8;
                    if (c8_in->keys[0]) {
                        c8_in->V[x] = 0;
                    } else if (c8_in->keys[1]) {
                        c8_in->V[x] = 1;
                    } else if (c8_in->keys[2]) {
                        c8_in->V[x] = 2;
                    } else if (c8_in->keys[3]) {
                        c8_in->V[x] = 3;
                    } else if (c8_in->keys[4]) {
                        c8_in->V[x] = 4;
                    } else if (c8_in->keys[5]) {
                        c8_in->V[x] = 5;
                    } else if (c8_in->keys[6]) {
                        c8_in->V[x] = 6;
                    } else if (c8_in->keys[7]) {
                        c8_in->V[x] = 7;
                    } else if (c8_in->keys[8]) {
                        c8_in->V[x] = 8;
                    } else if (c8_in->keys[9]) {
                        c8_in->V[x] = 9;
                    } else if (c8_in->keys[10]) {
                        c8_in->V[x] = 10;
                    } else if (c8_in->keys[11]) {
                        c8_in->V[x] = 11;
                    } else if (c8_in->keys[12]) {
                        c8_in->V[x] = 12;
                    } else if (c8_in->keys[13]) {
                        c8_in->V[x] = 13;
                    } else if (c8_in->keys[14]) {
                        c8_in->V[x] = 14;
                    } else if (c8_in->keys[15]) {
                        c8_in->V[x] = 15;
                    }
                    break;
                }
                case 0x0015: {
                    uint8_t x = (c8_in->opcode & 0x0F00) >> 8;
                    c8_in->delay_timer = c8_in->V[x];
                    break;
                }
                case 0x0018: {
                    uint8_t x = (c8_in->opcode & 0x0F00) >> 8;
                    c8_in->sound_timer = c8_in->V[x];
                    break;
                }
                case 0x001E: {
                    uint8_t x = (c8_in->opcode & 0x0F00) >> 8;
                    c8_in->I += c8_in->V[x];
                    break;
                }
                case 0x0029: {
                    uint8_t x = (c8_in->opcode & 0x0F00) >> 8;
                    uint8_t digit = c8_in->V[x];
                    c8_in->I = (5 * digit); /* fonts starting address is RAM[0] */
                    break;
                }
                case 0x0033: {
                    uint8_t x = (c8_in->opcode & 0x0F00) >> 8;
                    uint8_t value = c8_in->V[x];
                    c8->RAM[c8_in->I] = value / 100;
                    c8->RAM[(c8_in->I) + 1] = value % 100 / 10;
                    c8->RAM[(c8_in->I) + 2] = value % 10;
                    break;
                }
                case 0x0055: {
                    uint8_t x = (c8_in->opcode & 0x0F00) >> 8;
                    for(uint8_t i = 0; i <= x; i++) {
                        c8->RAM[(c8_in->I) + i] = c8_in->V[i];
                    }
                    break;
                }
                case 0x0065: {
                    uint8_t x = (c8_in->opcode & 0x0F00) >> 8;
                    for (uint8_t i = 0; i <= x; i++) {
                        c8_in->V[i] = c8->RAM[(c8_in->I) + i];
                    }
                    break;
                }
            }
            break;
        default:
            fprintf(stderr, "Error: invalid opcode: 0x%X\n", c8_in->opcode);
            break;
    }
    /* update timers */
    if (c8_in->delay_timer > 0) {
        c8_in->delay_timer -= 1;
    }
    if (c8_in->sound_timer > 0) {
        c8_in->sound_timer -= 1;
    }
}

void
chip8_loadgame(chip8_t *c8, const char *game_name)
{
    FILE *game;
    game = fopen(game_name, "rb");
    if (NULL == game) {
        fprintf(stderr, "Error: failed to open game: %s.\n", game_name);
        exit(-1); /* TODO add proper exit code */
    }
    fread(&(c8->RAM[0x200]), 1, MAX_GAME_SIZE, game);
    for (int i = 0x200; i < 0x1000 - 1; i += 2) {
        fprintf(stdout, "0x %x%x\n", c8->RAM[i], c8->RAM[i + 1]);
    }
    fclose(game);
}

int
main(int argc, char **argv)
{
    if (argc < 2) {
        fprintf(stderr, "Error: No ROM selected!\n");
    }
    chip8_t *c8 = malloc(sizeof(*c8));
    if (NULL == c8) {
        fprintf(stderr, "Error: memory allocation failed!\n");
        exit(-1); /* TODO add proper exit code */
    }
    chip8_init(c8);
    chip8_loadgame(c8, argv[1]);
    sdl_layer_init(argv[1], DISP_WIDTH, DISP_HEIGHT, 10);

    int close_requested = 0;
    while (!close_requested) {
        SDL_Event event;
        SDL_PollEvent(&event);
        if (event.type == SDL_QUIT) {
            close_requested = 1;
        }
        chip8_emulatecycle(c8);
        if (c8->interpreter.draw_flag) {
            sdl_layer_draw(c8->interpreter.disp_mem, 2056);
            c8->interpreter.draw_flag = 0;
        }
    }
    sdl_layer_destroy();
    free(c8);
    return 0;
}
