#include "guiparts.h"
#include "gui.h"

#include <math.h>
#include <raylib.h>
#include <raymath.h>
#define RAYGUI_IMPLEMENTATION
#include "raylib/raygui.h"
#undef RAYGUI_IMPLEMENTATION


int Slider(Rectangle rect, SliderData *data, float min, float max, const char *name){
    int result = 0;
    rect.width *= 2.0f/3.0f;
    GuiSliderBar(rect, "", "", &data->value, min, max);
    GuiDrawText(name, rect, TEXT_ALIGN_CENTER, GRAY);
    rect.x += rect.width; rect.width /= 2;
    if (!data->editing) TextCopy(data->text, TextFormat("%g", data->value));
    if (GuiTextBox(rect, data->text, MAXSTRLEN, data->editing)) {
        data->value = TextToFloat(data->text);
        data->editing = !data->editing;
        result = 1;
    }
    return result;
}

int ValueBox(Rectangle rect, ValueBoxData *data, float min, float max, const char *name){
    int result = 0;
    Rectangle textRect = rect;
    textRect.width = fminf(GetTextWidth(name), rect.width / 3);
    textRect.x -= 5;
    rect.width -= textRect.width;
    rect.x += textRect.width;
    GuiState state = guiState;
    GuiDrawText(name, textRect, TEXT_ALIGN_CENTER, GetColor(GuiGetStyle(LABEL, TEXT + (state*3))));
    GuiDummyRec(rect, NULL);
    if (!data->editing) {
        Clamp(data->value, min, max);
        TextCopy(data->text, TextFormat("%g", data->value));
    }
    if (GuiTextBox(rect, data->text, MAXSTRLEN, data->editing)) {
        data->value = TextToFloat(data->text);
        data->editing = !data->editing;
        result = 1;
    }
    return result;
}
