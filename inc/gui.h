#ifndef GUI_H
#define GUI_H

#include <raylib.h>
#include "guiparts.h"

struct GuiData{
    bool isMarkers;
    bool isOrtho;
    bool isHardEdge;
    bool isHardCube;
    bool isDiminish;
    bool isAxis;
    bool isGrid;
    SliderData density;
    SliderData minValue;
    SliderData maxValue;
    ValueBoxData camPos[3];
};

extern struct GuiData guiData;
extern Rectangle guiRect;

extern void InitGUI(void);
extern void UpdateGUI(float delta);
extern void DrawGUI(void);
extern void DeInitGUI(void);

#endif
