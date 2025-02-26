#include "gui.h"

#include <raylib.h>
#include <raymath.h>
#include <string.h>
#include <float.h>
#include <sys/types.h>
#include "raylib/raygui.h"
#include "guiparts.h"
#include "raylib/rcamera.h"

#include "shader.h"
#include "main.h"

Rectangle guiRect;

struct GuiData guiData = {0, 0, 1, 0, 0, 1, 0, {0.03f, "", 0}, {0, "", 0}, {FLT_MAX, "", 0}};

static void UpdatePerspective(bool ortho);

void InitGUI(void){
    GuiLoadStyle("res/theme.rgs");
    GuiSetStyle(VALUEBOX, SPINNER_BUTTON_WIDTH, 12);
    GuiSetStyle(VALUEBOX, TEXT_PADDING, 2);
}

void UpdateGUI(float delta){
    if (CheckCollisionPointRec(GetMousePosition(), renderRect)){
        guiData.camPos[0].editing = 0;
        guiData.camPos[1].editing = 0;
        guiData.camPos[2].editing = 0;
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
    GuiToggle(tempA, "markers", &guiData.isMarkers); tempA.x += tempA.width;
    GuiToggle(tempA, "axis", &guiData.isAxis); tempA.x += tempA.width;
    GuiToggle(tempA, "grid", &guiData.isGrid);
    tempB.y += basis.height;
    tempA = tempB;

    tempA.width /= 3;
    GuiToggle(tempA, "hard cube", &guiData.isHardCube); tempA.x += tempA.width;
    GuiToggle(tempA, "hard edge", &guiData.isHardEdge); tempA.x += tempA.width;
    GuiToggle(tempA, "diminishing", &guiData.isDiminish);
    tempB.y += basis.height;
    tempA = tempB;

    tempA.width /= 3;
    if (GuiToggle(tempA, "ortho", &guiData.isOrtho)){
        UpdatePerspective(guiData.isOrtho);
    } tempA.x += tempA.width;
    GuiToggle(tempA, "", NULL); tempA.x += tempA.width;
    GuiToggle(tempA, "", NULL);
    tempB.y += basis.height;
    tempA = tempB;

    tempB.y += basis.height;
    tempA = tempB;

    Slider(tempA, &guiData.density, 0.005f, 0.1f, "density");
    tempB.y += basis.height;
    tempA = tempB;

    Slider(tempA, &guiData.minValue, 0, dataSettings.maxValue, "min value");
    tempB.y += basis.height;
    tempA = tempB;

    Slider(tempA, &guiData.maxValue, 0, dataSettings.maxValue, "max value");
    tempB.y += basis.height;
    tempA = tempB;

    tempB.y += basis.height;
    tempA = tempB;

    if (!guiData.camPos[0].editing && !guiData.camPos[1].editing && !guiData.camPos[2].editing){
        guiData.camPos[0].value = floorf(camera.target.x * voxScale);
        guiData.camPos[1].value = floorf(camera.target.y * voxScale);
        guiData.camPos[2].value = floorf(camera.target.z * voxScale);
    }

    tempA.width /= 3;
    tempA.width -= 10; tempA.x += 10;
    if (ValueBox(tempA, &guiData.camPos[0], 0, dataSettings.size.x, "X")){
        float new = (floorf(guiData.camPos[0].value) + 0.5f) / voxScale;
        camera.position.x += new - camera.target.x;
        camera.target.x = new;
    } tempA.x += tempA.width+10;
    if (ValueBox(tempA, &guiData.camPos[1], 0, dataSettings.size.y, "Y")){
        float new = (floorf(guiData.camPos[1].value) + 0.5f) / voxScale;
        camera.position.y += new - camera.target.y;
        camera.target.y = new;
    } tempA.x += tempA.width+10;
    if (ValueBox(tempA, &guiData.camPos[2], 0, dataSettings.size.z, "Z")){
        float new = (floorf(guiData.camPos[2].value) + 0.5f) / voxScale;
        camera.position.z += new - camera.target.z;
        camera.target.z = new;
    } tempA.x += tempA.width+10;
    tempB.y += basis.height;
    tempA = tempB;

    tempB.y += basis.height;
    tempA = tempB;

    tempA.width /= 3;
    if (GuiButton(tempA, "YZ +")){
        camera.target = center;
        camera.position = (Vector3){0, center.y, center.z};
        UpdatePerspective(true);
    } tempA.x += tempA.width;
    if (GuiButton(tempA, "XZ +")){
        camera.target = center;
        camera.position = (Vector3){center.x-1, 0, center.z};
        CameraPitch(&camera, 10, true, true, false);
        UpdatePerspective(true);
    } tempA.x += tempA.width;
    if (GuiButton(tempA, "XY +")){
        camera.target = center;
        camera.position = (Vector3){center.x, center.y, 0};
        UpdatePerspective(true);
    }
    tempB.y += basis.height;
    tempA = tempB;
    tempA.width /= 3;
    if (GuiButton(tempA, "YZ -")){
        camera.target = center;
        camera.position = (Vector3){edge.x, center.y, center.z};
        UpdatePerspective(true);
    } tempA.x += tempA.width;
    if (GuiButton(tempA, "XZ -")){
        camera.target = center;
        camera.position = (Vector3){center.x+1, edge.y, center.z};
        CameraPitch(&camera, -10, true, true, false);
        UpdatePerspective(true);
    } tempA.x += tempA.width;
    if (GuiButton(tempA, "XY -")){
        camera.target = center;
        camera.position = (Vector3){center.x, center.y, edge.z};
        UpdatePerspective(true);
    }
    tempB.y += basis.height;
    tempA = tempB;

    tempB.y += basis.height;
    tempA = tempB;

    if (GuiButton(tempA, "render")) RenderScreenshot();
    tempB.y += basis.height;
    tempA = tempB;
}

void DeInitGUI(void){

}

static void UpdatePerspective(bool ortho){
    guiData.isOrtho = ortho;
    if (ortho){
        camera.projection = CAMERA_ORTHOGRAPHIC;
        CameraMoveToTarget(&camera, Vector3Length(edge) - Vector3Length(Vector3Subtract(camera.position, camera.target)));
        camera.fovy = 12;
    } else {
        camera.projection = CAMERA_PERSPECTIVE;
        camera.fovy = 45;
    }
}
