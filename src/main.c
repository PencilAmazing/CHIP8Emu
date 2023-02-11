#include <time.h>
#include <stdlib.h>
#include <string.h>

#include "host_io.h"
#include "chip8.h"

#include "raylib.h"

void raylib_test_keys(CHIP8* chip)
{

    /*
    1 	2 	3 	C
    4 	5 	6 	D
    7 	8 	9 	E
    A 	0 	B 	F
    -------------
    1 	2 	3 	4
    Q 	W 	E 	R
    A 	S 	D 	F
    Z 	X 	C 	V
    */

    KeyboardKey scan_keys[] = {
        KEY_X,    KEY_ONE,  KEY_TWO, KEY_THREE,
        KEY_Q,    KEY_W,    KEY_E,   KEY_A,
        KEY_S,    KEY_D,    KEY_Z,   KEY_C,
        KEY_FOUR, KEY_R,    KEY_F,   KEY_V
    };

    for (int i = 0; i < 16; i++) {
        if (IsKeyDown(scan_keys[i])) {
            chip->keypad[i] = 0x1;
        } else {
            chip->keypad[i] = 0x0;
        }
    }
}

int main(int argc, char** argv)
{
    srand(time(NULL));

    int err = 0;

    printf("Creating CHIP8 model\n");

    CHIP8* chip = create_chip8();

    if (chip == NULL) {
        printf("Failed to create model\n");
        return 1;
    }

    printf("Loading test_opcode.ch8\n");
    //err = load_program("c8_test.ch8", chip);
    //err = load_program("test_opcode.ch8", chip);
    err = load_program("chip8-test-suite.ch8", chip);
    //err = load_program("maze.ch8", chip);

    if (err != 0) {
        printf("%d Failed to load program\n", err);
        return err;
    }

    printf("Starting cycle\n");

    InitWindow(800, 400, "CHIP8 Emulator");

    int cycle_count = 0;

    // For aftereffect like CRT that doesn't even work
    uint64_t prev_display[32] = { 0 };

    // Don't set target fps that's stupid
    // SetTargetFPS(5006);

    while (!WindowShouldClose()) {
        // Copy prev display
        memcpy(prev_display, chip->display, sizeof(prev_display));
        // Test keys
        raylib_test_keys(chip);
        // Cycle and update current display
        cycle_chip(chip);
        cycle_count += 1;

        int scale = 10;
        int offset = 0;
        int shift = 5;

        BeginDrawing();
        ClearBackground(RAYWHITE);

        for (int x = 0; x < 64; x++) {
            for (int y = 0; y < 32; y++) {
                Color PixelColor = WHITE;
                uint64_t xmask = (1ui64 << (63 - x));

                if ((chip->display[y] & xmask) != 0)
                    PixelColor = BLACK;
                else if ((prev_display[y] & xmask) != 0)
                    PixelColor = GRAY;

                DrawRectangle(shift + x * scale, shift + y * scale, scale - offset, scale - offset, PixelColor);
            }
        }

        DrawText(TextFormat("%d cycles executed", cycle_count), 30, 360, 12, BLACK);
        DrawText(TextFormat("Current opcode 0x%x", chip->opcode), 30, 380, 12, BLACK);
        EndDrawing();
    }
    CloseWindow();
    printf("Cycle ended\n");

    printf("Shutting down CHIP8");
    free_chip8(chip);
    return 0;
}