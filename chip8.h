#ifndef CHIP_8
#define CHIP_8

#include <stdint.h>

#define RAM_SIZE 0x1000
#define MAX_GAME_SIZE (0x1000 - 0x200)
enum {
    DISP_WIDTH = 64,
    DISP_HEIGHT = 32
};

typedef union {
    uint8_t RAM[RAM_SIZE];
    struct internals {
        uint8_t V[16];          /* V[F] reserved for flag */
        uint8_t delay_timer;
        uint8_t sound_timer;
        uint8_t SP;             /* stack pointer */
        uint8_t keys[16];
        uint8_t disp_mem[DISP_WIDTH * DISP_HEIGHT / 8];
        uint8_t font[16 * 5];   /* 5 bytes per sprite */
        uint8_t draw_flag;      /* to avoid redundant drawing */

        uint16_t opcode;        /* current instruction */
        uint16_t PC;            /* program counter */
        uint16_t stack[16];
        uint16_t I;
    } interpreter;

} chip8_t;

uint8_t sprites[80] = {
    0xF0, 0x90, 0x90, 0x90, 0xF0,   /* 0 */
    0x20, 0x60, 0x20, 0x20, 0x70,   /* 1 */
    0xF0, 0x10, 0xF0, 0x80, 0xF0,   /* 2 */
    0xF0, 0x10, 0xF0, 0x10, 0xF0,   /* 3 */
    0x90, 0x90, 0xF0, 0x10, 0x10,   /* 4 */
    0xF0, 0x80, 0xF0, 0x10, 0xF0,   /* 5 */
    0xF0, 0x80, 0xF0, 0x90, 0xF0,   /* 6 */
    0xF0, 0x10, 0x20, 0x40, 0x40,   /* 7 */
    0xF0, 0x90, 0xF0, 0x90, 0xF0,   /* 8 */
    0xF0, 0x90, 0xF0, 0x10, 0xF0,   /* 9 */
    0xF0, 0x90, 0xF0, 0x90, 0x90,   /* A */
    0xE0, 0x90, 0xE0, 0x90, 0xE0,   /* B */
    0xF0, 0x80, 0x80, 0x80, 0xF0,   /* C */
    0xE0, 0x90, 0x90, 0x90, 0xE0,   /* D */
    0xF0, 0x80, 0xF0, 0x80, 0xF0,   /* E */
    0xF0, 0x80, 0xF0, 0x80, 0x80    /* F */

};

void chip8_init(chip8_t *c8);

void chip8_emulatecycle(chip8_t *c8);

void chip8_loadgame(chip8_t *c8, char *game);

#endif /* CHIP_8 */
