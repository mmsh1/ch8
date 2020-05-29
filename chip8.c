#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "chip8.h"

void
chip8_init(chip8_t *c8)
{
    c8->interpreter.PC = 0x200;
    c8->interpreter.opcode = 0;
    c8->interpreter.I = 0;
    c8->interpreter.SP = 0;

    /* clear display */
    /* clear stack */
    /* clear registers */
    /* clear memory */

    (void)memcpy(c8->RAM, &sprites, 80);

    /* Reset timers */
}

void
chip8_emulatecycle(chip8_t *c8)
{
    struct internals *c8_in = &(c8->interpreter);

    /* fetch opcode */
    c8_in->opcode =
        c8->RAM[c8_in->PC] << 8 | c8->RAM[(c8_in->PC) + 1];
    /* decode opcode */
    /* HUGE switch MUST be moved to separate function */
    /* check leftmost 4 bits */
    switch(c8_in->opcode & 0xF000) {
        case 0x0000:
            switch (c8_in->opcode & 0x000F) {
                case 0x00E0:
                    memset(c8_in->disp_mem, 0, DISP_WIDTH * DISP_HEIGHT / 8);
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
            c8_in->PC += 2;
            break;
        case 0xB000:
            c8_in->PC = (c8_in->opcode & 0x0FFF) + c8_in->V[0];
            break;
        case 0xC000: {
            uint8_t x = (c8_in->opcode & 0x0F00) >> 8;
            c8_in->V[x] = (rand() % 256) & (c8_in->opcode & 0x00FF);
            break;
        }
        /* case 0xD000: {
            uint8_t x = (c8_in->opcode & 0x0F00) >> 8;
            uint8_t y = (c8_in->opcode & 0x00F0) >> 4;
            uint8_t height = (c8_in->opcode & 0x000F); n from opcode

            uint8_t xpos = c8_in->V[x] % DISP_WIDTH;
            uint8_t ypos = c8_in->V[y] % DISP_HEIGHT;
            c8_in->V[0xF] = 0;

            for (uint32_t row = 0; row < height; row++) {
                uint8_t sprite_b = c8->RAM[c8_in->I + row];

                for (uint32_t col = 0; col < 8; col++);

            }
            break;
        } */
        case 0xE000:
            switch (c8_in->opcode & 0x00FF) {
                case 0x009E:
                    break;
                case 0x00A1:
                    break;
            }
            break;
        case 0xF000:
            break;
        default:
            fprintf(stderr, "Error: invalid opcode: 0x%X\n", c8_in->opcode);
            break;
    }
    /* update timers */
}

void
chip8_loadgame(chip8_t *c8, char *game_name)
{
    FILE *game;
    game = fopen(game_name, "rb");
    if (NULL == game) {
        fprintf(stderr, "Error: failed to open game: %s.\n", game_name);
        exit(-1); /* TODO add proper exit code */
    }
    fread(&(c8->RAM[0x200]), 1, MAX_GAME_SIZE, game);
    fclose(game);
}

int
main(void)
{
    chip8_t *ch8 = calloc(1, sizeof(chip8_t));
    chip8_init(ch8);
    free(ch8);
    return 0;
}
