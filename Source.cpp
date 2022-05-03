#include "raylib.h"
#include <string>
#include <iostream>
#include "World.h"

int main(void)
{
    // Initialization
    //--------------------------------------------------------------------------------------
    const int screenWidth = 1200;
    const int screenHeight = 800;

    InitWindow(screenWidth, screenHeight, "raylib [core] example - basic window");

    SetTargetFPS(60);               // Set our game to run at 60 frames-per-second
    //--------------------------------------------------------------------------------------

    const int width = screenWidth / 6;
    const int height = screenHeight / 4;
    const float maxEnergy = 5;
    const int firstLayerSize = 4;
    const int internalLayerAmount = 1;
    const int internalLayerSize = 1;
    const int finalLayerSize = 3;
    const int inputsAmount = 19;
    const float maxWeight = 2;

    World world(screenWidth / 6, screenHeight / 4, 5, 0.2, 0.1, 2);
    Creature* adam;
    for (int i = 0; i < 10; i++) {
        for (int j = 0; j < 10; j++) {
            adam = new Creature(maxEnergy, firstLayerSize, internalLayerAmount, internalLayerSize, finalLayerSize, inputsAmount, maxWeight);
            world.placeCreature(adam, 1 + i * 19, 1 + j * 19);
        }
    }
    
    int timer = 0;
    Creature*** map;
    float totalEnergy = 0;

    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        // Update
        //----------------------------------------------------------------------------------
        // TODO: Update your variables here
        //----------------------------------------------------------------------------------
        timer++;
        world.step(timer);
        map = world.getMap();
        /*totalEnergy = 0;
        for (int i = 0; i < screenWidth / 6; i++) {
            for (int j = 0; j < screenHeight / 4; j++) {
                if (map[i][j] != NULL) {
                    totalEnergy += map[i][j]->getEnergy();
                }
            }
        }
        std::cout << totalEnergy << std::endl;*/
        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

        ClearBackground(RAYWHITE);

        for (int i = 0; i < screenWidth / 6; i++) {
            for (int j = 0; j < screenHeight / 4; j++) {
                if (map[i][j] != NULL) {
                    DrawCircle(i * 4, j * 4, 3, BLACK);
                }
            }
        }
        DrawText(TextFormat("Timer: %02i", timer), 820, 100, 20, BLACK);

        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}