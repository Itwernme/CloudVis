#include "inc/main.h"

#include <string.h>
#include <raylib.h>
#include <math.h>
#include <stdint.h>
#include <GLES3/gl3.h>
#include <rlgl.h>
#include <stdlib.h>
#include <float.h>
#include <rcamera.h>
#include <raymath.h>

#define SIZE 400

static Rectangle renderRect;
static RenderTexture2D render;
static float avgFPS;

static Camera camera = {0};
static Shader shader;
static int targetPosLoc;
static int voxelDataLoc, vpiLoc;
static Texture whiteTex;
static GLuint voxelTexId;

static void resize(){
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

static void MoveCamera(Camera *camera)
{
    Vector2 mousePositionDelta = GetMouseDelta();

    // Camera speeds based on frame time
    float cameraMoveSpeed = 5.0f*GetFrameTime();
    float cameraRotationSpeed = 0.3f*GetFrameTime();

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
    CameraMoveToTarget(camera, -GetMouseWheelMove());
    if (IsKeyDown(KEY_MINUS)) CameraMoveToTarget(camera, 2.0f);
    if (IsKeyDown(KEY_EQUAL)) CameraMoveToTarget(camera, -2.0f);
}

static void Sort(char **data, int n){
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

static void loadData(){
    glBindTexture(GL_TEXTURE_2D, 0); // unbind

    //glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    glGenTextures(1, &voxelTexId);

    glBindTexture(GL_TEXTURE_3D, voxelTexId);

    //float voxelData [SIZE][SIZE][SIZE];

    float *data = malloc(SIZE*SIZE*SIZE*sizeof(float));
    memset(data, 0, SIZE*SIZE*SIZE*sizeof(float));

    // load in data values
    FilePathList paths = LoadDirectoryFiles("res/data");
    Sort(paths.paths, paths.count);
    for (int y = 0; y < SIZE; y++) {
        char *csv = LoadFileText(paths.paths[y+1]);
        char *value = strtok(csv, ",\n");
        UnloadFileText(csv);
        for (int x = 0; x < SIZE; x++) {
            for (int z = 0; z < SIZE; z++) {
                int i = x+y*SIZE+z*SIZE*SIZE;
                if (value != NULL) {
                    data[i] = TextToFloat(value)/100000;
                    value = strtok(NULL, ",\n");
                }
                //if (y < sin((float)x/100+(float)z/100)*100+100) data[i] = 0.5f;
            }
        }
    }
    UnloadDirectoryFiles(paths);

    // for (int i = 1+SIZE+SIZE*SIZE; i < SIZE+SIZE+SIZE*SIZE; i++) {
    //     data[i] = 0.5f;
    // }

    glTexImage3D(GL_TEXTURE_3D, 0, GL_R32F, SIZE, SIZE, SIZE, 0, GL_RED, GL_FLOAT, data);

    free(data);

    // wrapping
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);       // Set texture to repeat on x-axis
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);       // Set texture to repeat on y-axis
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);       // Set texture to repeat on z-axis

    // Magnification and minification filters
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);  // Alternative: GL_LINEAR
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);  // Alternative: GL_LINEAR

    glBindTexture(GL_TEXTURE_2D, 0); // unbind
}

void InitMain(){
    SetTargetFPS(15);
    SetTraceLogLevel(LOG_WARNING);
    render = LoadRenderTexture(RENDER_RES);
    resize();

    /* initialize game */
    camera.position = (Vector3){0.0f, 10.0f, 10.0f};
    camera.target = (Vector3){0.0f, 10.0f, 0.0f};
    camera.up = (Vector3){0.0f, 1.0f, 0.0f};
    camera.fovy = 45.0f;
    camera.projection = CAMERA_PERSPECTIVE;

    //DisableCursor();

    Image buf = GenImageColor(RENDER_RES, RED);
    whiteTex = LoadTextureFromImage(buf);
    UnloadImage(buf);

    loadData();

    // setup shader
    shader = LoadShader(0, "res/raycast.glsl");

    float resolution[2] = {RENDER_RES};
    SetShaderValue(shader, GetShaderLocation(shader, "resolution"), resolution, SHADER_UNIFORM_VEC2);

    voxelDataLoc = glGetUniformLocation(shader.id, "voxelData");
    glUseProgram(shader.id);
    glUniform1i(voxelDataLoc, 1);
    glActiveTexture(GL_TEXTURE0 + 1);
    glBindTexture(GL_TEXTURE_3D, voxelTexId);

    shader.locs[SHADER_LOC_VECTOR_VIEW] = GetShaderLocation(shader, "cameraPos");
    vpiLoc = glGetUniformLocation(shader.id, "vpi");
    targetPosLoc = GetShaderLocation(shader, "targetPos");

    /* initialize game */
}

void UpdateMain(float delta){
    if (IsWindowResized()) resize();

    /* update game */
    MoveCamera(&camera);

    float cameraPos[3] = {camera.position.x, camera.position.y, camera.position.z};
    float targetPos[3] = {camera.target.x, camera.target.y, camera.target.z};
    SetShaderValue(shader, shader.locs[SHADER_LOC_VECTOR_VIEW], cameraPos, SHADER_UNIFORM_VEC3);
    Matrix view = GetCameraViewMatrix(&camera);
    Matrix proj = GetCameraProjectionMatrix(&camera, (float)whiteTex.width / whiteTex.height);
    Matrix viewProjInv = MatrixInvert(MatrixMultiply(view, proj));
    SetShaderValue(shader, targetPosLoc, targetPos, SHADER_UNIFORM_VEC3);
    SetShaderValueMatrix(shader, vpiLoc, viewProjInv);
    //SetShaderValueMatrix(shader, shader.locs[SHADER_LOC_MATRIX_PROJECTION], GetCameraProjectionMatrix(&camera, 1));//(float)render.texture.width / render.texture.height));

    avgFPS = (fminf(1.0f/delta, 120) + avgFPS * 19) / 20.0;
    /* update game */
}

void DrawMain(){
    ClearBackground(BLACK);
    BeginTextureMode(render);
        ClearBackground(BLACK);

        /* draw game */
        BeginShaderMode(shader);
            DrawTexture(whiteTex, 0, 0, WHITE);
        EndShaderMode();
        // BeginMode3D(camera);
        //     DrawGrid(10, 1.0f);
        // EndMode3D();

        /* draw game */

        DrawText(TextFormat("%f", avgFPS), 5, 5, 10, LIME);
    EndTextureMode();

    DrawTexturePro(render.texture, (Rectangle){ 0, 0, render.texture.width, -render.texture.height }, renderRect, (Vector2){0}, 0.0f, WHITE);
}

void DeInitMain(){
    UnloadRenderTexture(render);

    /* de-initialize game */
    UnloadTexture(whiteTex);
    //UnloadTexture(voxelData);
    glDeleteTextures(1, &voxelTexId);
    UnloadShader(shader);
    /* de-initialize game */
}
