#include "opcode.h"

static const uint16_t nnn = 0x0FFFu; // Lowest 12 bit value
static const uint16_t n = 0x000Fu; // lowest 4 bit of instruction
static const uint16_t x = 0x0F00u; // lower 4 bits of the high byte
static const uint16_t y = 0x00F0u; // upper 4 bits of the low byte
static const uint16_t kk = 0x00FFu; // lowest 8 bits of instruction

// clear screen
void CHIP8_00E0(CHIP8* chip)
{
    memset(chip->display, 0, 32 * sizeof(uint64_t));
}

// return
void CHIP8_00EE(CHIP8* chip)
{
    chip->sp -= 1;
    chip->pc = chip->stack[chip->sp];
}

// Jump
void CHIP8_1NNN(CHIP8* chip)
{
    chip->pc = chip->opcode & nnn;
}

// CALL addr
void CHIP8_2NNN(CHIP8* chip)
{
    chip->stack[chip->sp] = chip->pc;
    chip->sp += 1;
    chip->pc = chip->opcode & nnn;
}

// SE Vx, byte
void CHIP8_3XKK(CHIP8* chip)
{
    uint8_t Vx = chip->registers[(chip->opcode & x) >> 8u];
    uint8_t KK = chip->opcode & kk;
    if (Vx == KK) chip->pc += 2; // Skip cycle
}

void CHIP8_4XKK(CHIP8* chip)
{
    uint8_t Vx = chip->registers[(chip->opcode & x) >> 8u];
    uint8_t KK = chip->opcode & kk;
    if (Vx != KK) chip->pc += 2; // Skip cycle
}

void CHIP8_5XY0(CHIP8* chip)
{
    uint8_t Vx = chip->registers[(chip->opcode & x) >> 8u];
    uint8_t Vy = chip->registers[(chip->opcode & y) >> 4u];
    if (Vx == Vy) chip->pc += 2; // Skip cycle
}

// LD Vx, byte
void CHIP8_6XKK(CHIP8* chip)
{
    uint8_t byte = chip->opcode & kk;
    chip->registers[(chip->opcode & x) >> 8u] = byte;
}

// ADD Vx, kk
void CHIP8_7XKK(CHIP8* chip)
{
    uint8_t Vx = (chip->opcode & x) >> 8u;
    uint8_t byte = chip->opcode & kk;
    chip->registers[Vx] += byte;
}

// LD Vx, Vy
void CHIP8_8XY0(CHIP8* chip)
{
    uint8_t Vx = (chip->opcode & x) >> 8u;
    uint8_t Vy = (chip->opcode & y) >> 4u;
    chip->registers[Vx] = chip->registers[Vy];
}

void CHIP8_8XY1(CHIP8* chip)
{
    uint8_t Vx = (chip->opcode & x) >> 8u;
    uint8_t Vy = (chip->opcode & y) >> 4u;
    chip->registers[Vx] |= chip->registers[Vy];
    chip->registers[0xF] = 0;
}

void CHIP8_8XY2(CHIP8* chip)
{
    uint8_t Vx = (chip->opcode & x) >> 8u;
    uint8_t Vy = (chip->opcode & y) >> 4u;
    chip->registers[Vx] &= chip->registers[Vy];
    chip->registers[0xF] = 0;
}

void CHIP8_8XY3(CHIP8* chip)
{
    uint8_t Vx = (chip->opcode & x) >> 8u;
    uint8_t Vy = (chip->opcode & y) >> 4u;
    chip->registers[Vx] ^= chip->registers[Vy];
    chip->registers[0xF] = 0;
}

void CHIP8_8XY4(CHIP8* chip)
{
    uint8_t Vx = (chip->opcode & x) >> 8u;
    uint8_t Vy = (chip->opcode & y) >> 4u;

    uint8_t a = chip->registers[Vx];
    uint8_t b = chip->registers[Vy];

    // Detect overflow before it happens
    // Thanks C99 standard
    // a + b > 255
    if (a > UINT8_MAX - b)
        chip->registers[0xF] = 1;
    else
        chip->registers[0xF] = 0;

    chip->registers[Vx] = a + b;
}

void CHIP8_8XY5(CHIP8* chip)
{
    uint8_t Vx = (chip->opcode & x) >> 8u;
    uint8_t Vy = (chip->opcode & y) >> 4u;

    uint8_t a = chip->registers[Vx];
    uint8_t b = chip->registers[Vy];

    chip->registers[0xF] = a > b ? 1 : 0;
    chip->registers[Vx] = a - b;
}

// SHR Vx, {, Vy}
void CHIP8_8XY6(CHIP8* chip)
{
    /*
    uint8_t Vx = (chip->opcode & x) >> 8u;
    // Save least significant bit in VF
    chip->registers[0xF] = chip->registers[Vx] & 1;
    chip->registers[Vx] >>= 1; // divide by 2
    */
    uint8_t Vx = (chip->opcode & x) >> 8u;
    uint8_t Vy = (chip->opcode & y) >> 4u;

    // FIXME store in VF after shift
    // Save least significant bit in VF

    // Don't be a smartass
    // This thing here makes us pass the test
    uint8_t value = chip->registers[Vy];
    chip->registers[Vx] = value >> 1;
    chip->registers[0xF] = (value & 1);
}

// SUBN Vx, Vy
void CHIP8_8XY7(CHIP8* chip)
{
    uint8_t Vx = (chip->opcode & x) >> 8u;
    uint8_t Vy = (chip->opcode & y) >> 4u;

    uint8_t a = chip->registers[Vx];
    uint8_t b = chip->registers[Vy];

    chip->registers[0xF] = b > a ? 1 : 0;
    chip->registers[Vx] = b - a;
}

void CHIP8_8XYE(CHIP8* chip)
{
    uint8_t Vx = (chip->opcode & x) >> 8u;
    uint8_t Vy = (chip->opcode & y) >> 4u;

    // Get most significant bit
    chip->registers[Vx] = chip->registers[Vy];
    chip->registers[Vx] <<= 1;
    chip->registers[0xF] = (chip->registers[Vx] & (1 << (8 - 1))) != 0 ? 1 : 0;
}

void CHIP8_9XY0(CHIP8* chip)
{
    uint8_t Vx = (chip->opcode & x) >> 8u;
    uint8_t Vy = (chip->opcode & y) >> 4u;

    if (chip->registers[Vx] != chip->registers[Vy])
        chip->pc += 2;
}

void CHIP8_ANNN(CHIP8* chip)
{
    uint16_t addr = chip->opcode & nnn;
    chip->index = addr;
}

// JP V0, addr
void CHIP8_BNNN(CHIP8* chip)
{
    // Quirk (use Vx in BXNN)
    //uint16_t addr = chip->opcode & nnn;
    //uint8_t Vx = (chip->opcode & x) >> 8u;
    //chip->pc = addr + (uint16_t)chip->registers[Vx];

    // non quirk thing (use V0)
    uint16_t addr = chip->opcode & nnn;
    chip->pc = addr + chip->registers[0];
}

// RND Vx, byte
void CHIP8_CXKK(CHIP8* chip)
{
    uint8_t Vx = (chip->opcode & x) >> 8u;
    uint8_t byte = chip->opcode & kk;
    // slightly biased random generation
    // but you're not doing SSL with a chip8, i hope
    chip->registers[Vx] = (rand() % 0xFFui8) & byte;
}

static inline uint64_t rotl(uint64_t n, unsigned int c)
{
    const unsigned int mask = (CHAR_BIT * sizeof(n) - 1);  // assumes width is a power of 2.
    //assert((c <= mask) && "rotate by type width or more");
    c &= mask;
    return (n << c) | (n >> ((-c) & mask));
};

static inline uint64_t rotr(uint64_t n, unsigned int c)
{
    const unsigned int mask = (CHAR_BIT * sizeof(n) - 1);

    // assert ( (c<=mask) &&"rotate by type width or more");
    c &= mask;
    return (n >> c) | (n << ((-c) & mask));
}

// DRW Vx, Vy, nibble
void CHIP8_DXYN(CHIP8* chip)
{
    uint8_t Vx = (chip->opcode & x) >> 8u;
    uint8_t Vy = (chip->opcode & y) >> 4u;
    uint8_t N = chip->opcode & n;

    // clamp xpos to [0,63]
    uint8_t xpos = chip->registers[Vx] % 64;
    // 8 bit wide mask, circular rotated by 64-xpos

    // clamp ypos to [0, 31] initially
    // Wrap around initial start coord
    //uint8_t ypos = chip->registers[Vy] % 32;
    uint8_t ypos = chip->registers[Vy] % 32;

    chip->registers[0xF] = 0;

    // read n bytes
    for (uint8_t i = 0; i < N; i++) {
        // Shift y display
        // Clip at bottom
        ypos += 1;
        if (ypos > 31)
            break;
        // Get sprite
        // All sprites are 8 pixels wide
        uint8_t byte = chip->memory[chip->index + i];
        // Expand sprite byte, rotate left 64-xpos
        //uint64_t spriteline = ((uint64_t)byte << (64 - xpos)) | (byte >> xpos);

        // Quirk: XO-chip wraps pixels. We don't
        //uint64_t spriteline = rotr((uint64_t)byte, 8 + xpos);
        //uint64_t spriteline = (uint64_t)byte << 64 - (xpos);
        // Wrap around initial 8 bits to left side
        uint64_t spriteline = rotr((uint64_t)byte, 8);
        // Actually shift/clip
        spriteline = (uint64_t)spriteline >> (xpos);

        uint64_t scanline = chip->display[ypos];
        // If bits already exist set collision bit
        if ((scanline & spriteline) != 0ull)
            chip->registers[0xF] = 1;

        // XOR whole byte on scanline
        chip->display[ypos] = scanline ^ spriteline;

        // clip if next ypos is > 31
        //if (chip->registers[Vy] + i + 1 > 31) break;
    }
}

void CHIP8_EX9E(CHIP8* chip)
{
    uint8_t Vx = (chip->opcode & x) >> 8u;
    if (chip->keypad[chip->registers[Vx]] != 0) { // key is down
        chip->pc += 2;
    }
}

void CHIP8_EXA1(CHIP8* chip)
{
    uint8_t Vx = (chip->opcode & x) >> 8u;
    if (chip->keypad[chip->registers[Vx]] == 0) { // key is up
        chip->pc += 2;
    }
}

void CHIP8_FX07(CHIP8* chip)
{
    uint8_t Vx = (chip->opcode & x) >> 8u;
    chip->registers[Vx] = chip->delay;
}

void CHIP8_FX0A(CHIP8* chip)
{
    uint8_t Vx = (chip->opcode & x) >> 8u;
    for (int i = 0; i < 16; i++) {
        if (chip->keypad[i] != 0) { // Key is down
            // Store key in Vx
            chip->registers[Vx] = i;
            // cancel step backwards
            chip->pc += 2;
        }
    }
    // repeat same instruction
    chip->pc -= 2;
}

void CHIP8_FX15(CHIP8* chip)
{
    uint8_t Vx = (chip->opcode & x) >> 8u;
    chip->delay = chip->registers[Vx];
}

void CHIP8_FX18(CHIP8* chip)
{
    uint8_t Vx = (chip->opcode & x) >> 8u;
    chip->sound = chip->registers[Vx];
}

void CHIP8_FX1E(CHIP8* chip)
{
    uint8_t Vx = (chip->opcode & x) >> 8u;
    uint16_t value = (uint16_t)chip->registers[Vx] + chip->index;
    chip->index = (uint16_t)value;
    if (value > 0x0FFF)
        chip->registers[0xF] = 1;
    else
        chip->registers[0xF] = 0;
}

void CHIP8_FX29(CHIP8* chip)
{
    uint8_t Vx = (chip->opcode & x) >> 8u;
    // Each font symbol is 5 bytes wide
    chip->index = FONSET_START_ADDRESS + (chip->registers[Vx] * 5);
}

void CHIP8_FX33(CHIP8* chip)
{
    uint8_t Vx = (chip->opcode & x) >> 8u;
    uint8_t value = chip->registers[Vx];
    // integer division
    // the compiler knows what im doing
    uint8_t ones = value % 10;
    chip->memory[chip->index + 2] = ones;
    uint8_t tens = (value / 10) % 10;
    chip->memory[chip->index + 1] = tens;
    uint8_t hundreds = (value / 100) % 10;
    chip->memory[chip->index] = hundreds;
}

void CHIP8_FX55(CHIP8* chip)
{
    uint8_t Vx = (chip->opcode & x) >> 8u;
    uint8_t l = chip->registers[Vx];
    for (uint8_t i = 0; i <= l; i++) {
        chip->memory[chip->index] = chip->registers[i];
        chip->index += 1;
    }

}

void CHIP8_FX65(CHIP8* chip)
{
    uint8_t Vx = (chip->opcode & x) >> 8u;
    for (uint8_t i = 0; i <= Vx; i++) {
        chip->registers[i] = chip->memory[chip->index];
        chip->index += 1;
    }
}

void CHIP8_NOOP(CHIP8* chip) { return; };

static CHIP8_OPCODE OP_0_TABLE[] = {
    CHIP8_00E0,
    CHIP8_NOOP, // 1
    CHIP8_NOOP, // 2
    CHIP8_NOOP, // 3
    CHIP8_NOOP, // 4
    CHIP8_NOOP, // 5
    CHIP8_NOOP, // 6
    CHIP8_NOOP, // 7
    CHIP8_NOOP, // 8
    CHIP8_NOOP, // 9
    CHIP8_NOOP, // A
    CHIP8_NOOP, // B
    CHIP8_NOOP, // C
    CHIP8_NOOP, // D
    CHIP8_00EE,
};

static CHIP8_OPCODE OP_8_TABLE[] = {
    CHIP8_8XY0,
    CHIP8_8XY1,
    CHIP8_8XY2,
    CHIP8_8XY3,
    CHIP8_8XY4,
    CHIP8_8XY5,
    CHIP8_8XY6,
    CHIP8_8XY7,
    CHIP8_NOOP, // 8
    CHIP8_NOOP, // 9
    CHIP8_NOOP, // A
    CHIP8_NOOP, // B
    CHIP8_NOOP, // C
    CHIP8_NOOP, // D
    CHIP8_8XYE,
};

static CHIP8_OPCODE OP_E_TABLE[] = {
    CHIP8_NOOP,
    CHIP8_EXA1,
    CHIP8_NOOP,
    CHIP8_NOOP,
    CHIP8_NOOP,
    CHIP8_NOOP,
    CHIP8_NOOP,
    CHIP8_NOOP,
    CHIP8_NOOP, // 8
    CHIP8_NOOP, // 9
    CHIP8_NOOP, // A
    CHIP8_NOOP, // B
    CHIP8_NOOP, // C
    CHIP8_NOOP, // D
    CHIP8_8XYE, // E
};

const CHIP8_OPCODE OP_TABLE[] = {
    CHIP8_decode_0_op,
    CHIP8_1NNN,
    CHIP8_2NNN,
    CHIP8_3XKK,
    CHIP8_4XKK,
    CHIP8_5XY0,
    CHIP8_6XKK,
    CHIP8_7XKK,
    CHIP8_decode_8_op, // op 8 does not exist
    CHIP8_9XY0,
    CHIP8_ANNN,
    CHIP8_BNNN,
    CHIP8_CXKK,
    CHIP8_DXYN,
    CHIP8_decode_E_op,
    CHIP8_decode_F_op,
};

void CHIP8_decode_0_op(CHIP8* chip)
{
    switch (chip->opcode & kk) {
    case 0xE0:
        CHIP8_00E0(chip);
        break;
    case 0x0EE:
        CHIP8_00EE(chip);
        break;
    default:
        printf("Unknown opcode 0x%x\n", chip->opcode);
        break;
    }
    //(*OP_0_TABLE[chip->opcode & n])(chip);
};

void CHIP8_decode_8_op(CHIP8* chip)
{
    (*OP_8_TABLE[chip->opcode & n])(chip);
};

void CHIP8_decode_E_op(CHIP8* chip)
{
    switch (chip->opcode & kk) {
    case 0x9E:
        CHIP8_EX9E(chip);
        break;
    case 0xA1:
        CHIP8_EXA1(chip);
        break;
    default:
        printf("Unknown opcode 0x%x\n", chip->opcode);
        break;
    }
    //(*OP_E_TABLE[chip->opcode & n])(chip);
}

void CHIP8_decode_F_op(CHIP8* chip)
{
    switch (chip->opcode & kk) {
    case 0x07:
        CHIP8_FX07(chip);
        break;
    case 0x0A:
        CHIP8_FX0A(chip);
        break;
    case 0x15:
        CHIP8_FX18(chip);
        break;
    case 0x18:
        CHIP8_FX18(chip);
        break;
    case 0x1E:
        CHIP8_FX1E(chip);
        break;
    case 0x29:
        CHIP8_FX29(chip);
        break;
    case 0x33:
        CHIP8_FX33(chip);
        break;
    case 0x55:
        CHIP8_FX55(chip);
        break;
    case 0x65:
        CHIP8_FX65(chip);
        break;
    default:
        CHIP8_NOOP(chip);
        break;
    }
}
