#include "chip8.h"
#include "opcode.h"

CHIP8* create_chip8()
{
    CHIP8* chip = malloc(sizeof(CHIP8));
    if (chip == NULL) return NULL;
    memset(chip, 0, sizeof(CHIP8));

    for (int i = 0; i < 16; i++) {
        chip->registers[i] = 0;
        chip->keypad[i] = 0;
    }

    // Setup program counter
    chip->pc = START_ADDRESS;

    // load fonset
    for (unsigned int i = 0; i < FONTSET_SIZE; i++) {
        chip->memory[FONSET_START_ADDRESS + i] = fontset[i];
    }

    // init debug values
    //chip->memory[0x1FF] = 5;
    //chip->memory[0x1FE] = 1;

    return chip;
}

void free_chip8(CHIP8* chip)
{
    free(chip);
}

void dump_chip(CHIP8* chip)
{
    return;
}

void cycle_chip(CHIP8* chip)
{
    // fetch
    chip->opcode = (chip->memory[chip->pc] << 8u) | (chip->memory[chip->pc + 1]);
    // advance
    chip->pc += 2;
    // decode and execute
    (*(OP_TABLE[chip->opcode >> 12u]))(chip);
    if (chip->delay > 0) chip->delay -= 1;
    if (chip->sound > 0) chip->sound -= 1;
}
