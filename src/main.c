#include "main.h"

#include <raylib.h>
#include <raymath.h>
#include <math.h>
#define RINI_IMPLEMENTATION
#define RINI_USE_TEXT_QUOTATION_MARKS 0
#include "raylib/rini.h"
#undef RINI_IMPLEMENTATION

#include "utils.h"
#include "shader.h"
#include "gui.h"

Rectangle renderRect;
RenderTexture2D render;
Camera camera = {0};

struct Settings settings;
Vector3 edge, center;

static float avgFPS;

static void loadSettings(){
    rini_config config = rini_load_config(CONF_DIR);
    settings.maxNsteps = getConfigInt(&config, "MaxNSteps", DEF_MAX_NSTEPS, "Max steps for each ray");
    settings.renderW = getConfigInt(&config, "RenderW", DEF_RENDER_RES_W, "Screenshot width");
    settings.renderH = getConfigInt(&config, "RenderH", DEF_RENDER_RES_H, "Screenshot height");
    settings.axisThickness = getConfigFloat(&config, "AxisThickness", DEF_AXIS_THICKNESS, "Axis line thickness");
    settings.gridThickness = getConfigFloat(&config, "GridThickness", DEF_GRID_THICKNESS, "Grid line thickness");
    rini_save_config(config, CONF_DIR);
    rini_unload_config(&config);
}

void InitMain(){
    SetTargetFPS(20);
    SetTraceLogLevel(LOG_WARNING);
    render = LoadRenderTexture(DRAW_RES);
    resize();

    /* initialize game */
    loadSettings();

    InitShader();
    InitGUI();

    edge = Vector3Scale(dataSettings.size, 1.0f / voxScale);
    center = Vector3Scale(edge, 0.5f);

    camera.position = Vector3Add(center, edge);
    camera.target = center;
    camera.up = (Vector3){0.0f, 1.0f, 0.0f};
    camera.fovy = 45.0f;//12
    camera.projection = CAMERA_PERSPECTIVE;
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
    DrawRectangleRec(renderRect, LIGHTGRAY);
    BeginTextureMode(render);
        ClearBackground(LIGHTGRAY);

        /* draw game */
        // BeginMode3D(camera);
        //     DrawLine3D((Vector3){0}, (Vector3){10, 0, 0}, RED);
        //     DrawLine3D((Vector3){0}, (Vector3){0, 10, 0}, GREEN);
        //     DrawLine3D((Vector3){0}, (Vector3){0, 0, 10}, BLUE);
        // EndMode3D();

        DrawShader();

        if (guiData.isMarkers){
            BeginMode3D(camera);
                DrawBoundingBox((BoundingBox){{0, 0, 0}, edge}, BLACK);

                DrawLine3D((Vector3){0, 0, camera.target.z}, (Vector3){edge.x, 0, camera.target.z}, RED);
                DrawLine3D((Vector3){camera.target.x, 0, 0}, (Vector3){camera.target.x, edge.y, 0}, GREEN);
                DrawLine3D((Vector3){0, camera.target.y, 0}, (Vector3){0, camera.target.y, edge.z}, BLUE);

                DrawLine3D((Vector3){0, camera.target.y, 0}, (Vector3){edge.x, camera.target.y, 0}, RED);
                DrawLine3D((Vector3){0, 0, camera.target.z}, (Vector3){0, edge.y, camera.target.z}, GREEN);
                DrawLine3D((Vector3){camera.target.x, 0, 0}, (Vector3){camera.target.x, 0, edge.z}, BLUE);

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
