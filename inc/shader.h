#ifndef SHADER_H
#define SHADER_H

#include <raylib.h>

enum sLocNames {
    S_VOXDATA,
    S_VPI,
    S_TIME,
    S_RES,
    S_NSTEPS,
    S_DIST,
    S_DENSITY,
    S_HARD,
    S_N_LOCS
};

struct DataSettings {
    Vector3 size;
    float maxValue;
};

extern struct DataSettings dataSettings;
extern float voxScale;

extern void RenderScreenshot();

extern void InitShader(void);
extern void UpdateShader(float delta);
extern void DrawShader(void);
extern void DeInitShader(void);

#endif
