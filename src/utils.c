#include "utils.h"

#include <stdio.h>
#include <raylib.h>
#include <math.h>
#include <string.h>
#include "raylib/rcamera.h"
#include "raylib/rini.h"

#include "main.h"
#include "gui.h"

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
        if (IsKeyDown(KEY_LEFT_SHIFT)){

        } else {
            CameraYaw(camera, -mousePositionDelta.x*0.003f, true);
            CameraPitch(camera, -mousePositionDelta.y*0.003f, true, true, false);
        }
    }

    // Keyboard support
    if (IsKeyDown(KEY_W)) CameraMoveForward(camera, cameraMoveSpeed, true);
    if (IsKeyDown(KEY_A)) CameraMoveRight(camera, -cameraMoveSpeed, true);
    if (IsKeyDown(KEY_S)) CameraMoveForward(camera, -cameraMoveSpeed, true);
    if (IsKeyDown(KEY_D)) CameraMoveRight(camera, cameraMoveSpeed, true);

    if (IsKeyDown(KEY_Q)) CameraMoveUp(camera, cameraMoveSpeed);// && camera->target.y < 10
    if (IsKeyDown(KEY_E)) CameraMoveUp(camera, -cameraMoveSpeed);// && camera->target.y > 0

    // Zoom target distance
    if (camera->projection == CAMERA_ORTHOGRAPHIC){ //camera->projection == CAMERA_ORTHOGRAPHIC
        camera->fovy += -0.6*GetMouseWheelMove();
        if (IsKeyDown(KEY_MINUS)) camera->fovy += cameraZoomSpeed;
        if (IsKeyDown(KEY_EQUAL)) camera->fovy += -cameraZoomSpeed;
    } else {
        CameraMoveToTarget(camera, -0.6*GetMouseWheelMove());
        if (IsKeyDown(KEY_MINUS)) CameraMoveToTarget(camera, cameraZoomSpeed);
        if (IsKeyDown(KEY_EQUAL)) CameraMoveToTarget(camera, -cameraZoomSpeed);
    }
    camera->fovy = fmaxf(camera->fovy, 0.1f);
}

void resize(){
    float renderScaleX = (float)GetScreenWidth() / (render.texture.width + GUI_WIDTH);
    float renderScaleY = (float)GetScreenHeight() / render.texture.height;
    float renderScale = fminf(renderScaleX, renderScaleY);

    renderRect.width = render.texture.width * renderScale;
    renderRect.height = render.texture.height * renderScale;

    renderRect.x = (GetScreenWidth() - renderRect.width + GUI_WIDTH * renderScale) / 2;
    renderRect.y = (GetScreenHeight() - renderRect.height) / 2;

    guiRect.width = GUI_WIDTH * renderScale;
    guiRect.height = render.texture.height * renderScale;

    guiRect.x = (GetScreenWidth() - renderRect.width - GUI_WIDTH * renderScale) / 2;
    guiRect.y = (GetScreenHeight() - renderRect.height) / 2;

    //SetMouseOffset(-renderRect.x, -renderRect.y);
    //SetMouseScale(1.0f/renderScale, 1.0f/renderScale);
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

int getConfigInt(rini_config *config, const char *key, int defaultValue, const char *defaultDesc){
    for (unsigned int i = 0; i < config->count; i++)
    {
        if (strcmp(key, config->values[i].key) == 0) // Key found
        {
            return TextToInteger(config->values[i].text);
        }
    }
    rini_set_config_value(config, key, defaultValue, defaultDesc);
    return defaultValue;
}

// float getConfigFloat(rini_config *config, const char *key, float defaultValue, const char *defaultDesc){
//     for (unsigned int i = 0; i < config->count; i++)
//     {
//         if (strcmp(key, config->values[i].key) == 0) // Key found
//         {
//             return TextToFloat(config->values[i].text);
//         }
//     }
//     rini_set_config_value_text(config, key, TextFormat("%g", defaultValue), defaultDesc);
//     return defaultValue;
// }

float getConfigFloat(rini_config *config, const char *key, float defaultValue, const char *defaultDesc){
    for (unsigned int i = 0; i < config->count; i++)
    {
        if (strcmp(key, config->values[i].key) == 0) // Key found
        {
            return TextToFloat(config->values[i].text);
        }
    }
    char value_text[RINI_MAX_TEXT_SIZE] = { 0 };

    sprintf(value_text, "%i", (int)defaultValue);
    TraceLog(LOG_WARNING, "%s", value_text);

    rini_set_config_value_text(config, key, value_text, defaultDesc);

    return defaultValue;
}
