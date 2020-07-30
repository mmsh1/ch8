#include <stdlib.h>
#include <stdio.h> /* fclose, fopen, fread, fprintf */
#include <string.h> /* memcpy, memset */

#include "chip8.h"
#include "sdl_layer.h"

#define BYTE_TO_BINARY_PATTERN "%c%c%c%c%c%c%c%c"
#define BYTE_TO_BINARY(byte)  \
  (byte & 0x80 ? '1' : '0'), \
  (byte & 0x40 ? '1' : '0'), \
  (byte & 0x20 ? '1' : '0'), \
  (byte & 0x10 ? '1' : '0'), \
  (byte & 0x08 ? '1' : '0'), \
  (byte & 0x04 ? '1' : '0'), \
  (byte & 0x02 ? '1' : '0'), \
  (byte & 0x01 ? '1' : '0')

enum {
    C8_DISP_WIDTH = 64,
    C8_DISP_HEIGHT = 32,

    SC8_DISP_WIDTH = 128,
    SC8_DISP_HEIGHT = 64
};

typedef void (*c8_opcode_func)(chip8 *);

static void c8_00E0(chip8 *);
static void c8_00EE(chip8 *);
/*TODO*/
static void c8_00Cx(chip8 *); /* SCD nibble: scroll screen x lines down */
static void c8_00FB(chip8 *); /* SCL: scroll screen 4 pix left */
static void c8_00FC(chip8 *); /* SCR: scroll screen 4 pix right */
/* TODOend*/
static void c8_00FD(chip8 *);
static void c8_00FE(chip8 *);
static void c8_00FF(chip8 *);

static void c8_1nnn(chip8 *);
static void c8_2nnn(chip8 *);
static void c8_3xkk(chip8 *);
static void c8_4xkk(chip8 *);
static void c8_5xy0(chip8 *);
static void c8_6xkk(chip8 *);
static void c8_7xkk(chip8 *);

static void c8_8xy0(chip8 *);
static void c8_8xy1(chip8 *);
static void c8_8xy2(chip8 *);
static void c8_8xy3(chip8 *);
static void c8_8xy4(chip8 *);
static void c8_8xy5(chip8 *);
static void c8_8xy6(chip8 *);
static void c8_8xy7(chip8 *);
static void c8_8xyE(chip8 *);

static void c8_9xy0(chip8 *);
static void c8_Annn(chip8 *);
static void c8_Bnnn(chip8 *);
static void c8_Cxkk(chip8 *);
static void c8_Dxyn(chip8 *);

static void c8_Ex9E(chip8 *);
static void c8_ExA1(chip8 *);

static void c8_Fx07(chip8 *);
static void c8_Fx0A(chip8 *);
static void c8_Fx15(chip8 *);
static void c8_Fx18(chip8 *);
static void c8_Fx1E(chip8 *);
static void c8_Fx29(chip8 *);
static void c8_Fx33(chip8 *);
static void c8_Fx55(chip8 *);
static void c8_Fx65(chip8 *);
static void c8_Fx30(chip8 *);
static void c8_Fx75(chip8 *);
static void c8_Fx85(chip8 *);

static void c8_NULL(chip8 *);
static void c8_goto_optable_0(chip8 *);
static void c8_goto_optable_8(chip8 *);
static void c8_goto_optable_E(chip8 *);
static void c8_goto_optable_F(chip8 *);

static void init_optable_main();
static void init_optable_0();
static void init_optable_8();
static void init_optable_E();
static void init_optable_F();

static c8_opcode_func optable_main[0xF + 1];
static c8_opcode_func optable_0[0xFF + 1];
static c8_opcode_func optable_8[0x0E + 1];
static c8_opcode_func optable_E[0xA1 + 1];
static c8_opcode_func optable_F[0x85 + 1];

uint8_t disp_mem[SC8_DISP_WIDTH * SC8_DISP_HEIGHT / 8];

static uint8_t lres_sprites[80] = {
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

static uint8_t hres_sprites[100] = {
    0xFF, 0xFF, 0xC3, 0xC3, 0xC3, 0xC3, 0xC3, 0xC3, 0xFF, 0xFF,   /* 0 */
    0x18, 0x78, 0x78, 0x18, 0x18, 0x18, 0x18, 0x18, 0xFF, 0xFF,   /* 1 */
    0xFF, 0xFF, 0x03, 0x03, 0xFF, 0xFF, 0xC0, 0xC0, 0xFF, 0xFF,   /* 2 */
    0xFF, 0xFF, 0x03, 0x03, 0xFF, 0xFF, 0x03, 0x03, 0xFF, 0xFF,   /* 3 */
    0xC3, 0xC3, 0xC3, 0xC3, 0xFF, 0xFF, 0x03, 0x03, 0x03, 0x03,   /* 4 */
    0xFF, 0xFF, 0xC0, 0xC0, 0xFF, 0xFF, 0x03, 0x03, 0xFF, 0xFF,   /* 5 */
    0xFF, 0xFF, 0xC0, 0xC0, 0xFF, 0xFF, 0xC3, 0xC3, 0xFF, 0xFF,   /* 6 */
    0xFF, 0xFF, 0x03, 0x03, 0x06, 0x0C, 0x18, 0x18, 0x18, 0x18,   /* 7 */
    0xFF, 0xFF, 0xC3, 0xC3, 0xFF, 0xFF, 0xC3, 0xC3, 0xFF, 0xFF,   /* 8 */
    0xFF, 0xFF, 0xC3, 0xC3, 0xFF, 0xFF, 0x03, 0x03, 0xFF, 0xFF,   /* 9 */
};

static void
_render_output(uint8_t *disp_mem, uint32_t *output, uint8_t ext_flag)
{
    if (ext_flag) {
        for(int i = 0; i < 128 * 64; i++) {
            output[i] = 0xFFFFFFFF * ((disp_mem[i / 8] >> (7 - i % 8)) & 1);
        }
    } else {
        /* TODO */
        for(int i = 0; i < 64 * 32; i++) {
            output[i] = 0xFFFFFFFF * ((disp_mem[i / 8] >> (7 - i % 8)) & 1);
        }
    }
}

static void
c8_00E0(chip8 *c8)
{
    memset(disp_mem, 0, sizeof(uint8_t)* (SC8_DISP_WIDTH * SC8_DISP_HEIGHT / 8));
    c8->core.draw_flag = 1;
}

static void
c8_00EE(chip8 *c8)
{
    c8->core.PC = c8->core.stack[c8->core.SP];
    c8->core.SP -= 1;
}

/*TODO*/
static void
c8_00Cx(chip8 *c8)
{
    /*uint8_t x = c8->core.opcode & 0x000F;
    uint8_t actual_height = 32;
    if (c8->core.extended_flag) {
        actual_height = 64;
    }
    for (int i = 0; i < x; i++) {
    }
    c8->core.draw_flag = 1;*/
}

static void
c8_00FB(chip8 *c8)
{
    c8->core.draw_flag = 1;
}

static void
c8_00FC(chip8 *c8)
{
    c8->core.draw_flag = 1;
}
/* TODOend */

static void
c8_00FD(chip8 *c8)
{
    c8->core.exit_flag = 1;
}

static void
c8_00FE(chip8 *c8)
{
    c8->core.extended_flag = 0;
}

static void
c8_00FF(chip8 *c8)
{
    c8->core.extended_flag = 1;
}

static void
c8_1nnn(chip8 *c8)
{
    c8->core.PC = c8->core.opcode & 0x0FFF;
}

static void
c8_2nnn(chip8 *c8)
{
    c8->core.SP += 1;
    c8->core.stack[c8->core.SP] = c8->core.PC;
    c8->core.PC = c8->core.opcode & 0x0FFF;
}

static void
c8_3xkk(chip8 *c8)
{
    uint8_t x = (c8->core.opcode & 0x0F00) >> 8;
    if (c8->core.V[x] == (c8->core.opcode & 0x00FF)) {
        c8->core.PC += 2;
    }
}

static void
c8_4xkk(chip8 *c8)
{
    uint8_t x = (c8->core.opcode & 0x0F00) >> 8;
    if (c8->core.V[x] != (c8->core.opcode & 0x00FF)) {
        c8->core.PC += 2;
    }
}

static void
c8_5xy0(chip8 *c8)
{
    uint8_t x = (c8->core.opcode & 0x0F00) >> 8;
    uint8_t y = (c8->core.opcode & 0x00F0) >> 4;
    if (c8->core.V[x] == c8->core.V[y]) {
        c8->core.PC += 2;
    }
}

static void
c8_6xkk(chip8 *c8)
{
    uint8_t x = (c8->core.opcode & 0x0F00) >> 8;
    c8->core.V[x] = c8->core.opcode & 0x00FF;
}

static void
c8_7xkk(chip8 *c8)
{
    uint8_t x = (c8->core.opcode & 0x0F00) >> 8;
    c8->core.V[x] += c8->core.opcode & 0x00FF;
}

static void
c8_8xy0(chip8 *c8)
{
    uint8_t x = (c8->core.opcode & 0x0F00) >> 8;
    uint8_t y = (c8->core.opcode & 0x00F0) >> 4;
    c8->core.V[x] = c8->core.V[y];
}

static void
c8_8xy1(chip8 *c8)
{
    uint8_t x = (c8->core.opcode & 0x0F00) >> 8;
    uint8_t y = (c8->core.opcode & 0x00F0) >> 4;
    c8->core.V[x] |= c8->core.V[y];
}

static void
c8_8xy2(chip8 *c8)
{
    uint8_t x = (c8->core.opcode & 0x0F00) >> 8;
    uint8_t y = (c8->core.opcode & 0x00F0) >> 4;
    c8->core.V[x] &= c8->core.V[y];
}

static void
c8_8xy3(chip8 *c8)
{
    uint8_t x = (c8->core.opcode & 0x0F00) >> 8;
    uint8_t y = (c8->core.opcode & 0x00F0) >> 4;
    c8->core.V[x] ^= c8->core.V[y];
}

static void
c8_8xy4(chip8 *c8)
{
    uint8_t x = (c8->core.opcode & 0x0F00) >> 8;
    uint8_t y = (c8->core.opcode & 0x00F0) >> 4;
    uint16_t sum = c8->core.V[x] + c8->core.V[y];
    if (sum > 0xFF) {
        c8->core.V[0xF] = 1;
    } else {
        c8->core.V[0xF] = 0;
    }
    c8->core.V[x] = sum & 0x00FF;
}

static void
c8_8xy5(chip8 *c8)
{
    uint8_t x = (c8->core.opcode & 0x0F00) >> 8;
    uint8_t y = (c8->core.opcode & 0x00F0) >> 4;
    if (c8->core.V[x] > c8->core.V[y]) {
        c8->core.V[0xF] = 1;
    } else {
        c8->core.V[0xF] = 0;
    }
    c8->core.V[x] = c8->core.V[x] - c8->core.V[y];
}

static void
c8_8xy6(chip8 *c8)
{
    uint8_t x = (c8->core.opcode & 0x0F00) >> 8;
    c8->core.V[0xF] = (c8->core.V[x] & 0x01);
    c8->core.V[x] >>= 1;
}

static void
c8_8xy7(chip8 *c8)
{
    uint8_t x = (c8->core.opcode & 0x0F00) >> 8;
    uint8_t y = (c8->core.opcode & 0x00F0) >> 4;
    if (c8->core.V[y] > c8->core.V[x]) {
        c8->core.V[0xF] = 1;
    } else {
        c8->core.V[0xF] = 0;
    }
    c8->core.V[x] = c8->core.V[y] - c8->core.V[x];
}

static void
c8_8xyE(chip8 *c8)
{
    uint8_t x = (c8->core.opcode & 0x0F00) >> 8;
    c8->core.V[0xF] = (c8->core.V[x] & 0x80) >> 7;
    c8->core.V[x] <<= 1;
}

static void
c8_9xy0(chip8 *c8)
{
    uint8_t x = (c8->core.opcode & 0x0F00) >> 8;
    uint8_t y = (c8->core.opcode & 0x00F0) >> 4;
    if (c8->core.V[x] != c8->core.V[y]) {
        c8->core.PC += 2;
    }
}

static void
c8_Annn(chip8 *c8)
{
    c8->core.I = c8->core.opcode & 0x0FFF;
}

static void
c8_Bnnn(chip8 *c8)
{
    c8->core.PC = (c8->core.opcode & 0x0FFF) + c8->core.V[0];
}

static void
c8_Cxkk(chip8 *c8)
{
    uint8_t x = (c8->core.opcode & 0x0F00) >> 8;
    c8->core.V[x] = (rand() % 256) & (c8->core.opcode & 0x00FF);
}

static void
c8_Dxyn(chip8 *c8)
{
    uint8_t x = (c8->core.opcode & 0x0F00) >> 8;
    uint8_t y = (c8->core.opcode & 0x00F0) >> 4;
    uint8_t height = (c8->core.opcode & 0x000F);

    uint8_t disp_width, mask_width;
    uint8_t disp_height, mask_height;

    uint8_t ypos;
    uint8_t xpos;

    c8->core.V[0xF] = 0;

    if (c8->core.extended_flag) {
        disp_width = 128;
        disp_height = 64;
        mask_width = 127;
        mask_height = 63;
    } else {
        disp_width = 64;
        disp_height = 32;
        mask_width = 63;
        mask_height = 31;
    }

    /* FIXME sprite bug */
    if (height != 0) {
        for (uint8_t sprite_row = 0; sprite_row < height; sprite_row++) {
            for (uint8_t sprite_col = 0; sprite_col < 8; sprite_col++) {
                uint8_t pix = ((c8->RAM[c8->core.I + sprite_row] & (0x80 >> sprite_col)) != 0);
                if (pix) {
                    xpos = (c8->core.V[x] + sprite_col) & mask_width;
                    ypos = (c8->core.V[y] + sprite_row) & mask_height;

                    int bitpos_global = ypos * disp_width + xpos;
                    uint8_t bitpos_internal = (uint8_t)1 << (bitpos_global & 0x07);
                    int bytepos = bitpos_global >> 3;
                    fprintf(stderr, "bitpos_global: %d\nbitpos_inter: %d\nbytepos: %d\n",
                            bitpos_global, bitpos_internal, bytepos);

                    if (disp_mem[bytepos] & bitpos_internal) {
                        c8->core.V[0xF] = 1;
                    }
                    disp_mem[bytepos] ^= bitpos_internal;
                }
            }
        }
    } else {
        /* if height (nibble) equals 0 we draw 16*16 sprite */
        /*for (uint8_t sprite_row = 0; sprite_row < height; sprite_row++) {
            for (uint8_t sprite_col = 0; sprite_col < 16; sprite_col++) {
                uint8_t pix;
                if (sprite_col > 7) {
                    pix = ((c8->RAM[c8->core.I + sprite_row * 2 + 1] & (0x80 >> (sprite_col & 0x07))) != 0);
                } else {
                    pix = ((c8->RAM[c8->core.I + sprite_row * 2] & (0x80 >> sprite_col)) != 0);
                }
                if (pix) {
                    xpos = (c8->core.V[x] + sprite_col) & mask_width;
                    ypos = (c8->core.V[y] + sprite_row) & mask_height;
                    int bitpos_global = ypos * disp_width + xpos;
                    int bitpos_internal = 1 << (bitpos_global & 0x07);
                    int bytepos = bitpos_global >> 3;
                    if (disp_mem[bytepos] & bitpos_internal) {
                        c8->core.V[0xF] = 1;
                    }
                    disp_mem[bytepos] ^= bitpos_internal;}
            }
        }*/
    }
    c8->core.draw_flag = 1;
}

static void
c8_Ex9E(chip8 *c8)
{
    uint8_t x = (c8->core.opcode & 0x0F00) >> 8;
    uint8_t keynum = c8->core.V[x];
    if (c8->core.keys[keynum]) {
        c8->core.PC += 2;
    }
}

static void
c8_ExA1(chip8 *c8)
{
    uint8_t x = (c8->core.opcode & 0x0F00) >> 8;
    uint8_t keynum = c8->core.V[x];
    if (!c8->core.keys[keynum]) {
        c8->core.PC += 2;
    }
}

static void
c8_Fx07(chip8 *c8)
{
    uint8_t x = (c8->core.opcode & 0x0F00) >> 8;
    c8->core.V[x] = c8->core.delay_timer;
}

static void
c8_Fx0A(chip8 *c8)
{
    uint8_t x = (c8->core.opcode & 0x0F00) >> 8;
    uint8_t no_key_pressed = 1;
    for (uint8_t i = 0; i < 16; i++) {
        if (c8->core.keys[i]) {
            c8->core.V[x] = i;
            no_key_pressed = 0;
            break;
        }
    }
    if (no_key_pressed) {
        c8->core.PC -= 2;
    }
}

static void
c8_Fx15(chip8 *c8)
{
    uint8_t x = (c8->core.opcode & 0x0F00) >> 8;
    c8->core.delay_timer = c8->core.V[x];
}

static void
c8_Fx18(chip8 *c8)
{
    uint8_t x = (c8->core.opcode & 0x0F00) >> 8;
    c8->core.sound_timer = c8->core.V[x];
}

static void
c8_Fx1E(chip8 *c8)
{
    uint8_t x = (c8->core.opcode & 0x0F00) >> 8;
    c8->core.I += c8->core.V[x];
}

static void
c8_Fx29(chip8 *c8)
{
    uint8_t x = (c8->core.opcode & 0x0F00) >> 8;
    uint8_t digit = c8->core.V[x];
    uint16_t fontset_address = &(c8->core.lres_font[0]) - c8->RAM;
    c8->core.I = fontset_address + digit * 5;
}

static void
c8_Fx30(chip8 *c8) {
    uint8_t x = (c8->core.opcode & 0x0F00) >> 8;
    uint8_t digit = c8->core.V[x];
    uint16_t s_fontset_address = &(c8->core.hres_font[0]) - c8->RAM;
    c8->core.I = s_fontset_address + digit * 10;
}

static void
c8_Fx33(chip8 *c8)
{
    uint8_t x = (c8->core.opcode & 0x0F00) >> 8;
    uint8_t value = c8->core.V[x];
    c8->RAM[c8->core.I] = value / 100;
    c8->RAM[(c8->core.I) + 1] = value % 100 / 10;
    c8->RAM[(c8->core.I) + 2] = value % 10;
}

static void
c8_Fx55(chip8 *c8)
{
    uint8_t x = (c8->core.opcode & 0x0F00) >> 8;
    for(uint8_t i = 0; i <= x; i++) {
        c8->RAM[(c8->core.I) + i] = c8->core.V[i];
    }
}

static void
c8_Fx65(chip8 *c8)
{
    uint8_t x = (c8->core.opcode & 0x0F00) >> 8;
    for (uint8_t i = 0; i <= x; i++) {
        c8->core.V[i] = c8->RAM[(c8->core.I) + i];
    }
}

static void
c8_Fx75(chip8 *c8)
{
    uint8_t x = (c8->core.opcode & 0x0F00) >> 8;
    if (x > 7) {
        fprintf(stderr, "ERROR: wrong x value(%d) in c8_Fx75!\n", x);
        c8->core.exit_flag = 1;
        return;
    }
    for (uint8_t i = 0; i <= x; i++) {
        c8->core.rpl_flags[i] = c8->core.V[i];
    }
}

static void
c8_Fx85(chip8 *c8)
{
    uint8_t x = (c8->core.opcode & 0x0F00) >> 8;
    if (x > 7) {
        fprintf(stderr, "ERROR: wrong x value(%d) in c8_Fx75!\n", x);
        c8->core.exit_flag = 1;
        return;
    }
    for (uint8_t i = 0; i <= x; i++) {
        c8->core.V[i] = c8->core.rpl_flags[i];
    }
}

static void
c8_NULL(chip8 *c8)
{
    fprintf(stderr, "ERROR: calling c8_NULL\n");
    fprintf(stderr, "opcode: %04X\n", c8->core.opcode & 0xFFFF);
}

static void
c8_goto_optable_0(chip8 *c8)
{

    /* TODO implement SuperChip-8 instructions */
    if ((c8->core.opcode & 0x00FF) == 0x00FF) {
        fprintf(stderr, "SuperChip-8 instruction 00FF encountered! Proceeding...\n");
    }
    if ((c8->core.opcode & 0x00F0) == 0x00C0) {
        fprintf(stderr, "SuperChip-8 instruction 00Cx encountered! Aborting...\n");
        exit(-1);
    }
    /*fprintf(stderr, "current opcode: %04X\n", c8->core.opcode & 0xFFFF);*/
    optable_0[(c8->core.opcode) & 0x00FF](c8);
}

static void
c8_goto_optable_8(chip8 *c8)
{
    optable_8[(c8->core.opcode) & 0x000F](c8);
}

static void
c8_goto_optable_E(chip8 *c8)
{
    optable_E[c8->core.opcode & 0x00FF](c8);
}

static void
c8_goto_optable_F(chip8 *c8)
{
    optable_F[c8->core.opcode & 0x00FF](c8);
}

static void
init_optable_main()
{
    optable_main[0x0] = c8_goto_optable_0;
    optable_main[0x1] = c8_1nnn;
    optable_main[0x2] = c8_2nnn;
    optable_main[0x3] = c8_3xkk;
    optable_main[0x4] = c8_4xkk;
    optable_main[0x5] = c8_5xy0;
    optable_main[0x6] = c8_6xkk;
    optable_main[0x7] = c8_7xkk;
    optable_main[0x8] = c8_goto_optable_8;
    optable_main[0x9] = c8_9xy0;
    optable_main[0xA] = c8_Annn;
    optable_main[0xB] = c8_Bnnn;
    optable_main[0xC] = c8_Cxkk;
    optable_main[0xD] = c8_Dxyn;
    optable_main[0xE] = c8_goto_optable_E;
    optable_main[0xF] = c8_goto_optable_F;
}

static void
init_optable_0()
{
    for (int i = 0; i < 0xFF + 1; i++) {
        optable_0[i] = c8_NULL;
    }
    for (int i = 0xC0; i <= 0xCF; i++) {
        optable_0[i] = c8_00Cx;
    }
    optable_0[0xE0] = c8_00E0;
    optable_0[0xEE] = c8_00EE;
    optable_0[0xFB] = c8_00FB;
    optable_0[0xFC] = c8_00FC;
    optable_0[0xFD] = c8_00FD;
    optable_0[0xFE] = c8_00FE;
    optable_0[0xFF] = c8_00FF;
}

static void
init_optable_8()
{
    optable_8[0x0] = c8_8xy0;
    optable_8[0x1] = c8_8xy1;
    optable_8[0x2] = c8_8xy2;
    optable_8[0x3] = c8_8xy3;
    optable_8[0x4] = c8_8xy4;
    optable_8[0x5] = c8_8xy5;
    optable_8[0x6] = c8_8xy6;
    optable_8[0x7] = c8_8xy7;
    optable_8[0x8] = c8_NULL;
    optable_8[0x9] = c8_NULL;
    optable_8[0xA] = c8_NULL;
    optable_8[0xB] = c8_NULL;
    optable_8[0xC] = c8_NULL;
    optable_8[0xD] = c8_NULL;
    optable_8[0xE] = c8_8xyE;
}

static void
init_optable_E()
{
    for (int i = 0; i < 0xA1 + 1; i++) {
        optable_E[i] = c8_NULL;
    }
    optable_E[0x9E] = c8_Ex9E;
    optable_E[0xA1] = c8_ExA1;
}

static void
init_optable_F()
{
    for (int i = 0; i < 0x85 + 1; i++) {
        optable_F[i] = c8_NULL;
    }
    optable_F[0x07] = c8_Fx07;
    optable_F[0x0A] = c8_Fx0A;
    optable_F[0x15] = c8_Fx15;
    optable_F[0x18] = c8_Fx18;
    optable_F[0x1E] = c8_Fx1E;
    optable_F[0x29] = c8_Fx29;
    optable_F[0x30] = c8_Fx30;
    optable_F[0x33] = c8_Fx33;
    optable_F[0x55] = c8_Fx55;
    optable_F[0x65] = c8_Fx65;
    optable_F[0x75] = c8_Fx75;
    optable_F[0x85] = c8_Fx85;
}

void
chip8_init(chip8 *c8)
{
    memset(c8, 0, sizeof(*c8));
    memcpy(c8->core.lres_font, lres_sprites, 80);
    memcpy(c8->core.hres_font, hres_sprites, 100);
    c8->core.PC = PROGRAMM_START_OFFSET;
    c8->core.exit_flag = 0;
    c8->core.extended_flag = 0;
}

void
chip8_emulatecycle(chip8 *c8)
{
    /* fetch opcode */
    c8->core.opcode =
        c8->RAM[c8->core.PC] << 8 | c8->RAM[(c8->core.PC) + 1];

    /* increment program counter */
    c8->core.PC += 2;

    /* execute opcode */
    optable_main[(c8->core.opcode & 0xF000) >> 12](c8);

    /* update timers */
    if (c8->core.delay_timer > 0) {
        c8->core.delay_timer -= 1;
    }
    if (c8->core.sound_timer > 0) {
        c8->core.sound_timer -= 1;
    }
}

int
chip8_loadgame(chip8 *c8, const char *game_name)
{
    FILE *game;
    game = fopen(game_name, "rb");
    if (game == NULL) {
        fprintf(stderr, "ERROR: failed to open game: %s.\n", game_name);
        return -1;
    }
    fread(&(c8->RAM[PROGRAMM_START_OFFSET]), 1, MAX_GAME_SIZE, game);
    fclose(game);
    return 0;
}

int
main(int argc, char **argv)
{
    if (argc != 2) {
        fprintf(stderr, "ERROR: wrong arguments!\n");
        fprintf(stderr, "Usage: %s ROM\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    init_optable_main();
    init_optable_0();
    init_optable_8();
    init_optable_E();
    init_optable_F();

    chip8 *c8 = NULL;

    c8 = malloc(sizeof(*c8));
    if (c8 == NULL) {
        fprintf(stderr, "ERROR: memory allocation failed!\n");
        exit(EXIT_FAILURE);
    }

    chip8_init(c8);
    if (chip8_loadgame(c8, argv[1]) == -1) {
        fprintf(stderr, "ERROR: game not loaded!\n");
        exit(EXIT_FAILURE);
    }

    if (sdl_layer_init(argv[1], SC8_DISP_WIDTH, SC8_DISP_HEIGHT, 4) == -1) {
        fprintf(stderr, "ERROR: sdl_layer creation failed!\n");
        exit(EXIT_FAILURE);
    }

    /* for debugging purposes */
    c8->core.extended_flag = 1;

    /*c8_00E0(c8);
    disp_mem[0] = 0xFF;
    disp_mem[2] = 0xFF;
    disp_mem[4] = 0xFF;
    disp_mem[6] = 0xFF;
    disp_mem[8] = 0xFF;
    disp_mem[10] = 0xFF;
    disp_mem[12] = 0xFF;
    disp_mem[14] = 0xFF;

    disp_mem[17] = 0xFF;
    disp_mem[19] = 0xFF;
    disp_mem[21] = 0xFF;
    disp_mem[23] = 0xFF;
    disp_mem[25] = 0xFF;
    disp_mem[27] = 0xFF;
    disp_mem[29] = 0xFF;
    disp_mem[31] = 0xFF;

    disp_mem[32] = 0x81;
    disp_mem[34] = 0xFF;
    disp_mem[36] = 0xFF;
    disp_mem[38] = 0xFF;
    disp_mem[40] = 0xFF;
    disp_mem[42] = 0xFF;
    disp_mem[44] = 0xFF;
    disp_mem[46] = 0xFF;*/

    while (!c8->core.exit_flag) {
        sdl_handle_keystroke(c8->core.keys, &(c8->core.exit_flag));
        chip8_emulatecycle(c8);
        if (c8->core.draw_flag) {
            uint32_t output[SC8_DISP_WIDTH * SC8_DISP_HEIGHT];
            _render_output(disp_mem, output, c8->core.extended_flag);
            sdl_layer_draw(output, SC8_DISP_WIDTH);
            c8->core.draw_flag = 0;
        }
        SDL_Delay(1);
    }
    sdl_layer_destroy();
    free(c8);
    return EXIT_SUCCESS;
}
