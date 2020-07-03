#include <stdlib.h>
#include <string.h>

#include "chip8.h"
#include "sdl_layer.h"

/*typedef void (*c8_opcode_func)(chip8_t *c8, struct internals *c8_in);

static void c8_NULL();
static void c8_goto_opcodes_0(chip8_t *c8, struct internals *c8_in);
static void c8_goto_opcodes_8(chip8_t *c8, struct internals *c8_in);
static void c8_goto_opcodes_E(chip8_t *c8, struct internals *c8_in);
static void c8_goto_opcodes_F(chip8_t *c8, struct internals *c8_in);*/

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

static uint64_t
_rotate_r64(uint64_t bitarr, uint8_t shr)
{
    return (bitarr >> shr) | (bitarr << (64 - shr));
}

/*static void
c8_00E0(chip8_t *c8, struct internals *c8_in)
{
    memset(c8_in->disp_mem, 0, sizeof(uint64_t) * C8_DISP_HEIGHT);
    c8_in->draw_flag = 1;
}

static void
c8_00EE(chip8_t *c8, struct internals *c8_in)
{
    c8_in->PC = c8_in->stack[c8_in->SP];
    c8_in->SP -= 1;
}

static void
c8_1nnn(chip8_t *c8, struct internals *c8_in)
{
    c8_in->PC = c8_in->opcode & 0x0FFF;
}

static void
c8_2nnn(chip8_t *c8, struct internals *c8_in)
{
    c8_in->SP += 1;
    c8_in->stack[c8_in->SP] = c8_in->PC;
    c8_in->PC = c8_in->opcode & 0x0FFF;
}

static void
c8_3xkk(chip8_t *c8, struct internals *c8_in)
{
    uint8_t x = (c8_in->opcode & 0x0F00) >> 8;
    if (c8_in->V[x] == (c8_in->opcode & 0x00FF)) {
        c8_in->PC += 2;
    }
}

static void
c8_4xkk(chip8_t *c8, struct internals *c8_in)
{
    uint8_t x = (c8_in->opcode & 0x0F00) >> 8;
    if (c8_in->V[x] != (c8_in->opcode & 0x00FF)) {
        c8_in->PC += 2;
    }
}

static void
c8_5xy0(chip8_t *c8, struct internals *c8_in)
{
    uint8_t x = (c8_in->opcode & 0x0F00) >> 8;
    uint8_t y = (c8_in->opcode & 0x00F0) >> 4;
    if (c8_in->V[x] == c8_in->V[y]) {
        c8_in->PC += 2;
    }
}

static void
c8_6xkk(chip8_t *c8, struct internals *c8_in)
{
    uint8_t x = (c8_in->opcode & 0x0F00) >> 8;
    c8_in->V[x] = c8_in->opcode & 0x00FF;
}

static void
c8_7xkk(chip8_t *c8, struct internals *c8_in)
{
    uint8_t x = (c8_in->opcode & 0x0F00) >> 8;
    c8_in->V[x] += c8_in->opcode & 0x00FF;
}

static void
c8_8xy0(chip8_t *c8, struct internals *c8_in)
{
    uint8_t x = (c8_in->opcode & 0x0F00) >> 8;
    uint8_t y = (c8_in->opcode & 0x00F0) >> 4;
    c8_in->V[x] = c8_in->V[y];
}

static void
c8_8xy1(chip8_t *c8, struct internals *c8_in)
{
    uint8_t x = (c8_in->opcode & 0x0F00) >> 8;
    uint8_t y = (c8_in->opcode & 0x00F0) >> 4;
    c8_in->V[x] |= c8_in->V[y];
}

static void
c8_8xy2(chip8_t *c8, struct internals *c8_in)
{
    uint8_t x = (c8_in->opcode & 0x0F00) >> 8;
    uint8_t y = (c8_in->opcode & 0x00F0) >> 4;
    c8_in->V[x] &= c8_in->V[y];
}

static void
c8_8xy3(chip8_t *c8, struct internals *c8_in)
{
    uint8_t x = (c8_in->opcode & 0x0F00) >> 8;
    uint8_t y = (c8_in->opcode & 0x00F0) >> 4;
    c8_in->V[x] ^= c8_in->V[y];
}

static void
c8_8xy4(chip8_t *c8, struct internals *c8_in)
{
    uint8_t x = (c8_in->opcode & 0x0F00) >> 8;
    uint8_t y = (c8_in->opcode & 0x00F0) >> 4;
    uint16_t sum = c8_in->V[x] + c8_in->V[y];
    if (sum > 0xFF) {
        c8_in->V[0xF] = 1;
    } else {
        c8_in->V[0xF] = 0;
    }
    c8_in->V[x] = sum & 0x00FF;
}

static void
c8_8xy5(chip8_t *c8, struct internals *c8_in)
{
    uint8_t x = (c8_in->opcode & 0x0F00) >> 8;
    uint8_t y = (c8_in->opcode & 0x00F0) >> 4;
    if (c8_in->V[x] > c8_in->V[y]) {
        c8_in->V[0xF] = 1;
    } else {
        c8_in->V[0xF] = 0;
    }
    c8_in->V[x] = c8_in->V[x] - c8_in->V[y];
}

static void
c8_8xy6(chip8_t *c8, struct internals *c8_in)
{
    uint8_t x = (c8_in->opcode & 0x0F00) >> 8;
    c8_in->V[0xF] = (c8_in->V[x] & 0x01);
    c8_in->V[x] >>= 1;
}

static void
c8_8xy7(chip8_t *c8, struct internals *c8_in)
{
    uint8_t x = (c8_in->opcode & 0x0F00) >> 8;
    uint8_t y = (c8_in->opcode & 0x00F0) >> 4;
    if (c8_in->V[y] > c8_in->V[x]) {
        c8_in->V[0xF] = 1;
    } else {
        c8_in->V[0xF] = 0;
    }
    c8_in->V[x] = c8_in->V[y] - c8_in->V[x];
}

static void
c8_8xyE(chip8_t *c8, struct internals *c8_in)
{
    uint8_t x = (c8_in->opcode & 0x0F00) >> 8;
    c8_in->V[0xF] = (c8_in->V[x] & 0x80) >> 7;
    c8_in->V[x] <<= 1;
}

static void
c8_9xy0(chip8_t *c8, struct internals *c8_in)
{
    uint8_t x = (c8_in->opcode & 0x0F00) >> 8;
    uint8_t y = (c8_in->opcode & 0x00F0) >> 4;
    if (c8_in->V[x] != c8_in->V[y]) {
        c8_in->PC += 2;
    }
}

static void
c8_Annn(chip8_t *c8, struct internals *c8_in)
{
    c8_in->I = c8_in->opcode & 0x0FFF;
}

static void
c8_Bnnn(chip8_t *c8, struct internals *c8_in)
{
    c8_in->PC = (c8_in->opcode & 0x0FFF) + c8_in->V[0];
}

static void
c8_Cxkk(chip8_t *c8, struct internals *c8_in)
{
    uint8_t x = (c8_in->opcode & 0x0F00) >> 8;
    c8_in->V[x] = (rand() % 256) & (c8_in->opcode & 0x00FF);
}

static void
c8_Dxyn(chip8_t *c8, struct internals *c8_in)
{
    uint8_t x = (c8_in->opcode & 0x0F00) >> 8;
    uint8_t y = (c8_in->opcode & 0x00F0) >> 4;
    uint8_t height = (c8_in->opcode & 0x000F);

    uint8_t ypos = c8_in->V[y];
    uint8_t xpos = c8_in->V[x] + 8;
    uint64_t flag = 0;

    c8_in->V[0xF] = 0;

    for (int row = 0; row < height; row++) {
        uint64_t *disp_row = &(c8_in->disp_mem[(ypos + row) % 32]);
        uint64_t sprite_row = _rotate_r64((uint64_t)c8->RAM[c8_in->I + row], xpos);
        flag |= *disp_row & sprite_row;
        *disp_row ^= sprite_row;

        if (flag) {
            c8_in->V[0xF] = 1;
        }
    }
    c8_in->draw_flag = 1;
}

static void
c8_Ex9E(chip8_t *c8, struct internals *c8_in)
{
    uint8_t x = (c8_in->opcode & 0x0F00) >> 8;
    uint8_t keynum = c8_in->V[x];
    if (c8_in->keys[keynum]) {
        c8_in->PC += 2;
    }
}

static void
c8_ExA1(chip8_t *c8, struct internals *c8_in)
{
    uint8_t x = (c8_in->opcode & 0x0F00) >> 8;
    uint8_t keynum = c8_in->V[x];
    if (!c8_in->keys[keynum]) {
        c8_in->PC += 2;
    }
}

static void
c8_Fx07(chip8_t *c8, struct internals *c8_in)
{
    fprintf(stderr, "c8_Fx07\n");
    uint8_t x = (c8_in->opcode & 0x0F00) >> 8;
    c8_in->V[x] = c8_in->delay_timer;
}

static void
c8_Fx0A(chip8_t *c8, struct internals *c8_in)
{
    fprintf(stderr, "c8_Fx0A\n");
    uint8_t x = (c8_in->opcode & 0x0F00) >> 8;
    uint8_t no_key_pressed = 1;
    for (uint8_t i = 0; i < 16; i++) {
        if (c8_in->keys[i]) {
            c8_in->V[x] = i;
            no_key_pressed = 0;
            break;
        }
    }
    if (no_key_pressed) {
        c8_in->PC -= 2;
    }
}

static void
c8_Fx15(chip8_t *c8, struct internals *c8_in)
{
    fprintf(stderr, "c8_Fx15\n");
    uint8_t x = (c8_in->opcode & 0x0F00) >> 8;
    c8_in->delay_timer = c8_in->V[x];
}

static void
c8_Fx18(chip8_t *c8, struct internals *c8_in)
{
    fprintf(stderr, "c8_Fx18\n");
    uint8_t x = (c8_in->opcode & 0x0F00) >> 8;
    c8_in->sound_timer = c8_in->V[x];
}

static void
c8_Fx1E(chip8_t *c8, struct internals *c8_in)
{
    fprintf(stderr, "c8_Fx1E\n");
    uint8_t x = (c8_in->opcode & 0x0F00) >> 8;
    c8_in->I += c8_in->V[x];
}

static void
c8_Fx29(chip8_t *c8, struct internals *c8_in)
{
    fprintf(stderr, "c8_Fx29\n");
    uint8_t x = (c8_in->opcode & 0x0F00) >> 8;
    uint8_t digit = c8_in->V[x];
    uint16_t fontset_address = &(c8_in->font[0]) - c8->RAM;
    c8_in->I = fontset_address + digit * 5;
}

static void
c8_Fx33(chip8_t *c8, struct internals *c8_in)
{
    fprintf(stderr, "c8_Fx33\n");
    uint8_t x = (c8_in->opcode & 0x0F00) >> 8;
    uint8_t value = c8_in->V[x];
    c8->RAM[c8_in->I] = value / 100;
    c8->RAM[(c8_in->I) + 1] = value % 100 / 10;
    c8->RAM[(c8_in->I) + 2] = value % 10;
}

static void
c8_Fx55(chip8_t *c8, struct internals *c8_in)
{
    fprintf(stderr, "c8_Fx55\n");
    uint8_t x = (c8_in->opcode & 0x0F00) >> 8;
    for(uint8_t i = 0; i <= x; i++) {
        c8->RAM[(c8_in->I) + i] = c8_in->V[i];
    }
}

static void
c8_Fx65(chip8_t *c8, struct internals *c8_in)
{
   fprintf(stderr, "c8_Fx65\n");
   uint8_t x = (c8_in->opcode & 0x0F00) >> 8;
    for (uint8_t i = 0; i <= x; i++) {
        c8_in->V[i] = c8->RAM[(c8_in->I) + i];
    }
}

c8_opcode_func opcodes_main[0xF + 1] = {
    c8_goto_opcodes_0, c8_1nnn, c8_2nnn, c8_3xkk, c8_4xkk, c8_5xy0, c8_6xkk,
    c8_7xkk, c8_goto_opcodes_8, c8_9xy0, c8_Annn, c8_Bnnn, c8_Cxkk, c8_Dxyn,
    c8_goto_opcodes_E, c8_goto_opcodes_F
};

c8_opcode_func opcodes_0[0xE + 1] = {
    c8_00E0, c8_NULL, c8_NULL, c8_NULL, c8_NULL,
    c8_NULL, c8_NULL, c8_NULL, c8_NULL, c8_NULL,
    c8_NULL, c8_NULL, c8_NULL, c8_NULL, c8_00EE
};

c8_opcode_func opcodes_8[0xE + 1] = {
    c8_8xy0, c8_8xy1, c8_8xy2, c8_8xy3, c8_8xy4,
    c8_8xy5, c8_8xy6, c8_8xy7, c8_NULL, c8_NULL,
    c8_NULL, c8_NULL, c8_NULL, c8_NULL, c8_8xyE
};

c8_opcode_func opcodes_E[0xA1 + 1];
c8_opcode_func opcodes_F[0x65 + 1];

static void
c8_NULL(chip8_t *c8, struct internals *c8_in)
{
    fprintf(stderr, "NULL case!\n");
    fprintf(stderr, "opcode: %x\n", c8_in->opcode & 0xFFFF);
}

static void
c8_goto_opcodes_0(chip8_t *c8, struct internals *c8_in)
{
    opcodes_0[c8_in->opcode & 0x000F](c8, c8_in);
}

static void
c8_goto_opcodes_8(chip8_t *c8, struct internals *c8_in)
{
    opcodes_8[(c8_in->opcode) & 0x000F](c8, c8_in);
}

static void
c8_goto_opcodes_E(chip8_t *c8, struct internals *c8_in)
{
    opcodes_E[c8_in->opcode & 0x00FF](c8, c8_in);
}

static void
c8_goto_opcodes_F(chip8_t *c8, struct internals *c8_in)
{
    fprintf(stderr, "F-opcode: %x\n", c8_in->opcode & 0x00FF);
    opcodes_F[c8_in->opcode & 0x00FF](c8, c8_in);
}

static void
init_opcodes_E()
{
    for (int i = 0; i < 0xA1 + 1; i++) {
        opcodes_E[i] = c8_NULL;
    }
    opcodes_E[0x9E] = c8_Ex9E;
    opcodes_E[0xA1] = c8_ExA1;
}

static void
init_opcodes_F()
{
    for (int i = 0; i < 0x65 + 1; i++) {
        opcodes_F[i] = c8_NULL;
    }
    opcodes_F[0x07] = c8_Fx07;
    opcodes_F[0x0A] = c8_Fx0A;
    opcodes_F[0x15] = c8_Fx15;
    opcodes_F[0x18] = c8_Fx18;
    opcodes_F[0x1E] = c8_Fx1E;
    opcodes_F[0x29] = c8_Fx29;
    opcodes_F[0x33] = c8_Fx33;
    opcodes_F[0x55] = c8_Fx55;
    opcodes_F[0x65] = c8_Fx65;
}*/

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
    /* (OBSOLETE) HUGE switch MUST be moved to separate function */
    /* TODO replace with function pointer table */

    //opcodes_main[(c8_in->opcode & 0xF000) >> 12](c8, c8_in);

    /* check leftmost 4 bits */
    switch(c8_in->opcode & 0xF000) {
        case 0x0000:
            switch (c8_in->opcode & 0x000F) {
                case 0x0000:
                    memset(c8_in->disp_mem, 0, sizeof(uint64_t) * C8_DISP_HEIGHT);
                    c8_in->draw_flag = 1;
                    break;
                case 0x000E:
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
                default: {
                    fprintf(stderr, "%x\n", c8_in->opcode & 0xFFFF);
                    fprintf(stderr, "unknown 8-opcode encountered\n");}
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
            uint8_t x = (c8_in->opcode & 0x0F00) >> 8;
            uint8_t y = (c8_in->opcode & 0x00F0) >> 4;
            uint8_t height = (c8_in->opcode & 0x000F);

            uint8_t ypos = c8_in->V[y];
            uint8_t xpos = c8_in->V[x] + 8;
            uint64_t flag = 0;

            c8_in->V[0xF] = 0;

            for (int row = 0; row < height; row++) {
                uint64_t *disp_row = &(c8_in->disp_mem[(ypos + row) % 32]);
                uint64_t sprite_row = _rotate_r64((uint64_t)c8->RAM[c8_in->I + row], xpos);
                flag |= *disp_row & sprite_row;
                *disp_row ^= sprite_row;

                if (flag) {
                    c8_in->V[0xF] = 1;
                }
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
                default:
                    fprintf(stderr, "%x\n", c8_in->opcode & 0xFFFF);
                    fprintf(stderr, "unknown E-opcode encountered\n");}
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
                    uint8_t no_key_pressed = 1;
                    for (uint8_t i = 0; i < 16; i++) {
                        if (c8_in->keys[i]) {
                        c8_in->V[x] = i;
                        no_key_pressed = 0;
                        break;
                        }
                    }
                    if (no_key_pressed) {
                        c8_in->PC -= 2;
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
                    uint16_t fontset_address = &(c8_in->font[0]) - c8->RAM;
                    c8_in->I = fontset_address + digit * 5;
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
                default:
                    fprintf(stderr, "%x\n", c8_in->opcode & 0xFFFF);
                    fprintf(stderr, "unknown F-opcode encountered\n");
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

int
chip8_loadgame(chip8_t *c8, const char *game_name)
{
    FILE *game;
    game = fopen(game_name, "rb");
    if (game == NULL) {
        fprintf(stderr, "Error: failed to open game: %s.\n", game_name);
        return -1;
    }
    fread(&(c8->RAM[0x200]), 1, MAX_GAME_SIZE, game);
    /*for (int i = 0x200; i < 0x1000 - 1; i += 2)
        fprintf(stdout, "%X\n",
                c8->RAM[i] << 8 | c8->RAM[i+1]);
    */
    fclose(game);
    return 0;
}

int
main(int argc, char **argv)
{
    if (argc != 3) {
        fprintf(stderr, "Error: wrong arguments!\n");
        fprintf(stderr, "Usage: %s delay ROM\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    /*init_opcodes_E();
    init_opcodes_F();*/

    chip8_t *c8 = NULL;
    uint8_t quit_flag = 0;

    c8 = malloc(sizeof(*c8));
    if (c8 == NULL) {
        fprintf(stderr, "Error: memory allocation failed!\n");
        exit(EXIT_FAILURE);
    }

    chip8_init(c8);
    if (chip8_loadgame(c8, argv[2]) == -1) {
        fprintf(stderr, "Error: game not loaded!\n");
        exit(EXIT_FAILURE);
    }

    if (sdl_layer_init(argv[2], C8_DISP_WIDTH, C8_DISP_HEIGHT, 10) == -1) {
        fprintf(stderr, "Error: sdl_layer creation failed!\n");
        exit(EXIT_FAILURE);
    }

    while (!quit_flag) {
        sdl_handle_keystroke(c8->interpreter.keys, &quit_flag);
        chip8_emulatecycle(c8);
        if (c8->interpreter.draw_flag) {
            uint32_t output[C8_DISP_WIDTH * C8_DISP_HEIGHT];
            sdl_layer_draw(c8->interpreter.disp_mem,
                           output,
                           C8_DISP_WIDTH * C8_DISP_HEIGHT);
            c8->interpreter.draw_flag = 0;
        }
    }
    sdl_layer_destroy(); free(c8);
    return EXIT_SUCCESS;
}
