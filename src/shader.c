#include <string.h>
#include <raylib.h>
#include <GLES3/gl3.h>
#include <stdlib.h>
#include <rcamera.h>
#include <raymath.h>

#include "inc/main.h"
#include "inc/utils.h"

static Shader shader;
static int voxelDataLoc, vpiLoc, timeLoc;
static Texture whiteTex;
static GLuint voxelTexId;

void InitShader(){
    Image buf = GenImageColor(RENDER_RES, RED);
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
            for (int z = 0; z < SIZE; z++) {
                int i = x+y*SIZE+z*SIZE*SIZE;
                if (token != NULL) {
                    float value = TextToInteger(token);
                    if (value == 0) data[i] = 0;
                    else data[i] = (1.0f/value)*1300;
                    token = strtok(NULL, ",\n");
                }
            }
        }
    }
    UnloadDirectoryFiles(paths);

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

    // ----------------------------
    // Load Shader
    // ----------------------------

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
    timeLoc = glGetUniformLocation(shader.id, "time");
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
}

void DrawShader(){
    BeginShaderMode(shader);
        DrawTexture(whiteTex, 0, 0, WHITE);
    EndShaderMode();
}

void DeInitShader(){
    UnloadTexture(whiteTex);
    glDeleteTextures(1, &voxelTexId);
    UnloadShader(shader);
}
