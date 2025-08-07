/* Author : XentiQ
Date created - 2022.12.14 - XentiQ version
*/

#include "UART.h"
#include <AccelStepper.h>

#ifndef _XY_TABLE_H_
#define _XY_TABLE_H_


//Change MAX_TUBES_X and MAX_TUBES_Y depending on the use;
//for (L) 300x300 - MAX_TUBES_X 33; MAX_TUBES_Y 33
//for (S) 200x300 - MAX_TUBES_X 20; MAX_TUBES_Y 27
#define MAX_TUBES_X 42//try054 33
#define MAX_TUBES_Y 33

#define MANUAL_MODE 1
#define AUTO_MODE 2

enum class ActionState
{
    VibrateOn,
    VibrateOff,
    DispenseStart,
    DispenseStop,
    MoveX,
    MoveY,
};

struct PauseState
{
    int32_t x;
    int32_t y;
    int32_t cycle;
    float currentX;
    float currentY;
    long lastX;
    long lastY;
    ActionState lastAction;
    // ActionState currentAction;
    int32_t currentCycle;
};


void activateDispenser(void);

void GPIO_Setup(void);
void init_Motors(void);
bool Homing(void);
bool Zeroing(void);

void runStepper(AccelStepper &stepper, int distance, uint8_t limitPin, const char *limitMsg, ActionState actionState);

uint8_t runStepper_normal(AccelStepper &stepper, long distance, uint8_t limitPin); 
void returnToStart();
// void checkAndMoveStepper(AccelStepper &stepper, int steps, int limit, const char *message, ActionState action);

void checkAndMoveStepper_normal(AccelStepper &stepper, int steps, int limit, const char *message);
uint8_t vibrateAndCheckPause();
uint8_t dispenseAndCheckPause();
void Run_profile();


void xy_init(void);
void xy_main(void);

bool performVibrateAndDispenseOperations();
void startProcess();
void resumeProcess();


#endif /*_XY_TABLE_H_*/
