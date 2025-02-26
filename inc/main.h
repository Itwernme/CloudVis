#ifndef MAIN_H
#define MAIN_H

#include <raylib.h>

#define DRAW_RES 300, 400
#define GUI_WIDTH 128

#define CONF_DIR TextFormat("%s/config.ini", GetWorkingDirectory())
#define DEF_RENDER_RES_W 2048
#define DEF_RENDER_RES_H 3072
#define DEF_MAX_NSTEPS 2048.0
#define DEF_AXIS_THICKNESS 0.01
#define DEF_GRID_THICKNESS 0.005

#define QUICKLOG TraceLog(LOG_WARNING, "here");

struct Settings {
    int maxNsteps;
    int renderW;
    int renderH;
    float axisThickness;
    float gridThickness;
};

extern Rectangle renderRect;
extern RenderTexture2D render;
extern Camera camera;

extern struct Settings settings;
extern Vector3 edge, center;

extern void InitMain(void);
extern void UpdateMain(float delta);
extern void DrawMain(void);
extern void DeInitMain(void);

#endif
