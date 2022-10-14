#include "physics.h"
#include <math.h>

void newtons_law_of_grav(uint8_t bodyAmount, Body *bodies ){
    
    double diffX, diffY, diffZ, c;
    for (int i =0; i < bodyAmount; i++) {
        bodies[i].acc.x = 0;
        bodies[i].acc.y = 0;
        bodies[i].acc.z = 0;
        for (int k = 0; k < bodyAmount; k++){
            
            if (i == k) {
                continue;
            }

            diffX = bodies[i].pos.x - bodies[k].pos.x;
            diffY = bodies[i].pos.y - bodies[k].pos.y;
            diffZ = bodies[i].pos.z - bodies[k].pos.z;
            

            c = -( G * bodies[k].mass ) / pow( sqrt((diffX * diffX) + (diffY * diffY) + (diffZ * diffZ)) ,3 );
            

            bodies[i].acc.x += c * diffX;
            bodies[i].acc.y += c * diffY;
            bodies[i].acc.z += c * diffZ;

        }
    }

}
