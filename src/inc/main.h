#ifndef MAIN_H
#define MAIN_H

#include <raylib.h>

#define DRAW_RES 512, 1024
#define GUI_WIDTH 256
#define RENDER_RES 2048, 4096
#define DRAW_NSTEPS 1024
#define RENDER_NSTEPS 8192

#define SIZE 401

#define QUICKLOG TraceLog(LOG_WARNING, "here");

extern Rectangle renderRect;
extern RenderTexture2D render;
extern Camera camera;

extern void InitMain(void);
extern void UpdateMain(float delta);
extern void DrawMain(void);
extern void DeInitMain(void);

#endif
