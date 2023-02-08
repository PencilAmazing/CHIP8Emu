#pragma once

#include "chip8.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

typedef void (*CHIP8_OPCODE)(CHIP8* chip);

extern const CHIP8_OPCODE OP_TABLE[];

void CHIP8_decode_0_op(CHIP8* chip);
void CHIP8_decode_8_op(CHIP8* chip);
void CHIP8_decode_E_op(CHIP8* chip);
void CHIP8_decode_F_op(CHIP8* chip);
