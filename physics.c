#include "physics.h"
#include <math.h>

void newtons_law_of_grav(uint8_t bodyAmount, Body *bodies ){
    
    double diffX, diffY, diffZ, c;
    for (int i =0; i < bodyAmount; i++) {
        bodies[i].accX = 0;
        bodies[i].accY = 0;
        bodies[i].accZ = 0;
        for (int k = 0; k < bodyAmount; k++){
            
            if (i == k) {
                continue;
            }

            diffX = bodies[i].posX - bodies[k].posX;
            diffY = bodies[i].posY - bodies[k].posY;
            diffZ = bodies[i].posZ - bodies[k].posZ;
            

            c = -( G * bodies[k].mass ) / pow( sqrt((diffX * diffX) + (diffY * diffY) + (diffZ * diffZ)) ,3 );
            

            bodies[i].accX += c * diffX;
            bodies[i].accY += c * diffY;
            bodies[i].accZ += c * diffZ;

        }
    }

}
