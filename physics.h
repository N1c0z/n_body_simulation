#ifndef PHYSICS_H
#define PHYSICS_H
#include "./include/raylib.h"
#include <stdint.h>

#define G 6.67408e-11

typedef struct Vec {
    double x, y, z;
} Vec;

typedef struct Body {
    Vec pos, vel, acc, nAcc;
    double mass, radius;
    Color color;
} Body;

void newtons_law_of_grav(uint8_t bodyAmount, Body *bodies);

#endif
