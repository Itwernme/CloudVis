#include "inc/gui.h"

#include <raylib.h>
#include <raymath.h>
#define RAYGUI_IMPLEMENTATION
#include "../lib/raygui.h"

#include "inc/shader.h"
#include "inc/main.h"

bool showMarkers;

void InitGUI(void){

}

void UpdateGUI(float delta){

}

void DrawGUI(void){
    if (showMarkers){
        BeginMode3D(camera);
            DrawBoundingBox((BoundingBox){{0, 0, 0},{10, 10, 10}}, RED);

            DrawLine3D((Vector3){0, 0, camera.target.z}, (Vector3){10, 0, camera.target.z}, BLUE);
            DrawLine3D((Vector3){camera.target.x, 0, 0}, (Vector3){camera.target.x, 10, 0}, BLUE);
            DrawLine3D((Vector3){0, camera.target.y, 0}, (Vector3){0, camera.target.y, 10}, BLUE);

            DrawLine3D((Vector3){0, camera.target.y, 0}, (Vector3){10, camera.target.y, 0}, BLUE);
            DrawLine3D((Vector3){0, 0, camera.target.z}, (Vector3){0, 10, camera.target.z}, BLUE);
            DrawLine3D((Vector3){camera.target.x, 0, 0}, (Vector3){camera.target.x, 0, 10}, BLUE);

            DrawLine3D(camera.target, Vector3Add(camera.target, (Vector3){0.1, 0, 0}), RED);
            DrawLine3D(camera.target, Vector3Add(camera.target, (Vector3){0, 0.1, 0}), GREEN);
            DrawLine3D(camera.target, Vector3Add(camera.target, (Vector3){0, 0, 0.1}), BLUE);
        EndMode3D();
    }

    GuiToggle((Rectangle){10, 20, 50, 15}, "markers", &showMarkers);
    if (GuiButton((Rectangle){10, 20+20, 50, 15}, "render")) RenderScreenshot();
}

void DeInitGUI(void){

}
