#include "shader.h"

#include <string.h>
#include <raylib.h>
#include <stdlib.h>
#include <raymath.h>
#include <time.h>
#include "glad/glad.h"
#include "raylib/rcamera.h"
#include "raylib/rini.h"

#include "main.h"
#include "utils.h"
#include "gui.h"

static Shader shader;
static struct {
    int voxData;
    int voxRes;
    int voxScale;

    int camPos;
    int vpi;
    int renderRes;

    int nSteps;
    int density;
    int diminish;
    int hardEdges;
    int hardCubes;

    int minValue;
    int maxValue;

    int grid;
    int axis;
    int axisThickness;
    int gridThickness;
    int gridScale;
} shaderLocs;
static Texture whiteTex;
static GLuint voxelTexId;

struct DataSettings dataSettings;
float voxScale;

static void loadDataToTexture(GLuint id);
static void loadDataSettings();

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

    loadDataSettings();
    voxScale = Vector3Length(Vector3Scale(dataSettings.size, 0.1f));

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
    shaderLocs.voxScale = GetShaderLocation(shader, "voxelScale");

    shaderLocs.camPos = GetShaderLocation(shader, "cameraPos");
    shaderLocs.vpi = GetShaderLocation(shader, "vpi");
    shaderLocs.renderRes = GetShaderLocation(shader, "renderRes");

    shaderLocs.nSteps = GetShaderLocation(shader, "nSteps");
    shaderLocs.density = GetShaderLocation(shader, "density");
    shaderLocs.diminish = GetShaderLocation(shader, "diminish");
    shaderLocs.hardEdges = GetShaderLocation(shader, "hardEdges");
    shaderLocs.hardCubes = GetShaderLocation(shader, "hardCubes");

    shaderLocs.minValue = GetShaderLocation(shader, "minValue");
    shaderLocs.maxValue = GetShaderLocation(shader, "maxValue");

    shaderLocs.grid = GetShaderLocation(shader, "grid");
    shaderLocs.axis = GetShaderLocation(shader, "axis");
    shaderLocs.axisThickness = GetShaderLocation(shader, "axisThickness");
    shaderLocs.gridThickness = GetShaderLocation(shader, "gridThickness");
    shaderLocs.gridScale = GetShaderLocation(shader, "gridScale");

    float renderRes[2] = {DRAW_RES};
    SetShaderValue(shader, shaderLocs.renderRes, renderRes, SHADER_UNIFORM_VEC2);
    SetShaderValue(shader, shaderLocs.voxRes, &dataSettings.size, SHADER_UNIFORM_VEC3);
    SetShaderValue(shader, shaderLocs.nSteps, &settings.maxNsteps, SHADER_UNIFORM_INT);
    SetShaderValue(shader, shaderLocs.voxScale, &voxScale, SHADER_UNIFORM_FLOAT);
    SetShaderValue(shader, shaderLocs.axisThickness, &settings.axisThickness, SHADER_UNIFORM_FLOAT);
    SetShaderValue(shader, shaderLocs.gridThickness, &settings.gridThickness, SHADER_UNIFORM_FLOAT);

    glUseProgram(shader.id);
    glUniform1i(shaderLocs.voxData, 1);
    glActiveTexture(GL_TEXTURE0 + 1);
    glBindTexture(GL_TEXTURE_3D, voxelTexId);

}

void UpdateShader(float delta){
    float cameraPos[3] = {camera.position.x, camera.position.y, camera.position.z};
    SetShaderValue(shader, shader.locs[SHADER_LOC_VECTOR_VIEW], cameraPos, SHADER_UNIFORM_VEC3);

    Matrix view = GetCameraViewMatrix(&camera);
    Matrix proj = GetCameraProjectionMatrix(&camera, (float)whiteTex.width / whiteTex.height);
    Matrix viewProjInv = MatrixInvert(MatrixMultiply(view, proj));
    SetShaderValueMatrix(shader, shaderLocs.vpi, viewProjInv);

    float fBuf;
    int iBuf;
    fBuf = guiData.density.value;
    SetShaderValue(shader, shaderLocs.density, &fBuf, SHADER_UNIFORM_FLOAT);
    fBuf = guiData.minValue.value / dataSettings.maxValue;
    SetShaderValue(shader, shaderLocs.minValue, &fBuf, SHADER_UNIFORM_FLOAT);
    fBuf = guiData.maxValue.value / dataSettings.maxValue;
    SetShaderValue(shader, shaderLocs.maxValue, &fBuf, SHADER_UNIFORM_FLOAT);
    iBuf = guiData.isHardEdge;
    SetShaderValue(shader, shaderLocs.hardEdges, &iBuf, SHADER_UNIFORM_INT);
    iBuf = guiData.isHardCube;
    SetShaderValue(shader, shaderLocs.hardCubes, &iBuf, SHADER_UNIFORM_INT);
    iBuf = guiData.isDiminish;
    SetShaderValue(shader, shaderLocs.diminish, &iBuf, SHADER_UNIFORM_INT);
    iBuf = guiData.isAxis;
    SetShaderValue(shader, shaderLocs.axis, &iBuf, SHADER_UNIFORM_INT);
    iBuf = guiData.isGrid;
    SetShaderValue(shader, shaderLocs.grid, &iBuf, SHADER_UNIFORM_INT);
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
    RenderTexture target = LoadRenderTexture(settings.renderW, settings.renderH);

    Image buf = GenImageColor(settings.renderW, settings.renderH, WHITE);
    Texture base = LoadTextureFromImage(buf);
    UnloadImage(buf);

    float newRes[2] = {settings.renderW, settings.renderH};
    SetShaderValue(shader, shaderLocs.renderRes, newRes, SHADER_UNIFORM_VEC2);

    Matrix view = GetCameraViewMatrix(&camera);
    Matrix proj = GetCameraProjectionMatrix(&camera, (float)base.width / base.height);
    Matrix viewProjInv = MatrixInvert(MatrixMultiply(view, proj));
    SetShaderValueMatrix(shader, shaderLocs.vpi, viewProjInv);

    BeginTextureMode(target);
        ClearBackground(WHITE);
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

    UnloadImage(buf);
}

static void loadDataSettings(){
    rini_config config = rini_load_config("res/input/settings.txt");
    dataSettings.maxValue = getConfigFloat(&config, "MaxValue", 0, "Max steps for each ray");
    dataSettings.size.x = getConfigInt(&config, "ResX", 0, "Number of points per column");
    dataSettings.size.y = getConfigInt(&config, "ResY", 0, "Number of files");
    dataSettings.size.z = getConfigInt(&config, "ResZ", 0, "Number of columns per file");
    rini_save_config(config, "res/input/settings.txt");
    if (dataSettings.maxValue <= 0 || dataSettings.size.x <= 0 || dataSettings.size.y <= 0 || dataSettings.size.z <= 0) TraceLog(LOG_FATAL, "Invalid settings file");
    rini_unload_config(&config);
}

static void loadDataToTexture(GLuint id){
    glBindTexture(GL_TEXTURE_3D, id);

    int dataSize = dataSettings.size.x*dataSettings.size.y*dataSettings.size.z;
    float *data = malloc(dataSize*sizeof(float));
    // memset(data, 0, dataSize*sizeof(float));

    float divisor = dataSettings.maxValue; //logf(+1)
    int sizex = dataSettings.size.x,
        sizey = dataSettings.size.y,
        sizez = dataSettings.size.z;
    float maxDataValue = 0.0f;
    // load in data values
    FilePathList paths = LoadDirectoryFiles("res/input/data");
    Sort(paths.paths, paths.count);
    for (int y = 0; y < sizey; y++) {
        char *csv = LoadFileText(paths.paths[y]);
        char *token = strtok(csv, ",\n");
        for (int x = 0; x < sizex; x++) {
            for (int z = sizez-1; z >= 0; z--) {
                int i = x+y*sizex+z*sizex*sizey;
                if (token != NULL) {
                    float value = TextToFloat(token);
                    if (value == 0) data[i] = 0;
                    else data[i] = 1-value/divisor; //logf(+1)
                    if (value > maxDataValue) maxDataValue = value;
                    token = strtok(NULL, ",\n");
                }
            }
        };
        UnloadFileText(csv);
    }
    UnloadDirectoryFiles(paths);

    if (maxDataValue > dataSettings.maxValue) TraceLog(LOG_WARNING, "Max value in data %f, exceeds settings max %f", maxDataValue, dataSettings.maxValue);

    glTexImage3D(GL_TEXTURE_3D, 0, GL_R32F, dataSettings.size.x, dataSettings.size.y, dataSettings.size.z, 0, GL_RED, GL_FLOAT, data);

    free(data);
}
