#include "shader.h"

#include <string.h>
#include <raylib.h>
#include <stdlib.h>
#include <raymath.h>
#include <time.h>
#include "glad/glad.h"
#include "raylib/rcamera.h"

#include "main.h"
#include "utils.h"
#include "gui.h"

static Shader shader;
static struct {
    int voxData;
    int voxRes;

    int camPos;
    int vpi;
    int renderRes;

    int nSteps;
    int dist;
    int density;
    int hardEdges;
    int hardCubes;
} shaderLocs;
static Texture whiteTex;
static GLuint voxelTexId;

int size;
static float maxVal;

static int readSettings();
static void loadDataToTexture(GLuint id);

void InitShader(){
    Image buf = GenImageColor(DRAW_RES, BLACK);
    whiteTex = LoadTextureFromImage(buf);
    UnloadImage(buf);

    // ----------------------------
    // Load Voxel Data
    // ----------------------------

    gladLoadGL();

    glBindTexture(GL_TEXTURE_2D, 0); // unbind

    glGenTextures(1, &voxelTexId);

    loadDataToTexture(voxelTexId);

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

    shader = LoadShader(NULL, "res/raycast.glsl");//res/rayvert.glsl

    shaderLocs.voxData = GetShaderLocation(shader, "voxelData");
    shaderLocs.voxRes = GetShaderLocation(shader, "voxelRes");

    shaderLocs.camPos = GetShaderLocation(shader, "cameraPos");
    shaderLocs.vpi = GetShaderLocation(shader, "vpi");
    shaderLocs.renderRes = GetShaderLocation(shader, "renderRes");

    shaderLocs.nSteps = GetShaderLocation(shader, "nSteps");
    shaderLocs.dist = GetShaderLocation(shader, "dist");
    shaderLocs.density = GetShaderLocation(shader, "density");
    shaderLocs.hardEdges = GetShaderLocation(shader, "hardEdges");
    shaderLocs.hardCubes = GetShaderLocation(shader, "hardCubes");

    {
    float renderRes[2] = {DRAW_RES};
    SetShaderValue(shader, shaderLocs.renderRes, renderRes, SHADER_UNIFORM_VEC2);
    int voxRes = size;
    SetShaderValue(shader, shaderLocs.voxRes, &voxRes, SHADER_UNIFORM_INT);
    int nSteps = DRAW_NSTEPS;
    SetShaderValue(shader, shaderLocs.nSteps, &nSteps, SHADER_UNIFORM_INT);
    float dist = DIST;
    SetShaderValue(shader, shaderLocs.dist, &dist, SHADER_UNIFORM_FLOAT);

    glUseProgram(shader.id);
    glUniform1i(shaderLocs.voxData, 1);
    // int texPos = 1;
    // SetShaderValue(shader, shaderLocs.voxData, &texPos, SHADER_UNIFORM_INT);
    glActiveTexture(GL_TEXTURE0 + 1);
    glBindTexture(GL_TEXTURE_3D, voxelTexId);
    }
}

void UpdateShader(float delta){
    float cameraPos[3] = {camera.position.x, camera.position.y, camera.position.z};
    SetShaderValue(shader, shader.locs[SHADER_LOC_VECTOR_VIEW], cameraPos, SHADER_UNIFORM_VEC3);

    Matrix view = GetCameraViewMatrix(&camera);
    Matrix proj = GetCameraProjectionMatrix(&camera, (float)whiteTex.width / whiteTex.height);
    Matrix viewProjInv = MatrixInvert(MatrixMultiply(view, proj));
    SetShaderValueMatrix(shader, shaderLocs.vpi, viewProjInv);

    SetShaderValue(shader, shaderLocs.density, &density, SHADER_UNIFORM_FLOAT);
    int hardEdges = isHardEdge;
    SetShaderValue(shader, shaderLocs.hardEdges, &hardEdges, SHADER_UNIFORM_INT);
    int hardCubes = isHardCube;
    SetShaderValue(shader, shaderLocs.hardCubes, &hardCubes, SHADER_UNIFORM_INT);
}

void DrawShader(){
    BeginShaderMode(shader);
        DrawTexture(whiteTex, 0, 0, BLANK);
    EndShaderMode();
}

void DeInitShader(){
    UnloadTexture(whiteTex);
    glDeleteTextures(1, &voxelTexId);
    UnloadShader(shader);
}

void RenderScreenshot(){
    RenderTexture target = LoadRenderTexture(RENDER_RES);

    Image buf = GenImageColor(RENDER_RES, WHITE);
    Texture base = LoadTextureFromImage(buf);
    UnloadImage(buf);

    float newRes[2] = {RENDER_RES};
    SetShaderValue(shader, shaderLocs.renderRes, newRes, SHADER_UNIFORM_VEC2);
    int newNSteps = RENDER_NSTEPS;
    SetShaderValue(shader, shaderLocs.nSteps, &newNSteps, SHADER_UNIFORM_INT);

    Matrix view = GetCameraViewMatrix(&camera);
    Matrix proj = GetCameraProjectionMatrix(&camera, (float)base.width / base.height);
    Matrix viewProjInv = MatrixInvert(MatrixMultiply(view, proj));
    SetShaderValueMatrix(shader, shaderLocs.vpi, viewProjInv);

    BeginTextureMode(target);
        ClearBackground(WHITE);
        BeginMode3D(camera);
            DrawCube((Vector3){5, 0, 0}, 10, 0.05, 0.05, RED);
            DrawCube((Vector3){0, 5, 0}, 0.05, 10, 0.05, GREEN);
            DrawCube((Vector3){0, 0, 5}, 0.05, 0.05, 10, BLUE);
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
    SetShaderValue(shader, shaderLocs.renderRes, oldRes, SHADER_UNIFORM_VEC2);
    int oldNSteps = DRAW_NSTEPS;
    SetShaderValue(shader, shaderLocs.nSteps, &oldNSteps, SHADER_UNIFORM_INT);

    UnloadImage(buf);
}

static int readSettings(){
    char *settings = LoadFileText("res/input/settings.txt");
    if (settings == NULL) return 1;
    char *token = strtok(settings, " ");
    size = TextToInteger(token);
    token = strtok(NULL, " ");
    maxVal = TextToFloat(token);
    if (size <= 0 || maxVal <= 0.0f) return 1;
    return 0;
}

static void loadDataToTexture(GLuint id){
    glBindTexture(GL_TEXTURE_3D, id);
    if (readSettings()) TraceLog(LOG_FATAL, "res/input/settings.txt not found\n\tNo valid settings file found\n\tsettings.txt should be formatted: \"<size>[int] <max value>[float]\" eg. \"401 1300.0\"");

    float *data = malloc(size*size*size*sizeof(float));
    memset(data, 0, size*size*size*sizeof(float));

    float divisor = logf(maxVal+1);
    float maxDataValue = 0.0f;
    // load in data values
    FilePathList paths = LoadDirectoryFiles("res/input/data");
    Sort(paths.paths, paths.count);
    for (int y = 0; y < size; y++) {
        char *csv = LoadFileText(paths.paths[y]);
        char *token = strtok(csv, ",\n");
        for (int x = 0; x < size; x++) {
            for (int z = size-1; z >= 0; z--) {
                int i = x+y*size+z*size*size;
                if (token != NULL) {
                    float value = TextToInteger(token);
                    if (value == 0) data[i] = 0;
                    else data[i] = logf(value+1)/divisor;
                    if (value > maxDataValue) maxDataValue = value;
                    token = strtok(NULL, ",\n");
                }
            }
        };
        UnloadFileText(csv);
    }
    UnloadDirectoryFiles(paths);

    if (maxDataValue > maxVal) TraceLog(LOG_WARNING, "Max value in data %f, exceeds settings max %f", maxDataValue, maxVal);

    glTexImage3D(GL_TEXTURE_3D, 0, GL_R32F, size, size, size, 0, GL_RED, GL_FLOAT, data);

    free(data);
}
