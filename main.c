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

#define BODY_AMOUNT 4
#define VECT_MULTIPLIER 1e-100
#define HISTORY_AMOUNT 800
#define SCALE_FACTOR 1.0f
#define RADIUS_MULTIPLIER 10

void body_init(Body *body, Vec pos, Vec vel, double mass, double radius, Color color);
void move_player(Camera3D* camera, double curntAngle);
double current_angle(Vector3 pos, Vector3 tar);
double current_angle2(Vector3 pos, Vector3 tar);

int main(void)
{
    const int screenWidth = 1920;
    const int screenHeight = 1080;
    
    InitWindow(screenWidth, screenHeight, "3 Body Problem");

    Vector3 oldpos[BODY_AMOUNT][HISTORY_AMOUNT] = {0};
    uint16_t oldPosSize = 0;
    uint16_t speed = UINT16_MAX;
    uint16_t p, n;
    float currentAngle;
    double deltaT = 0.5;
    Body bodies[BODY_AMOUNT] = {0};

    //Initial conditions

    //sun
    body_init(&(bodies[0]), (Vec) {0, 0, 0}, (Vec) {0, 0, 0}, 1.989e30, 695508e3, YELLOW);

    //earth
    body_init(&(bodies[1]), (Vec) {-1.477068139829994e11, 1.739142536908122e10, -6.778431043550372e05}, (Vec) {-3.971613886377753e03, -2.970677798637254e04, 1.457910991652156}, 5.97219e24, 6371e3, BLUE);

    //moon
    body_init(&(bodies[2]), (Vec) {-1.473277082011268e11, 1.725588050527905e10, 2.977381652378291e07}, (Vec) {-3.625876977185716e03, -2.879309410748038e04, -4.626213107348853e1}, 7.349e22, 1737.5e3, WHITE);

    //aphosis 99942
    body_init(&(bodies[3]), (Vec) {-1.600850796411153e11, 7.492778512651158e09, -4.181932494664246e09}, (Vec) {1.454413661095624e03, -2.626042109059918e04, 1.437005235971867e03},0, 185, GREEN);

    
    for (int i = 0; i < BODY_AMOUNT; i++) {
        for (int k = 0; k < HISTORY_AMOUNT; k++) {
            oldpos[i][k].x = bodies[i].pos.x;
            oldpos[i][k].y = bodies[i].pos.y;
            oldpos[i][k].z = bodies[i].pos.z;
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
    
    while (!WindowShouldClose())                // Detect window close button or ESC key
    {
        UpdateCamera(&camera);
        
        currentAngle = current_angle(camera.position, camera.target);

        move_player(&camera, currentAngle);

        speed = speed * IsKeyUp(KEY_I) + 10* (IsKeyDown(KEY_O) && (speed < UINT16_MAX)) -10* (IsKeyDown(KEY_P) && (speed > 0));
        
        for (int p = 0; p < speed; p++){
        // Calculate acceleration or something
            newtons_law_of_grav(BODY_AMOUNT, &(bodies[0]));


            for (int i = 0; i < BODY_AMOUNT; i++) {
                
                bodies[i].vel.x += bodies[i].acc.x * deltaT;
                bodies[i].vel.y += bodies[i].acc.y * deltaT;
                bodies[i].vel.z += bodies[i].acc.z * deltaT;
                
                bodies[i].pos.x += bodies[i].vel.x * deltaT + 0.5 * bodies[i].acc.x * deltaT * deltaT;
                bodies[i].pos.y += bodies[i].vel.y * deltaT + 0.5 * bodies[i].acc.y * deltaT * deltaT;
                bodies[i].pos.z += bodies[i].vel.z * deltaT + 0.5 * bodies[i].acc.z * deltaT * deltaT;

            }
        }

        oldPosSize = (oldPosSize) * (oldPosSize < HISTORY_AMOUNT) + 1;
        
        for (int i = 0; i < BODY_AMOUNT; i++) {
            oldpos[i][oldPosSize-1] = (Vector3){bodies[i].pos.x, bodies[i].pos.y, bodies[i].pos.z};
        }

        BeginDrawing();

            ClearBackground(BLACK);
            DrawFPS(10, 10);
            DrawText(TextFormat("Speed %d", speed), 10, 40, 10, WHITE);
            DrawText(TextFormat("%f %f %f", bodies[0].pos.x, bodies[0].pos.y, bodies[0].pos.z), 10, 30, 10, WHITE);
            
            BeginMode3D(camera);
                //DrawGrid(100, 1);
                rlPushMatrix();
                rlScalef(1e-9,1e-9,1e-9);

                for (int i = 0; i< BODY_AMOUNT; i++) {
                    DrawSphere((Vector3) { bodies[i].pos.x * VECT_MULTIPLIER, bodies[i].pos.y * VECT_MULTIPLIER, bodies[i].pos.z * VECT_MULTIPLIER},bodies[i].radius * RADIUS_MULTIPLIER, bodies[i].color);
                    p = oldPosSize;
                    n = p;
                    for (uint16_t k = 0; k < HISTORY_AMOUNT-1; k++) {
                        p = (p+1) * (p < (HISTORY_AMOUNT-1)) ;
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

void body_init(Body *body, Vec pos, Vec vel, double mass, double radius, Color color) {
    body->pos = pos;
    body->vel = vel;
    body->mass = mass;
    body->radius = radius;
    body->color = color;
    body->acc = (Vec) {0, 0, 0};
    body->nAcc = (Vec) {0, 0, 0};
}

double current_angle(Vector3 pos, Vector3 tar){
    Vector2 temp = (Vector2){ pos.x - tar.x, pos.z - tar.z};
    return atan2(temp.x, temp.y)+PI;
}

void move_player(Camera3D* camera, double curntAngle){

    double xcurntAngle = current_angle2(camera->position, camera->target);

    camera->target.x = camera->target.x + ((SCALE_FACTOR * cos(curntAngle)) * (IsKeyDown(KEY_A) && !IsKeyDown(KEY_D))) + ((- SCALE_FACTOR * cos(curntAngle)) * (IsKeyDown(KEY_D) && !IsKeyDown(KEY_A))) + ((SCALE_FACTOR * sin(curntAngle)) * (IsKeyDown(KEY_W) && !IsKeyDown(KEY_S))) + ((- SCALE_FACTOR * sin(curntAngle)) * (IsKeyDown(KEY_S) && !IsKeyDown(KEY_W)));

    camera->position.x = camera->position.x + (((SCALE_FACTOR * cos(curntAngle)) * (IsKeyDown(KEY_A) && !IsKeyDown(KEY_D))) + (- SCALE_FACTOR * cos(curntAngle)) * (IsKeyDown(KEY_D) && !IsKeyDown(KEY_A))) + ((+ SCALE_FACTOR * sin(curntAngle)) * (IsKeyDown(KEY_W) && !IsKeyDown(KEY_S))) + ((- SCALE_FACTOR * sin(curntAngle)) * (IsKeyDown(KEY_S) && !IsKeyDown(KEY_W)));
    
    camera->position.z = camera->position.z +((- SCALE_FACTOR * sin(curntAngle)) * (IsKeyDown(KEY_A) && !IsKeyDown(KEY_D))) + (+ SCALE_FACTOR * sin(curntAngle)) * (IsKeyDown(KEY_D) && !IsKeyDown(KEY_A)) + ((+ SCALE_FACTOR * sin(curntAngle)) * (IsKeyDown(KEY_W) && !IsKeyDown(KEY_S))) + ((- SCALE_FACTOR * cos(curntAngle)) * (IsKeyDown(KEY_S) && !IsKeyDown(KEY_W)));
    
    camera->target.z = camera->target.z + ((- SCALE_FACTOR * sin(curntAngle)) * (IsKeyDown(KEY_A) && !IsKeyDown(KEY_D))) + ((+ SCALE_FACTOR * sin(curntAngle)) * (IsKeyDown(KEY_D) && !IsKeyDown(KEY_A))) + ((+ SCALE_FACTOR * cos(curntAngle)) * (IsKeyDown(KEY_W) && !IsKeyDown(KEY_S))) + ((- SCALE_FACTOR * cos(curntAngle)) * (IsKeyDown(KEY_S) && !IsKeyDown(KEY_W)));

    camera-> position.y = camera->position.y + (- SCALE_FACTOR * sin(xcurntAngle)) * (IsKeyDown(KEY_W) && !IsKeyDown(KEY_S)) + (+ SCALE_FACTOR * sin(xcurntAngle)) * (!IsKeyDown(KEY_W) && IsKeyDown(KEY_S));

    camera->target.y = camera->target.y + ( - SCALE_FACTOR * sin(xcurntAngle)) * (IsKeyDown(KEY_W) && !IsKeyDown(KEY_S)) + (+ SCALE_FACTOR * sin(xcurntAngle)) * (!IsKeyDown(KEY_W) && IsKeyDown(KEY_S));
}

double current_angle2(Vector3 pos, Vector3 tar){

    return atan((pos.y-tar.y)/(sqrt(pow(pos.x-tar.x, 2) + sqrt(pow(pos.z-tar.z, 2)))));
}
