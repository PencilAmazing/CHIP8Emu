#include <time.h>
#include <stdlib.h>

#include "host_io.h"
#include "chip8.h"

#include "raylib.h"

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
    SetTargetFPS(60);
    //SetTargetFPS(5006);

    int cycle_count = 0;

    while (!WindowShouldClose()) {
        cycle_count += 1;
        cycle_chip(chip);
        BeginDrawing();

        ClearBackground(RAYWHITE);

        int scale = 10;
        int offset = 0;
        int shift = 5;

        for (int x = 0; x < 64; x++) {
            for (int y = 0; y < 32; y++) {
                if ((chip->display[y] & (1ui64 << (63 - x))) != 0)
                    DrawRectangle(shift + x * scale, shift + y * scale, scale - offset, scale - offset, BLACK);
                else
                    DrawRectangle(shift + x * scale, shift + y * scale, scale - offset, scale - offset, WHITE);
            }
        }

        DrawText(TextFormat("%d cycles executed", cycle_count), 30, 360, 12, BLACK);

        EndDrawing();
    }
    CloseWindow();
    printf("Cycle ended\n");

    printf("Shutting down CHIP8");
    free_chip8(chip);
    return 0;
}