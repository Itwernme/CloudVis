#include <raylib.h>

#include "main.h"

int main(void)
{
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(GUI_WIDTH * 2 + 2 * DRAW_RES * 2, "");//DRAW_RES
    ChangeDirectory(GetApplicationDirectory());

    BeginDrawing();
    EndDrawing();
    BeginDrawing();
        ClearBackground(BLACK);
        DrawText("LOADING...", GetRenderWidth()/2, GetRenderHeight()/2, 1, WHITE);
    EndDrawing();

    InitMain();

    // Main loop
    while (!WindowShouldClose())
    {
        UpdateMain(GetFrameTime());

        BeginDrawing();
            DrawMain();
        EndDrawing();
    }

    DeInitMain();

    CloseWindow();
    return 0;
}
