#pragma once

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

typedef struct {
    uint8_t registers[16]; // 16 registers, 8 bits each
    uint8_t memory[4096]; // 4K byte memory

    uint16_t index; // index register
    uint16_t pc; // program counter

    uint16_t stack[16]; // 16 level stack
    uint8_t sp; // stack pointer

    uint8_t delay; // delay timer
    uint8_t sound; // sound timer

    uint8_t keypad[16]; // 16 buttons

    uint64_t display[32]; // 64 wide, 32 high monochrome display

    uint16_t opcode; // Current opcode (?)
} CHIP8;

static const unsigned int FONSET_START_ADDRESS = 0x0;
static const unsigned int START_ADDRESS = 0x200;
static const unsigned int MAX_ADDRESS = 0xFFF;

static const unsigned int FONTSET_SIZE = 5 * 16;
static const uint8_t fontset[5 * 16] =
{
    0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
    0x20, 0x60, 0x20, 0x20, 0x70, // 1
    0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
    0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
    0x90, 0x90, 0xF0, 0x10, 0x10, // 4
    0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
    0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
    0xF0, 0x10, 0x20, 0x40, 0x40, // 7
    0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
    0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
    0xF0, 0x90, 0xF0, 0x90, 0x90, // A
    0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
    0xF0, 0x80, 0x80, 0x80, 0xF0, // C
    0xE0, 0x90, 0x90, 0x90, 0xE0, // D
    0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
    0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};

// CHIP8 operations
CHIP8* create_chip8();
void free_chip8(CHIP8* chip);
void dump_chip(CHIP8* chip);
void cycle_chip(CHIP8* chip);
