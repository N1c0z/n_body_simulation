#ifndef PHYSICS_H
#define PHYSICS_H
#include "./include/raylib.h"
#include <stdint.h>

#define G 6.67408e-11

typedef struct Body{
    double posX, posY, posZ, velX, velY, velZ, accX, accY, accZ, mass, radius;
    Color color;
}Body;


void newtons_law_of_grav(uint8_t bodyAmount, Body *bodies);
#endif
