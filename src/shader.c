#include "inc/shader.h"

#include <string.h>
#include <raylib.h>
#include <GLES3/gl3.h>
#include <stdlib.h>
#include <rcamera.h>
#include <raymath.h>
#include <time.h>

#include "inc/main.h"
#include "inc/utils.h"
#include "inc/gui.h"

static Shader shader;
static int voxelDataLoc, vpiLoc, timeLoc, resLoc, nStepsLoc, distLoc, densityLoc, hardLoc;
static Texture whiteTex;
static GLuint voxelTexId;

float density = 40;

void InitShader(){
    Image buf = GenImageColor(DRAW_RES, BLACK);
    whiteTex = LoadTextureFromImage(buf);
    UnloadImage(buf);

    // ----------------------------
    // Load Voxel Data
    // ----------------------------

    glBindTexture(GL_TEXTURE_2D, 0); // unbind

    //glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    glGenTextures(1, &voxelTexId);

    glBindTexture(GL_TEXTURE_3D, voxelTexId);

    float *data = malloc(SIZE*SIZE*SIZE*sizeof(float));
    memset(data, 0, SIZE*SIZE*SIZE*sizeof(float));

    // load in data values

    FilePathList paths = LoadDirectoryFiles("res/data");
    Sort(paths.paths, paths.count);
    for (int y = 0; y < SIZE; y++) {
        char *csv = LoadFileText(paths.paths[y]);
        char *token = strtok(csv, ",\n");
        UnloadFileText(csv);
        for (int x = 0; x < SIZE; x++) {
            for (int z = SIZE-1; z >= 0; z--) {
                int i = x+y*SIZE+z*SIZE*SIZE;
                if (token != NULL) {
                    float value = TextToInteger(token);
                    if (value == 0) data[i] = 0;
                    else data[i] = logf(1300.0f/value+1)*1.44f;
                    token = strtok(NULL, ",\n");
                }
            }
        }
    }
    UnloadDirectoryFiles(paths);

    glTexImage3D(GL_TEXTURE_3D, 0, GL_R32F, SIZE, SIZE, SIZE, 0, GL_RED, GL_FLOAT, data);

    free(data);

    // wrapping
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);       // Set texture to repeat on x-axis : GL_CLAMP_TO_EDGE
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);       // Set texture to repeat on y-axis : GL_REPEAT
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);       // Set texture to repeat on z-axis : GL_MIRRORED_REPEAT

    // Magnification and minification filters
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);  // Alternative: GL_LINEAR
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);  // Alternative: GL_LINEAR

    glBindTexture(GL_TEXTURE_2D, 0); // unbind

    // ----------------------------
    // Load Shader
    // ----------------------------

    shader = LoadShader("", "res/raycast.glsl");//res/rayvert.glsl

    resLoc = GetShaderLocation(shader, "resolution");
    voxelDataLoc = glGetUniformLocation(shader.id, "voxelData");
    shader.locs[SHADER_LOC_VECTOR_VIEW] = GetShaderLocation(shader, "cameraPos");
    vpiLoc = glGetUniformLocation(shader.id, "vpi");
    timeLoc = glGetUniformLocation(shader.id, "time");
    nStepsLoc = glGetUniformLocation(shader.id, "nSteps");
    distLoc = glGetUniformLocation(shader.id, "dist");
    densityLoc = glGetUniformLocation(shader.id, "density");
    hardLoc = glGetUniformLocation(shader.id, "hard");

    float resolution[2] = {DRAW_RES};
    SetShaderValue(shader, resLoc, resolution, SHADER_UNIFORM_VEC2);

    glUseProgram(shader.id);
    glUniform1i(voxelDataLoc, 1);
    glActiveTexture(GL_TEXTURE0 + 1);
    glBindTexture(GL_TEXTURE_3D, voxelTexId);
}

void UpdateShader(float delta){
    float cameraPos[3] = {camera.position.x, camera.position.y, camera.position.z};
    SetShaderValue(shader, shader.locs[SHADER_LOC_VECTOR_VIEW], cameraPos, SHADER_UNIFORM_VEC3);

    Matrix view = GetCameraViewMatrix(&camera);
    Matrix proj = GetCameraProjectionMatrix(&camera, (float)whiteTex.width / whiteTex.height);
    Matrix viewProjInv = MatrixInvert(MatrixMultiply(view, proj));
    SetShaderValueMatrix(shader, vpiLoc, viewProjInv);

    float time = GetTime();
    SetShaderValue(shader, timeLoc, &time, SHADER_UNIFORM_FLOAT);

    SetShaderValue(shader, densityLoc, &density, SHADER_UNIFORM_FLOAT);
    int hard = isHardEdge;
    SetShaderValue(shader, hardLoc, &hard, SHADER_UNIFORM_INT);
}

void DrawShader(){
    BeginShaderMode(shader);
        DrawTexture(whiteTex, 0, 0, BLANK);
    EndShaderMode();
}

void RenderScreenshot(){
    RenderTexture target = LoadRenderTexture(RENDER_RES);

    Image buf = GenImageColor(RENDER_RES, WHITE);
    Texture base = LoadTextureFromImage(buf);
    UnloadImage(buf);

    float newRes[2] = {RENDER_RES};
    SetShaderValue(shader, resLoc, newRes, SHADER_UNIFORM_VEC2);
    int newNSteps = RENDER_NSTEPS;
    SetShaderValue(shader, nStepsLoc, &newNSteps, SHADER_UNIFORM_INT);

    Matrix view = GetCameraViewMatrix(&camera);
    Matrix proj = GetCameraProjectionMatrix(&camera, (float)base.width / base.height);
    Matrix viewProjInv = MatrixInvert(MatrixMultiply(view, proj));
    SetShaderValueMatrix(shader, vpiLoc, viewProjInv);

    BeginTextureMode(target);
        ClearBackground(WHITE);
        BeginMode3D(camera);
            DrawLine3D((Vector3){0}, (Vector3){10, 0, 0}, RED);
            DrawLine3D((Vector3){0}, (Vector3){0, 10, 0}, GREEN);
            DrawLine3D((Vector3){0}, (Vector3){0, 0, 10}, BLUE);
        EndMode3D();
        BeginShaderMode(shader);
            DrawTexture(base, 0, 0, WHITE);
        EndShaderMode();
    EndTextureMode();

    buf = LoadImageFromTexture(target.texture);
    ImageFormat(&buf, PIXELFORMAT_UNCOMPRESSED_R8G8B8);
    ImageFlipVertical(&buf);

    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    ExportImage(buf, TextFormat("render(%02d.%02d.%d %02d.%02d.%02d).png", tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900, tm.tm_hour, tm.tm_min, tm.tm_sec));

    float oldRes[2] = {DRAW_RES};
    SetShaderValue(shader, resLoc, oldRes, SHADER_UNIFORM_VEC2);
    int oldNSteps = DRAW_NSTEPS;
    SetShaderValue(shader, nStepsLoc, &oldNSteps, SHADER_UNIFORM_INT);

    UnloadImage(buf);
}

void DeInitShader(){
    UnloadTexture(whiteTex);
    glDeleteTextures(1, &voxelTexId);
    UnloadShader(shader);
}
