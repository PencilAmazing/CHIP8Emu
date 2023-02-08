#pragma once

#include "chip8.h"
#include <stdio.h>
#include <stdlib.h>

// Load program from disk into chip8 struct
int load_program(char* filename, CHIP8* chip)
{
    //FILE* rom = fopen(filename, "r");
    FILE* rom = NULL;
    int err = fopen_s(&rom, filename, "rb");
    if (err != 0) return err;

    fseek(rom, 0, SEEK_END);
    long length = ftell(rom);
    if (length <= 0) {
        fclose(rom);
        return 1;
    }
    rewind(rom);
    //fseek(rom, 0, SEEK_SET);

    // integer overflow maybe?
    uint8_t* data = calloc(length + 1, sizeof(uint8_t));
    if (data == NULL) {
        fclose(rom);
        return 1;
    };

    fread(data, length, 1, rom);
    for (int i = 0; i <= length; i++) {
        chip->memory[START_ADDRESS + i] = data[i];
    }

    fclose(rom);
    free(data);

    return 0;
}
