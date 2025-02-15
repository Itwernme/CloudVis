#include "inc/main.h"

#include <raylib.h>
#include <math.h>
#include <raymath.h>

#include "inc/utils.h"
#include "inc/shader.h"
#include "inc/gui.h"

Rectangle renderRect;
RenderTexture2D render;
Camera camera = {0};

static float avgFPS;

void InitMain(){
    SetTargetFPS(10);
    SetTraceLogLevel(LOG_WARNING);
    render = LoadRenderTexture(DRAW_RES);
    resize();

    /* initialize game */
    camera.position = (Vector3){5.0f, 5.0f, 14.0f};
    camera.target = (Vector3){5.0f, 5.0f, 5.0f};
    camera.up = (Vector3){0.0f, 1.0f, 0.0f};
    camera.fovy = 12.0f;
    camera.projection = CAMERA_ORTHOGRAPHIC;

    //DisableCursor();
    InitShader();
    InitGUI();
    /* initialize game */
}

void UpdateMain(float delta){
    if (IsWindowResized()) resize();

    /* update game */
    MoveCamera(&camera, delta);

    UpdateGUI(delta);
    UpdateShader(delta);

    avgFPS = (fminf(1.0f/delta, 120) + avgFPS * 9) / 10.0;
    /* update game */
}

void DrawMain(){
    ClearBackground(WHITE);
    DrawRectangleRec(renderRect, BLACK);
    BeginTextureMode(render);
        ClearBackground(BLACK);

        /* draw game */
        BeginMode3D(camera);
            DrawLine3D((Vector3){0}, (Vector3){10, 0, 0}, RED);
            //DrawCube((Vector3){10, 0, 0}, 0.2, 0.2, 0.2, RED);
            DrawLine3D((Vector3){0}, (Vector3){0, 10, 0}, GREEN);
            DrawLine3D((Vector3){0}, (Vector3){0, 0, 10}, BLUE);
        EndMode3D();

        DrawShader();
        DrawGUI();
        /* draw game */

        DrawText(TextFormat("%05.2f", avgFPS), 5, 5, 10, LIME);
    EndTextureMode();

    DrawTexturePro(render.texture, (Rectangle){ 0, 0, render.texture.width, -render.texture.height }, renderRect, (Vector2){0}, 0.0f, WHITE);
}

void DeInitMain(){
    UnloadRenderTexture(render);

    /* de-initialize game */
    DeInitShader();
    DeInitGUI();
    /* de-initialize game */
}
