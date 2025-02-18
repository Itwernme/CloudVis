#ifndef GUI_H
#define GUI_H

#include <raylib.h>

extern Rectangle guiRect;
extern bool isMarkers;
extern bool isOrtho;
extern bool isHardEdge;

extern void InitGUI(void);
extern void UpdateGUI(float delta);
extern void DrawGUI(void);
extern void DeInitGUI(void);

#endif
