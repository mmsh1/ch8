#ifndef CHIP_8
#define CHIP_8

#include <stdint.h>

#define RAM_SIZE 0x1000
#define PROGRAMM_START_OFFSET 0x200
#define MAX_GAME_SIZE (RAM_SIZE - PROGRAMM_START_OFFSET)

typedef union {
    uint8_t RAM[RAM_SIZE];
    struct internals {
        uint8_t V[16];              /* V[F] reserved for carry flag */
        uint8_t delay_timer;
        uint8_t sound_timer;
        uint8_t SP;                 /* stack pointer */
        uint8_t keys[16];
        uint8_t lres_font[16 * 5];  /* 16 sprites, 5 bytes per sprite */
        uint8_t hres_font[10 * 10]; /* 10 sprites, 10 bytes per sprite */

        uint8_t draw_flag;          /* to avoid redundant drawing */
        uint8_t extended_flag;      /* SCHIP mode */
        uint8_t exit_flag;
        uint8_t rpl_flags[8];

        uint16_t opcode;            /* current instruction */
        uint16_t PC;                /* program counter */
        uint16_t stack[16];
        uint16_t I;
    } core;

} chip8;

void chip8_init(chip8 *);

void chip8_emulatecycle(chip8 *);

int chip8_loadgame(chip8 *, const char *);

#endif /* CHIP_8 */
