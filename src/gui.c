#include "inc/gui.h"

#include <raylib.h>
#include <raymath.h>
#define RAYGUI_IMPLEMENTATION
#include "../lib/raygui.h"
#include "../lib/rcamera.h"

#include "inc/shader.h"
#include "inc/main.h"

Rectangle guiRect;

bool isMarkers = false;
bool isOrtho = false;
bool isHardEdge = true;
float density = 0.5;

static int cam[3];
static bool editCam[3] = {0};

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
    GuiSetStyle(VALUEBOX, TEXT_PADDING, 2);
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

    GuiSliderBar(tempA, "", "", &density, 0, 1);
    GuiDrawText("density", tempA, TEXT_ALIGN_CENTER, GRAY);
    tempB.y += basis.height;
    tempA = tempB;

    if (!editCam[0] && !editCam[1] && !editCam[2]){
        cam[0] = ((float)camera.target.x * SIZE) / 10.0;
        cam[1] = ((float)camera.target.y * SIZE) / 10.0;
        cam[2] = ((float)camera.target.z * SIZE) / 10.0;
    }

    tempA.width /= 3;
    tempA.width -= 10; tempA.x += 10;
    if (GuiValueBox(tempA, "X", &cam[0], 0, SIZE, editCam[0])){
        editCam[0] = !editCam[0];
        float new = ((float)cam[0] * 10.0) / SIZE;
        camera.position.x += new - camera.target.x;
        camera.target.x = new;
    } tempA.x += tempA.width+10;
    if (GuiValueBox(tempA, "Y", &cam[1], 0, SIZE, editCam[1])){
        editCam[1] = !editCam[1];
        float new = ((float)cam[1] * 10.0) / SIZE;
        camera.position.y += new - camera.target.y;
        camera.target.y = new;
    } tempA.x += tempA.width+10;
    if (GuiValueBox(tempA, "Z", &cam[2], 0, SIZE, editCam[2])){
        editCam[2] = !editCam[2];
        float new = ((float)cam[2] * 10.0) / SIZE;
        camera.position.z += new - camera.target.z;
        camera.target.z = new;
    } tempA.x += tempA.width+10;

    tempB.y += basis.height;
    tempA = tempB;

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
