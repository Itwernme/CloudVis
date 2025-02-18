#include "inc/gui.h"

#include <raylib.h>
#include <raymath.h>
#define RAYGUI_IMPLEMENTATION
#include "../lib/raygui.h"
#include <rcamera.h>

#include "inc/shader.h"
#include "inc/main.h"

Rectangle guiRect;

bool isMarkers;
bool isOrtho;
bool isHardEdge;

static int cam[3];
static bool editCam[3];

static void UpdatePerspective(bool ortho){
    isOrtho = ortho;
    if (ortho){
        camera.projection = CAMERA_ORTHOGRAPHIC;
        CameraMoveToTarget(&camera, 15 - Vector3Length(Vector3Subtract(camera.position, camera.target)));
        camera.fovy = 12;
    } else {
        camera.projection = CAMERA_PERSPECTIVE;
        camera.fovy = 45;
    }
}

void InitGUI(void){
    GuiLoadStyle("res/style_dark.rgs");
    GuiSetStyle(VALUEBOX, SPINNER_BUTTON_WIDTH, 12);
}

void UpdateGUI(float delta){
    if (CheckCollisionPointRec(GetMousePosition(), renderRect)){
        editCam[0] = 0; editCam[1] = 0; editCam[2] = 0;
    }
}

void DrawGUI(void){
    GuiPanel(guiRect, "CloudVis - Bruce Copeman");
    Rectangle basis = guiRect;
    basis.height -= 25; basis.y += 25;
    basis.height /= 32;
    Rectangle tempA = basis;
    Rectangle tempB = basis;

    tempA.width /= 3;
    GuiToggle(tempA, "markers", &isMarkers); tempA.x += tempA.width;
    if (GuiToggle(tempA, "ortho", &isOrtho)){
        UpdatePerspective(isOrtho);
    } tempA.x += tempA.width;
    GuiToggle(tempA, "hard edge", &isHardEdge);
    tempB.y += basis.height;
    tempA = tempB;

    if (GuiButton(tempA, "render")) RenderScreenshot();
    tempB.y += basis.height;
    tempA = tempB;

    GuiSliderBar(tempA, "", "", &density, 2, 100);
    GuiDrawText("density", tempA, TEXT_ALIGN_CENTER, GRAY);
    tempB.y += basis.height;
    tempA = tempB;

    if (!editCam[0] && !editCam[1] && !editCam[2]){
        cam[0] = camera.target.x / 10 * SIZE;
        cam[1] = camera.target.y / 10 * SIZE;
        cam[2] = camera.target.z / 10 * SIZE;
    }

    tempA.width /= 3;
    tempA.width -= 10; tempA.x += 10;
    if (GuiSpinner(tempA, "X", &cam[0], 0, SIZE, editCam[0])) editCam[0] = !editCam[0]; tempA.x += tempA.width+10;
    if (GuiSpinner(tempA, "Y", &cam[1], 0, SIZE, editCam[1])) editCam[1] = !editCam[1]; tempA.x += tempA.width+10;
    if (GuiSpinner(tempA, "Z", &cam[2], 0, SIZE, editCam[2])) editCam[2] = !editCam[2]; tempA.x += tempA.width+10;

    tempB.y += basis.height;
    tempA = tempB;

    if (!editCam[0] && !editCam[1] && !editCam[2]){
        Vector3 newTarget = (Vector3){(double)cam[0] / SIZE * 10, (double)cam[1] / SIZE * 10, (double)cam[2] / SIZE * 10};
        camera.position = Vector3Add(camera.position, Vector3Subtract(newTarget, camera.target));
        camera.target = newTarget;
    }

    tempA.width /= 3;
    if (GuiButton(tempA, "X +")){
        camera.target = (Vector3){5, 5, 5};
        camera.position = (Vector3){-1, 5, 5};
        UpdatePerspective(true);
    } tempA.x += tempA.width;
    if (GuiButton(tempA, "Y +")){
        camera.target = (Vector3){5, 5, 5};
        camera.position = (Vector3){6, -1, 5};
        CameraPitch(&camera, 10, true, true, false);
        UpdatePerspective(true);
    } tempA.x += tempA.width;
    if (GuiButton(tempA, "Z +")){
        camera.target = (Vector3){5, 5, 5};
        camera.position = (Vector3){5, 5, -1};
        UpdatePerspective(true);
    }
    tempB.y += basis.height;
    tempA = tempB;
    tempA.width /= 3;
    if (GuiButton(tempA, "X -")){
        camera.target = (Vector3){5, 5, 5};
        camera.position = (Vector3){11, 5, 5};
        UpdatePerspective(true);
    } tempA.x += tempA.width;
    if (GuiButton(tempA, "Y -")){
        camera.target = (Vector3){5, 5, 5};
        camera.position = (Vector3){6, 11, 5};
        CameraPitch(&camera, -10, true, true, false);
        UpdatePerspective(true);
    } tempA.x += tempA.width;
    if (GuiButton(tempA, "Z -")){
        camera.target = (Vector3){5, 5, 5};
        camera.position = (Vector3){5, 5, 11};
        UpdatePerspective(true);
    }
    tempB.y += basis.height;
    tempA = tempB;
}

void DeInitGUI(void){

}
