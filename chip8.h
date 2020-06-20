#ifndef CHIP_8
#define CHIP_8

#include <stdint.h>

#define RAM_SIZE 0x1000
#define MAX_GAME_SIZE (0x1000 - 0x200)
enum {
    C8_DISP_WIDTH = 64,
    C8_DISP_HEIGHT = 32
};

typedef union {
    uint8_t RAM[RAM_SIZE];
    struct internals {
        uint8_t V[16];          /* V[F] reserved for carry flag */
        uint8_t delay_timer;
        uint8_t sound_timer;
        uint8_t SP;             /* stack pointer */
        uint8_t keys[16];
        uint8_t font[16 * 5];   /* 16 sprites, 5 bytes per sprite */
        uint8_t draw_flag;      /* to avoid redundant drawing */

        uint16_t opcode;        /* current instruction */
        uint16_t PC;            /* program counter */
        uint16_t stack[16];
        uint16_t I;

        uint64_t disp_mem[C8_DISP_HEIGHT];
    } interpreter;

} chip8_t;

void chip8_init(chip8_t *c8);

void chip8_emulatecycle(chip8_t *c8);

void chip8_loadgame(chip8_t *c8, const char *game_name);

#endif /* CHIP_8 */
