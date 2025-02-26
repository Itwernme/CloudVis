#ifndef UTILS_H
#define UTILS_H

#include <raylib.h>
#include "raylib/rini.h"

extern void MoveCamera(Camera *camera, float delta);
extern void resize();
extern void Sort(char **data, int n);
int getConfigInt(rini_config *config, const char *key, int defaultValue, const char *defaultDesc);
float getConfigFloat(rini_config *config, const char *key, float defaultValue, const char *defaultDesc);

#endif
