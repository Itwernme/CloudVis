#ifndef MAIN_H
#define MAIN_H

#include <raylib.h>

#define RENDER_RES 720, 576
#define SIZE 401

extern Rectangle renderRect;
extern RenderTexture2D render;
extern Camera camera;

extern void InitMain(void);
extern void UpdateMain(float delta);
extern void DrawMain(void);
extern void DeInitMain(void);

#endif
