#ifndef MAIN_H
#define MAIN_H

#include <raylib.h>

#define DRAW_RES 300, 400
#define GUI_WIDTH 128
#define RENDER_RES 2048, 4096
#define DRAW_NSTEPS 2028
#define RENDER_NSTEPS 4096

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
