#include "include/raylib.h"
#include "include/rlgl.h"
#include <bits/floatn-common.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <malloc.h>
#include <unistd.h>
#include <time.h>
#include <math.h>
#include <pthread.h>
#include "physics.h"

#define BodyAmount 4
#define VectMultiplier 1e-100
#define HistoryAmount 800
#define ScaleFactor 1.0f
#define RadiusMult 10


void MovePlayer(Camera3D* camera, double curntAngle);
double CurrentAngle(Vector3 pos, Vector3 tar);

double xCurrentAngle(Vector3 pos, Vector3 tar);

Vector3 oldpos[BodyAmount][HistoryAmount] = {0};
uint16_t oldPosSize = 0;
uint16_t speed = UINT16_MAX;
uint16_t p, n;
float currntAngle;

int main(void)
{
    const int screenWidth = 1920;
    const int screenHeight = 1080;
    
    
    InitWindow(screenWidth, screenHeight, "3 Body Problem");
    Body bodies[BodyAmount] = {0};
    double delta_t = 0.5;
    bodies[0].posX = 0; //Sun
    bodies[0].posY = 0;
    bodies[0].posZ = 0;
    
    bodies[0].velX = 0;
    bodies[0].velY = 0;
    bodies[0].velZ = 0;

    bodies[0].accX = 0;
    bodies[0].accY = 0;
    bodies[0].accZ = 0;

    bodies[0].mass = 1.989e30;
    bodies[0].color = YELLOW;
    bodies[0].radius = 695508e3;
    // X =-1.477068139829994E+08 Y = 1.739142536908122E+07 Z =-6.778431043550372E+02
    //VX=-3.971613886377753E+00 VY=-2.970677798637254E+01 VZ= 1.457910991652156E-03
    //Earth
    //X =-1.477068139829994E+08 Y = 1.739142536908122E+07 Z =-6.778431043550372E+02
    //VX=-3.971613886377753E+00 VY=-2.970677798637254E+01 VZ= 1.457910991652156E-03
    bodies[1].posX = -1.477068139829994e11;
    bodies[1].posY = 1.739142536908122e10;
    bodies[1].posZ = -6.778431043550372e05;
    
    bodies[1].velX = -3.971613886377753e03;
    bodies[1].velY = -2.970677798637254e04;
    bodies[1].velZ = 1.457910991652156;
    
    bodies[1].accX = 0;
    bodies[1].accY = 0;
    bodies[1].accZ = 0;

    bodies[1].mass = 5.97219e24;
    bodies[1].color = BLUE;
    bodies[1].radius = 6371e3;
    
 
    //X =-1.473277082011268E+08 Y = 1.725588050527905E+07 Z = 2.977381652378291E+04
    //VX=-3.625876977185716E+00 VY=-2.879309410748038E+01 VZ= 4.626213107348853E-02
    
    //Moon
    //X =-1.473277082011268E+08 Y = 1.725588050527905E+07 Z = 2.977381652378291E+04
    //VX=-3.625876977185716E+00 VY=-2.879309410748038E+01 VZ= 4.626213107348853E-02
    bodies[2].posX = -1.473277082011268e11;
    bodies[2].posY = 1.725588050527905e10;
    bodies[2].posZ = 2.977381652378291e07;
    
    bodies[2].velX = -3.625876977185716e03;
    bodies[2].velY = -2.879309410748038e04;
    bodies[2].velZ = -4.626213107348853e1;
    
    bodies[2].accX = 0;
    bodies[2].accY = 0;
    bodies[2].accZ = 0;

    bodies[2].mass = 7.349e22;
    bodies[2].color = WHITE;
    bodies[2].radius = 1737.5e3 *2;
    //Aphosis 99942
    //X =-1.600850796411153E+08 Y = 7.492778512651158E+06 Z =-4.181932494664246E+06
    //VX= 1.454413661095624E+00 VY=-2.626042109059918E+01 VZ= 1.437005235971867E+00
    
    bodies[3].posX = -1.600850796411153e11; 
    bodies[3].posY = 7.492778512651158e09;
    bodies[3].posZ = -4.181932494664246e09;
   
    bodies[3].velX = 1.454413661095624e03;
    bodies[3].velY = -2.626042109059918e04;
    bodies[3].velZ = 1.437005235971867e03;
    
    bodies[3].accX = 0;
    bodies[3].accY = 0;
    bodies[3].accZ = 0;

    //bodies[2].mass = 6.1e10;
    bodies[3].mass = 0;
    bodies[3].color = GREEN;
    bodies[3].radius = 185;
    
    for (int i = 0; i < BodyAmount; i++) {
        for (int k = 0; k < HistoryAmount; k++) {
            oldpos[i][k].x = bodies[i].posX;
            oldpos[i][k].y = bodies[i].posY;
            oldpos[i][k].z = bodies[i].posZ;
        }
    }

    Camera3D camera = { 0 };
    camera.position = (Vector3){ 100, 0, 50.f };
    camera.target = (Vector3){ 0.0f, 0.0f, 0.0f };
    camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };
    camera.fovy = 80.0f;
    camera.projection = CAMERA_PERSPECTIVE;
    
    SetCameraMode(camera, CAMERA_FREE);
    SetTargetFPS(60);
    ToggleFullscreen();
    double diffX, diffY, diffZ, c;
    
    while (!WindowShouldClose())                // Detect window close button or ESC key
    {
        UpdateCamera(&camera);
        
        currntAngle = CurrentAngle(camera.position, camera.target);

        MovePlayer(&camera, currntAngle);

        speed = speed * IsKeyUp(KEY_I) + 10* (IsKeyDown(KEY_O) && (speed < UINT16_MAX)) -10* (IsKeyDown(KEY_P) && (speed > 0));
        
        for (int p = 0; p < speed; p++){
        // Calculate acceleration or something
            newtons_law_of_grav(BodyAmount, &(bodies[0]));


            for (int i = 0; i < BodyAmount; i++) {
                
                bodies[i].velX += bodies[i].accX * delta_t;
                bodies[i].velY += bodies[i].accY * delta_t;
                bodies[i].velZ += bodies[i].accZ * delta_t;
                
                bodies[i].posX += bodies[i].velX * delta_t + 0.5 * bodies[i].accX * delta_t * delta_t;
                bodies[i].posY += bodies[i].velY * delta_t + 0.5 * bodies[i].accY * delta_t * delta_t;
                bodies[i].posZ += bodies[i].velZ * delta_t + 0.5 * bodies[i].accZ * delta_t * delta_t;

            }
        }

        oldPosSize = (oldPosSize) * (oldPosSize < HistoryAmount) + 1;
        
        for (int i = 0; i < BodyAmount; i++) {
            oldpos[i][oldPosSize-1] = (Vector3){bodies[i].posX, bodies[i].posY, bodies[i].posZ};
        }
        BeginDrawing();

            ClearBackground(BLACK);
            DrawFPS(10, 10);
            DrawText(TextFormat("Speed %d", speed), 10, 40, 10, WHITE);
            DrawText(TextFormat("%f %f %f", bodies[0].posX, bodies[0].posY, bodies[0].posZ), 10, 30, 10, WHITE);
            BeginMode3D(camera);
                //DrawGrid(100, 1);
                rlPushMatrix();
                rlScalef(1e-9,1e-9,1e-9);
                for (int i = 0; i< BodyAmount; i++) {
                    DrawSphere((Vector3) { bodies[i].posX * VectMultiplier, bodies[i].posY * VectMultiplier, bodies[i].posZ * VectMultiplier},bodies[i].radius * RadiusMult, bodies[i].color);
                    p = oldPosSize;
                    n = p;
                    for (uint16_t k = 0; k < HistoryAmount-1; k++) {
                        p = (p+1) * (p < (HistoryAmount-1)) ;
                        DrawLine3D( oldpos[i][n], oldpos[i][p], bodies[i].color);
                        n = p;
                    }
                }
                
                rlPopMatrix();
            EndMode3D();
        EndDrawing();
    }
    CloseWindow();
    return 0;
}
double CurrentAngle(Vector3 pos, Vector3 tar){
    Vector2 temp = (Vector2){ pos.x - tar.x, pos.z - tar.z};
    return atan2(temp.x, temp.y)+PI;
}

void MovePlayer(Camera3D* camera, double curntAngle){

    double xcurntAngle = xCurrentAngle(camera->position, camera->target);

    camera->target.x = camera->target.x + ((ScaleFactor * cos(curntAngle)) * (IsKeyDown(KEY_A) && !IsKeyDown(KEY_D))) + ((- ScaleFactor * cos(curntAngle)) * (IsKeyDown(KEY_D) && !IsKeyDown(KEY_A))) + ((ScaleFactor * sin(curntAngle)) * (IsKeyDown(KEY_W) && !IsKeyDown(KEY_S))) + ((- ScaleFactor * sin(curntAngle)) * (IsKeyDown(KEY_S) && !IsKeyDown(KEY_W)));


    camera->position.x = camera->position.x + (((ScaleFactor * cos(curntAngle)) * (IsKeyDown(KEY_A) && !IsKeyDown(KEY_D))) + (- ScaleFactor * cos(curntAngle)) * (IsKeyDown(KEY_D) && !IsKeyDown(KEY_A))) + ((+ ScaleFactor * sin(curntAngle)) * (IsKeyDown(KEY_W) && !IsKeyDown(KEY_S))) + ((- ScaleFactor * sin(curntAngle)) * (IsKeyDown(KEY_S) && !IsKeyDown(KEY_W)));
    

    camera->position.z = camera->position.z +((- ScaleFactor * sin(curntAngle)) * (IsKeyDown(KEY_A) && !IsKeyDown(KEY_D))) + (+ ScaleFactor * sin(curntAngle)) * (IsKeyDown(KEY_D) && !IsKeyDown(KEY_A)) + ((+ ScaleFactor * sin(curntAngle)) * (IsKeyDown(KEY_W) && !IsKeyDown(KEY_S))) + ((- ScaleFactor * cos(curntAngle)) * (IsKeyDown(KEY_S) && !IsKeyDown(KEY_W)));
    

    camera->target.z = camera->target.z + ((- ScaleFactor * sin(curntAngle)) * (IsKeyDown(KEY_A) && !IsKeyDown(KEY_D))) + ((+ ScaleFactor * sin(curntAngle)) * (IsKeyDown(KEY_D) && !IsKeyDown(KEY_A))) + ((+ ScaleFactor * cos(curntAngle)) * (IsKeyDown(KEY_W) && !IsKeyDown(KEY_S))) + ((- ScaleFactor * cos(curntAngle)) * (IsKeyDown(KEY_S) && !IsKeyDown(KEY_W)));

    camera-> position.y = camera->position.y + (- ScaleFactor * sin(xcurntAngle)) * (IsKeyDown(KEY_W) && !IsKeyDown(KEY_S)) + (+ ScaleFactor * sin(xcurntAngle)) * (!IsKeyDown(KEY_W) && IsKeyDown(KEY_S));

    camera->target.y = camera->target.y + ( - ScaleFactor * sin(xcurntAngle)) * (IsKeyDown(KEY_W) && !IsKeyDown(KEY_S)) + (+ ScaleFactor * sin(xcurntAngle)) * (!IsKeyDown(KEY_W) && IsKeyDown(KEY_S));

}
double xCurrentAngle(Vector3 pos, Vector3 tar){

    return atan((pos.y-tar.y)/(sqrt(pow(pos.x-tar.x, 2) + sqrt(pow(pos.z-tar.z, 2)))));
}
