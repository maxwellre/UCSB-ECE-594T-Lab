#pragma once // Ensure unique inclusion

#ifndef MYIMPLEMENT_H
#define MYIMPLEMENT_H
#endif

//---------------------------------------------------------------------------
#include "chai3d.h"
//---------------------------------------------------------------------------

// A pointer to the haptic devices detected on this computer (defined in Lab1.cpp)
extern cGenericHapticDevice* hapticDevice;

// Device maximum viscosity 
extern double maxLinearDamping;

// A 3D cursors for the haptic device
extern cShapeSphere* a3DCursor;

// A line to display displacement of the cursor
extern cShapeLine* displacementVector;

// material properties used to render the color of the cursors
extern cMaterial matCursorButtonON;
extern cMaterial matCursorButtonOFF;

// Timer
extern cPrecisionClock aTimer;

// Save position data
extern std::ofstream posiOFS;

// Save event data
extern std::ofstream eventOFS;

// Implement the model inside the function (will be called at each haptic update loop)
void myUpdate();

// Implement the model inside the function (will be called only once during initialization)
void myInitialization();