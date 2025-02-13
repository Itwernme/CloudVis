#include "inc/main.h"

#include <raylib.h>
#include <math.h>
#include <raymath.h>

#define RAYGUI_IMPLEMENTATION
#include "../lib/raygui.h"

#include "inc/utils.h"
#include "inc/shader.h"

Rectangle renderRect;
RenderTexture2D render;
Camera camera = {0};

static float avgFPS;

void InitMain(){
    SetTargetFPS(10);
    SetTraceLogLevel(LOG_WARNING);
    render = LoadRenderTexture(RENDER_RES);
    resize();

    /* initialize game */
    camera.position = (Vector3){-10.0f, 10.0f, 0.0f};
    camera.target = (Vector3){5.0f, 5.0f, 5.0f};
    camera.up = (Vector3){0.0f, 1.0f, 0.0f};
    camera.fovy = 45.0f;
    camera.projection = CAMERA_PERSPECTIVE;

    //DisableCursor();
    InitShader();
    /* initialize game */
}

void UpdateMain(float delta){
    if (IsWindowResized()) resize();

    /* update game */
    MoveCamera(&camera, delta);

    UpdateShader(delta);

    avgFPS = (fminf(1.0f/delta, 120) + avgFPS * 9) / 10.0;
    /* update game */
}

void DrawMain(){
    ClearBackground(BLACK);
    BeginTextureMode(render);
        ClearBackground(BLACK);

        /* draw game */
        BeginMode3D(camera);
            //DrawGrid(400, 0.025f);
            DrawLine3D((Vector3){0}, (Vector3){10, 0, 0}, WHITE);
            DrawLine3D((Vector3){0}, (Vector3){0, 10, 0}, WHITE);
            DrawLine3D((Vector3){0}, (Vector3){0, 0, 10}, WHITE);

            DrawLine3D(camera.target, Vector3Add(camera.target, (Vector3){0.1, 0, 0}), RED);
            DrawLine3D(camera.target, Vector3Add(camera.target, (Vector3){0, 0.1, 0}), GREEN);
            DrawLine3D(camera.target, Vector3Add(camera.target, (Vector3){0, 0, 0.1}), BLUE);
        EndMode3D();

        DrawShader();
        /* draw game */

        DrawText(TextFormat("%f", avgFPS), 5, 5, 10, LIME);
    EndTextureMode();

    DrawTexturePro(render.texture, (Rectangle){ 0, 0, render.texture.width, -render.texture.height }, renderRect, (Vector2){0}, 0.0f, WHITE);
}

void DeInitMain(){
    UnloadRenderTexture(render);

    /* de-initialize game */
    DeInitShader();
    /* de-initialize game */
}
