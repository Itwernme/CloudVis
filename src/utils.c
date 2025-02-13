#include <raylib.h>
#include <math.h>
#include <rcamera.h>
#include <string.h>

#include "inc/main.h"

void MoveCamera(Camera *camera, float delta){
    Vector2 mousePositionDelta = GetMouseDelta();

    // Camera speeds based on frame time
    float cameraMoveSpeed = 1.0f*delta;
    float cameraRotationSpeed = 0.3f*delta;
    float cameraZoomSpeed = 2.0f*delta;

    // Camera rotation
    if (IsKeyDown(KEY_DOWN)) CameraPitch(camera, cameraRotationSpeed, true, true, false);
    if (IsKeyDown(KEY_UP)) CameraPitch(camera, -cameraRotationSpeed, true, true, false);
    if (IsKeyDown(KEY_RIGHT)) CameraYaw(camera, cameraRotationSpeed, true);
    if (IsKeyDown(KEY_LEFT)) CameraYaw(camera, -cameraRotationSpeed, true);

    // Mouse support
    if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)){
        CameraYaw(camera, -mousePositionDelta.x*0.003f, true);
        CameraPitch(camera, -mousePositionDelta.y*0.003f, true, true, false);
    }

    // Keyboard support
    if (IsKeyDown(KEY_W)) CameraMoveForward(camera, cameraMoveSpeed, true);
    if (IsKeyDown(KEY_A)) CameraMoveRight(camera, -cameraMoveSpeed, true);
    if (IsKeyDown(KEY_S)) CameraMoveForward(camera, -cameraMoveSpeed, true);
    if (IsKeyDown(KEY_D)) CameraMoveRight(camera, cameraMoveSpeed, true);

    if (IsKeyDown(KEY_Q)) CameraMoveUp(camera, cameraMoveSpeed);
    if (IsKeyDown(KEY_E)) CameraMoveUp(camera, -cameraMoveSpeed);

    // Zoom target distance
    CameraMoveToTarget(camera, -0.5*GetMouseWheelMove());
    if (IsKeyDown(KEY_MINUS)) CameraMoveToTarget(camera, cameraZoomSpeed);
    if (IsKeyDown(KEY_EQUAL)) CameraMoveToTarget(camera, -cameraZoomSpeed);
}

void resize(){
    float renderScaleX = (float)GetScreenWidth() / render.texture.width;
    float renderScaleY = (float)GetScreenHeight() / render.texture.height;
    float renderScale = fminf(renderScaleX, renderScaleY);

    renderRect.width = render.texture.width * renderScale;
    renderRect.height = render.texture.height * renderScale;

    renderRect.x = (GetScreenWidth() - renderRect.width) / 2;
    renderRect.y = (GetScreenHeight() - renderRect.height) / 2;

    SetMouseOffset(-renderRect.x, -renderRect.y);
    SetMouseScale(1.0f/renderScale, 1.0f/renderScale);
}

void Sort(char **data, int n){
  for (int i = 0; i < (n-1); i++){
  	bool swap = false;
    for (int j = 0; j < ((n-1) - i); j++){
        if (*data[j] == '\0' || (*data[j+1] != '\0' && strcmp(data[j], data[j+1]) > 0)){
        char temp[64];
        strcpy(temp, data[j]);
        strcpy(data[j], data[j+1]);
        strcpy(data[j+1], temp);
        swap = true;
      }
    }
    if (!swap) break;
  }
}
