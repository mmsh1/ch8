#include <stdlib.h>
#include <stdio.h> /* fclose, fopen, fread, frpintf */
#include <string.h> /* memcpy, memset */

#include "chip8.h"
#include "sdl_layer.h"

typedef void (*c8_opcode_func)(chip8_t *);

static void c8_00E0(chip8_t *);
static void c8_00EE(chip8_t *);
/*TODO*/
static void c8_00Cx(chip8_t *); /* SCD nibble: scroll screen x lines down */
static void c8_00FB(chip8_t *); /* SCL: scroll screen 4 pix left */
static void c8_00FC(chip8_t *); /* SCR: scroll screen 4 pix right */
static void c8_00FD(chip8_t *); /* EXIT: terminate interpreter */
static void c8_00FE(chip8_t *); /* LOW: disable extended screen mode */
static void c8_00FF(chip8_t *); /* HIGH: enable extended screen mode */
/* TODOend*/

static void c8_1nnn(chip8_t *);
static void c8_2nnn(chip8_t *);
static void c8_3xkk(chip8_t *);
static void c8_4xkk(chip8_t *);
static void c8_5xy0(chip8_t *);
static void c8_6xkk(chip8_t *);
static void c8_7xkk(chip8_t *);

static void c8_8xy0(chip8_t *);
static void c8_8xy1(chip8_t *);
static void c8_8xy2(chip8_t *);
static void c8_8xy3(chip8_t *);
static void c8_8xy4(chip8_t *);
static void c8_8xy5(chip8_t *);
static void c8_8xy6(chip8_t *);
static void c8_8xy7(chip8_t *);
static void c8_8xyE(chip8_t *);

static void c8_9xy0(chip8_t *);
static void c8_Annn(chip8_t *);
static void c8_Bnnn(chip8_t *);
static void c8_Cxkk(chip8_t *);
static void c8_Dxyn(chip8_t *);

static void c8_Ex9E(chip8_t *);
static void c8_ExA1(chip8_t *);

static void c8_Fx07(chip8_t *);
static void c8_Fx0A(chip8_t *);
static void c8_Fx15(chip8_t *);
static void c8_Fx18(chip8_t *);
static void c8_Fx1E(chip8_t *);
static void c8_Fx29(chip8_t *);
static void c8_Fx33(chip8_t *);
static void c8_Fx55(chip8_t *);
static void c8_Fx65(chip8_t *);

/*TODO*/
static void c8_Fx30(chip8_t *); /* LD HF, VX: Point I to 10 byte numeric sprite for value in VX */
static void c8_Fx75(chip8_t *); /* LD R, VX: Store V0 .. VX in RPL user flags. Only V0 .. V7 valid */
static void c8_Fx85(chip8_t *); /* LD VX, R: Read V0 .. VX from RPL user flags. Only V0 .. V7 valid */
/* TODOend*/

static void c8_NULL(chip8_t *);
static void c8_goto_optable_0(chip8_t *);
static void c8_goto_optable_8(chip8_t *);
static void c8_goto_optable_E(chip8_t *);
static void c8_goto_optable_F(chip8_t *);

static void init_optable_main();
static void init_optable_0();
static void init_optable_8();
static void init_optable_E();
static void init_optable_F();

c8_opcode_func optable_main[0xF + 1];
c8_opcode_func optable_0[0xE + 1];
c8_opcode_func optable_8[0xE + 1];
c8_opcode_func optable_E[0xA1 + 1];
c8_opcode_func optable_F[0x65 + 1];


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

static void
c8_00E0(chip8_t *c8)
{
    memset(c8->core.disp_mem, 0, sizeof(uint64_t) * C8_DISP_HEIGHT);
    c8->core.draw_flag = 1;
}

static void
c8_00EE(chip8_t *c8)
{
    c8->core.PC = c8->core.stack[c8->core.SP];
    c8->core.SP -= 1;
}

/*TODO*/
static void
c8_00Cx(chip8_t *c8)
{
    /* SCD nibble: scroll screen x lines down */
}

static void
c8_00FB(chip8_t *c8)
{
    /* SCL: scroll screen 4 pix left */
}

static void
c8_00FC(chip8_t *c8)
{
    /* SCR: scroll screen 4 pix right */
}

static void
c8_00FD(chip8_t *c8)
{
    /* EXIT: terminate interpreter */
    exit(-1); /* TODO add termination exit code */
}

static void
c8_00FE(chip8_t *c8)
{
    /* LOW: disable extended screen mode */
}

static void
c8_00FF(chip8_t *c8)
{
    /* HIGH: enable extended screen mode */
}
/* TODOend */

static void
c8_1nnn(chip8_t *c8)
{
    c8->core.PC = c8->core.opcode & 0x0FFF;
}

static void
c8_2nnn(chip8_t *c8)
{
    c8->core.SP += 1;
    c8->core.stack[c8->core.SP] = c8->core.PC;
    c8->core.PC = c8->core.opcode & 0x0FFF;
}

static void
c8_3xkk(chip8_t *c8)
{
    uint8_t x = (c8->core.opcode & 0x0F00) >> 8;
    if (c8->core.V[x] == (c8->core.opcode & 0x00FF)) {
        c8->core.PC += 2;
    }
}

static void
c8_4xkk(chip8_t *c8)
{
    uint8_t x = (c8->core.opcode & 0x0F00) >> 8;
    if (c8->core.V[x] != (c8->core.opcode & 0x00FF)) {
        c8->core.PC += 2;
    }
}

static void
c8_5xy0(chip8_t *c8)
{
    uint8_t x = (c8->core.opcode & 0x0F00) >> 8;
    uint8_t y = (c8->core.opcode & 0x00F0) >> 4;
    if (c8->core.V[x] == c8->core.V[y]) {
        c8->core.PC += 2;
    }
}

static void
c8_6xkk(chip8_t *c8)
{
    uint8_t x = (c8->core.opcode & 0x0F00) >> 8;
    c8->core.V[x] = c8->core.opcode & 0x00FF;
}

static void
c8_7xkk(chip8_t *c8)
{
    uint8_t x = (c8->core.opcode & 0x0F00) >> 8;
    c8->core.V[x] += c8->core.opcode & 0x00FF;
}

static void
c8_8xy0(chip8_t *c8)
{
    uint8_t x = (c8->core.opcode & 0x0F00) >> 8;
    uint8_t y = (c8->core.opcode & 0x00F0) >> 4;
    c8->core.V[x] = c8->core.V[y];
}

static void
c8_8xy1(chip8_t *c8)
{
    uint8_t x = (c8->core.opcode & 0x0F00) >> 8;
    uint8_t y = (c8->core.opcode & 0x00F0) >> 4;
    c8->core.V[x] |= c8->core.V[y];
}

static void
c8_8xy2(chip8_t *c8)
{
    uint8_t x = (c8->core.opcode & 0x0F00) >> 8;
    uint8_t y = (c8->core.opcode & 0x00F0) >> 4;
    c8->core.V[x] &= c8->core.V[y];
}

static void
c8_8xy3(chip8_t *c8)
{
    uint8_t x = (c8->core.opcode & 0x0F00) >> 8;
    uint8_t y = (c8->core.opcode & 0x00F0) >> 4;
    c8->core.V[x] ^= c8->core.V[y];
}

static void
c8_8xy4(chip8_t *c8)
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
c8_8xy5(chip8_t *c8)
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
c8_8xy6(chip8_t *c8)
{
    uint8_t x = (c8->core.opcode & 0x0F00) >> 8;
    c8->core.V[0xF] = (c8->core.V[x] & 0x01);
    c8->core.V[x] >>= 1;
}

static void
c8_8xy7(chip8_t *c8)
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
c8_8xyE(chip8_t *c8)
{
    uint8_t x = (c8->core.opcode & 0x0F00) >> 8;
    c8->core.V[0xF] = (c8->core.V[x] & 0x80) >> 7;
    c8->core.V[x] <<= 1;
}

static void
c8_9xy0(chip8_t *c8)
{
    uint8_t x = (c8->core.opcode & 0x0F00) >> 8;
    uint8_t y = (c8->core.opcode & 0x00F0) >> 4;
    if (c8->core.V[x] != c8->core.V[y]) {
        c8->core.PC += 2;
    }
}

static void
c8_Annn(chip8_t *c8)
{
    c8->core.I = c8->core.opcode & 0x0FFF;
}

static void
c8_Bnnn(chip8_t *c8)
{
    c8->core.PC = (c8->core.opcode & 0x0FFF) + c8->core.V[0];
}

static void
c8_Cxkk(chip8_t *c8)
{
    uint8_t x = (c8->core.opcode & 0x0F00) >> 8;
    c8->core.V[x] = (rand() % 256) & (c8->core.opcode & 0x00FF);
}

static void
c8_Dxyn(chip8_t *c8)
{
    uint8_t x = (c8->core.opcode & 0x0F00) >> 8;
    uint8_t y = (c8->core.opcode & 0x00F0) >> 4;
    uint8_t height = (c8->core.opcode & 0x000F);

    uint8_t ypos = c8->core.V[y];
    uint8_t xpos = c8->core.V[x] + 8;
    uint64_t flag = 0;

    c8->core.V[0xF] = 0;

    for (int row = 0; row < height; row++) {
        uint64_t *disp_row = &(c8->core.disp_mem[(ypos + row) % 32]);
        uint64_t sprite_row = _rotate_r64((uint64_t)c8->RAM[c8->core.I + row], xpos);
        flag |= *disp_row & sprite_row;
        *disp_row ^= sprite_row;

        if (flag) {
            c8->core.V[0xF] = 1;
        }
    }
    c8->core.draw_flag = 1;
}

static void
c8_Ex9E(chip8_t *c8)
{
    uint8_t x = (c8->core.opcode & 0x0F00) >> 8;
    uint8_t keynum = c8->core.V[x];
    if (c8->core.keys[keynum]) {
        c8->core.PC += 2;
    }
}

static void
c8_ExA1(chip8_t *c8)
{
    uint8_t x = (c8->core.opcode & 0x0F00) >> 8;
    uint8_t keynum = c8->core.V[x];
    if (!c8->core.keys[keynum]) {
        c8->core.PC += 2;
    }
}

static void
c8_Fx07(chip8_t *c8)
{
    uint8_t x = (c8->core.opcode & 0x0F00) >> 8;
    c8->core.V[x] = c8->core.delay_timer;
}

static void
c8_Fx0A(chip8_t *c8)
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
c8_Fx15(chip8_t *c8)
{
    uint8_t x = (c8->core.opcode & 0x0F00) >> 8;
    c8->core.delay_timer = c8->core.V[x];
}

static void
c8_Fx18(chip8_t *c8)
{
    uint8_t x = (c8->core.opcode & 0x0F00) >> 8;
    c8->core.sound_timer = c8->core.V[x];
}

static void
c8_Fx1E(chip8_t *c8)
{
    uint8_t x = (c8->core.opcode & 0x0F00) >> 8;
    c8->core.I += c8->core.V[x];
}

static void
c8_Fx29(chip8_t *c8)
{
    uint8_t x = (c8->core.opcode & 0x0F00) >> 8;
    uint8_t digit = c8->core.V[x];
    uint16_t fontset_address = &(c8->core.font[0]) - c8->RAM;
    c8->core.I = fontset_address + digit * 5;
}

static void
c8_Fx33(chip8_t *c8)
{
    uint8_t x = (c8->core.opcode & 0x0F00) >> 8;
    uint8_t value = c8->core.V[x];
    c8->RAM[c8->core.I] = value / 100;
    c8->RAM[(c8->core.I) + 1] = value % 100 / 10;
    c8->RAM[(c8->core.I) + 2] = value % 10;
}

static void
c8_Fx55(chip8_t *c8)
{
    uint8_t x = (c8->core.opcode & 0x0F00) >> 8;
    for(uint8_t i = 0; i <= x; i++) {
        c8->RAM[(c8->core.I) + i] = c8->core.V[i];
    }
}

static void
c8_Fx65(chip8_t *c8)
{
    uint8_t x = (c8->core.opcode & 0x0F00) >> 8;
    for (uint8_t i = 0; i <= x; i++) {
        c8->core.V[i] = c8->RAM[(c8->core.I) + i];
    }
}

static void
c8_NULL(chip8_t *c8)
{
    fprintf(stderr, "calling c8_NULL\n");
    fprintf(stderr, "opcode: %x\n", c8->core.opcode & 0xFFFF);
}

static void
c8_goto_optable_0(chip8_t *c8)
{
    /* TODO implement SuperChip-8 instructions */
    if ((c8->core.opcode & 0x00FF) == 0x00FF) {
        fprintf(stderr, "SuperChip-8 instruction 00FF encountered! Aborting...\n");
        exit(-1);
    }
    if ((c8->core.opcode & 0x00F0) == 0x00C0) {
        fprintf(stderr, "SuperChip-8 instruction 00Cx encountered! Aborting...\n");
        exit(-1);
        /*optable_0[ _index_to_00Cx_ ](c8); */
    }
    optable_0[c8->core.opcode & 0x000F](c8);
}

static void
c8_goto_optable_8(chip8_t *c8)
{
    optable_8[(c8->core.opcode) & 0x000F](c8);
}

static void
c8_goto_optable_E(chip8_t *c8)
{
    optable_E[c8->core.opcode & 0x00FF](c8);
}

static void
c8_goto_optable_F(chip8_t *c8)
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
    optable_0[0x0] = c8_00E0;
    optable_0[0x1] = c8_NULL;
    optable_0[0x2] = c8_NULL;
    optable_0[0x3] = c8_NULL;
    optable_0[0x4] = c8_NULL;
    optable_0[0x5] = c8_NULL;
    optable_0[0x6] = c8_NULL;
    optable_0[0x7] = c8_NULL;
    optable_0[0x8] = c8_NULL;
    optable_0[0x9] = c8_NULL;
    optable_0[0xA] = c8_NULL;
    optable_0[0xB] = c8_NULL;
    optable_0[0xC] = c8_NULL;
    optable_0[0xD] = c8_NULL;
    optable_0[0xE] = c8_00EE;
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
    for (int i = 0; i < 0x65 + 1; i++) {
        optable_F[i] = c8_NULL;
    }
    optable_F[0x07] = c8_Fx07;
    optable_F[0x0A] = c8_Fx0A;
    optable_F[0x15] = c8_Fx15;
    optable_F[0x18] = c8_Fx18;
    optable_F[0x1E] = c8_Fx1E;
    optable_F[0x29] = c8_Fx29;
    optable_F[0x33] = c8_Fx33;
    optable_F[0x55] = c8_Fx55;
    optable_F[0x65] = c8_Fx65;
}

void
chip8_init(chip8_t *c8)
{
    memset(c8, 0, sizeof(*c8));
    c8->core.PC = PROGRAMM_START_OFFSET;
    memcpy(c8->core.font, sprites, 80);
    c8->core.exit_flag = 0;
}

void
chip8_emulatecycle(chip8_t *c8)
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
chip8_loadgame(chip8_t *c8, const char *game_name)
{
    FILE *game;
    game = fopen(game_name, "rb");
    if (game == NULL) {
        fprintf(stderr, "Error: failed to open game: %s.\n", game_name);
        return -1;
    }
    fread(&(c8->RAM[0x200]), 1, MAX_GAME_SIZE, game);
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

    init_optable_main();
    init_optable_0();
    init_optable_8();
    init_optable_E();
    init_optable_F();

    chip8_t *c8 = NULL;
    /*uint8_t quit_flag = 0;*/

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

    while (!c8->core.exit_flag) {
        sdl_handle_keystroke(c8->core.keys, &(c8->core.exit_flag));
        chip8_emulatecycle(c8);
        if (c8->core.draw_flag) {

            /*uint32_t output[C8_DISP_WIDTH * C8_DISP_HEIGHT];*/
            uint32_t output[128 * 64];
            sdl_layer_draw(c8->core.disp_mem,
                           output,
                           /*C8_DISP_WIDTH * C8_DISP_HEIGHT);*/
                           128 * 64);
            c8->core.draw_flag = 0;
        }
        SDL_Delay(1);
    }
    sdl_layer_destroy();
    free(c8);
    return EXIT_SUCCESS;
}
