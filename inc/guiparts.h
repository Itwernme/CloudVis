#ifndef GUI_PARTS_H
#define GUI_PARTS_H

#include <raylib.h>

#define MAXSTRLEN 16

typedef struct {
    float value;
    char text[MAXSTRLEN+1];
    bool editing;
} ValueBoxData;

typedef ValueBoxData SliderData;

typedef struct {
    bool value;
    const char *text;
} ToggleData;

int Slider(Rectangle rect, SliderData *data, float min, float max, const char *name);
int ValueBox(Rectangle rect, ValueBoxData *data, float min, float max, const char *name);

#endif
