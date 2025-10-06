/* Author : XentiQ
Date created - 2022.12.14 - XentiQ version
*/

#include <AccelStepper.h>

#ifndef _XY_TABLE_H_
#define _XY_TABLE_H_

//Change MAX_TUBES_X and MAX_TUBES_Y depending on the use;
//for (L) 300x300 - MAX_TUBES_X 33; MAX_TUBES_Y 33
//for (S) 200x300 - MAX_TUBES_X 20; MAX_TUBES_Y 27
#define MAX_TUBES_X 42//try054 33
#define MAX_TUBES_Y 33
#define MAX_ZDIP 2000 // 20cm

class BaseController;

// External reference to the current controller being used.
extern BaseController* controller;

void setupPasswordHandling(void);

#endif /*_XY_TABLE_H_*/
