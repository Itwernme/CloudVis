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
    SetTargetFPS(20);
    SetTraceLogLevel(LOG_WARNING);
    render = LoadRenderTexture(DRAW_RES);
    resize();

    /* initialize game */
    camera.position = (Vector3){0.0f, 5.0f, 10.0f};
    camera.target = (Vector3){5.0f, 5.0f, 5.0f};
    camera.up = (Vector3){0.0f, 1.0f, 0.0f};
    camera.fovy = 45.0f;//12
    camera.projection = CAMERA_PERSPECTIVE;

    InitShader();
    TraceLog(LOG_DEBUG, "Shader initiated");
    InitGUI();
    /* initialize game */
}

void UpdateMain(float delta){
    if (IsWindowResized()) resize();

    /* update game */
    if (CheckCollisionPointRec(GetMousePosition(), renderRect)) MoveCamera(&camera, delta);

    UpdateGUI(delta);
    UpdateShader(delta);

    avgFPS = (fminf(1.0f/delta, 120) + avgFPS * 9) / 10.0;
    /* update game */
}

void DrawMain(){
    ClearBackground(BLACK);
    DrawRectangleRec(renderRect, GRAY);
    BeginTextureMode(render);
        ClearBackground(GRAY);

        /* draw game */
        BeginMode3D(camera);
            DrawLine3D((Vector3){0}, (Vector3){10, 0, 0}, RED);
            DrawLine3D((Vector3){0}, (Vector3){0, 10, 0}, GREEN);
            DrawLine3D((Vector3){0}, (Vector3){0, 0, 10}, BLUE);
        EndMode3D();

        DrawShader();

        if (isMarkers){
            BeginMode3D(camera);
                DrawBoundingBox((BoundingBox){{0, 0, 0},{10, 10, 10}}, BLACK);

                DrawLine3D((Vector3){0, 0, camera.target.z}, (Vector3){10, 0, camera.target.z}, RED);
                DrawLine3D((Vector3){camera.target.x, 0, 0}, (Vector3){camera.target.x, 10, 0}, GREEN);
                DrawLine3D((Vector3){0, camera.target.y, 0}, (Vector3){0, camera.target.y, 10}, BLUE);

                DrawLine3D((Vector3){0, camera.target.y, 0}, (Vector3){10, camera.target.y, 0}, RED);
                DrawLine3D((Vector3){0, 0, camera.target.z}, (Vector3){0, 10, camera.target.z}, GREEN);
                DrawLine3D((Vector3){camera.target.x, 0, 0}, (Vector3){camera.target.x, 0, 10}, BLUE);

                DrawLine3D(camera.target, Vector3Add(camera.target, (Vector3){0.1, 0, 0}), RED);
                DrawLine3D(camera.target, Vector3Add(camera.target, (Vector3){0, 0.1, 0}), GREEN);
                DrawLine3D(camera.target, Vector3Add(camera.target, (Vector3){0, 0, 0.1}), BLUE);
            EndMode3D();
        }
        /* draw game */

    EndTextureMode();

    DrawTexturePro(render.texture, (Rectangle){ 0, 0, render.texture.width, -render.texture.height }, renderRect, (Vector2){0}, 0.0f, WHITE);
    DrawText(TextFormat("%05.2f", avgFPS), renderRect.x+5, renderRect.y+5, 20, LIME);

    DrawGUI();
}

void DeInitMain(){
    UnloadRenderTexture(render);

    /* de-initialize game */
    DeInitShader();
    DeInitGUI();
    /* de-initialize game */
}
