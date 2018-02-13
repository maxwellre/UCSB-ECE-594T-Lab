//===========================================================================
/*
    This file is part of the CHAI 3D visualization and haptics libraries.
    Copyright (C) 2003-2009 by CHAI 3D. All rights reserved.

    This library is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License("GPL") version 2
    as published by the Free Software Foundation.

    For using the CHAI 3D libraries with software that can not be combined
    with the GNU GPL, and for taking advantage of the additional benefits
    of our support services, please contact CHAI 3D about acquiring a
    Professional Edition License.

    \author    <http://www.chai3d.org>
    \author    Francois Conti
    \version   2.0.0 $Rev: 265 $
*/
/*
	Modified for ECE 594T Haptics - Lab1
*/
//===========================================================================
#include <stdafx.h>
//---------------------------------------------------------------------------
#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "MyImplementation.h"

//---------------------------------------------------------------------------
// DECLARED CONSTANTS
//---------------------------------------------------------------------------

// initial size (width/height) in pixels of the display window
const int WINDOW_SIZE_W         = 600;
const int WINDOW_SIZE_H         = 600;

// mouse menu options (right button)
const int OPTION_FULLSCREEN     = 1;
const int OPTION_WINDOWDISPLAY  = 2;

//---------------------------------------------------------------------------
// DECLARED VARIABLES
//---------------------------------------------------------------------------

// a world that contains all objects of the virtual environment
cWorld* world;

// a camera that renders the world in a window display
cCamera* camera;

// a light source to illuminate the objects in the virtual scene
cLight *light;

// a little "chai3d" bitmap logo at the bottom of the screen
cBitmap* logo;

// width and height of the current window display
int displayW  = 0;
int displayH  = 0;

// A pointer to the haptic devices detected on this computer
cGenericHapticDevice* hapticDevice;

// a haptic device handler
cHapticDeviceHandler* handler;

// A pointer to label which display the position of the haptic device
cLabel* posiLabel;

cGenericObject* rootLabels;

// number of haptic devices detected
int numHapticDevices = 0;

// Device maximum viscosity 
double maxLinearDamping;

// A 3D cursors for the haptic device
cShapeSphere* a3DCursor;

// A line to display displacement of the cursor
cShapeLine* displacementVector;

// material properties used to render the color of the cursors
cMaterial matCursorButtonON;
cMaterial matCursorButtonOFF;

// status of the main simulation haptics loop
bool simulationRunning = false;

// root resource path
string resourceRoot;

// has exited haptics simulation thread
bool simulationFinished = false;

// Timer
cPrecisionClock aTimer;

// Save position data
std::string posiFileName;
std::ofstream posiOFS;

//---------------------------------------------------------------------------
// DECLARED MACROS
//---------------------------------------------------------------------------
// convert to resource path
#define RESOURCE_PATH(p)    (char*)((resourceRoot+string(p)).c_str())


//---------------------------------------------------------------------------
// DECLARED FUNCTIONS
//---------------------------------------------------------------------------

// callback when the window display is resized
void resizeWindow(int w, int h);

// callback when a keyboard key is pressed
void keySelect(unsigned char key, int x, int y);

// callback when the right mouse button is pressed to select a menu item
void menuSelect(int value);

// function called before exiting the application
void close(void);

// main graphics callback
void updateGraphics(void);

// main haptics loop
void updateHaptics(void);


//===========================================================================
/*
    DEMO:    device.cpp

    This application illustrates the use of the haptic device handler
    "cHapticDevicehandler" to access all of the haptic devices
    "cGenericHapticDevice" connected to the computer.

    In this example the application opens an OpenGL window and displays a
    3D cursor for each device. Each cursor (sphere + reference frame)
    represents the position and orientation of its respective device.
    If the operator presses the device user button (if available), the color
    of the cursor changes accordingly.

    In the main haptics loop function  "updateHaptics()" , the position,
    orientation and user switch status of each device are retrieved at
    each simulation iteration. The information is then used to update the
    position, orientation and color of the cursor. A force is then commanded
    to the haptic device to attract the end-effector towards the device origin.
*/
//===========================================================================

int main(int argc, char* argv[])
{
    //-----------------------------------------------------------------------
    // INITIALIZATION
    //-----------------------------------------------------------------------

	posiFileName = "../../CursorPosition.csv";

    printf ("\n");
    printf ("-----------------------------------\n");
    printf ("ECE594T Haptics\n");
    printf ("Lab1: Spring-damper System\n");
    printf ("-----------------------------------\n");
    printf ("\n\n");
    printf ("Keyboard Options:\n\n");
    printf ("[x] - Exit application\n");
    printf ("\n\n");
	printf ("-----------------------------------\n");
	std::cout << "Cursor position data will be saved to " << posiFileName << std::endl;

    // parse first arg to try and locate resources
    resourceRoot = string(argv[0]).substr(0,string(argv[0]).find_last_of("/\\")+1);

	posiOFS.open(posiFileName);
    //-----------------------------------------------------------------------
    // 3D - SCENEGRAPH
    //-----------------------------------------------------------------------

    // create a new world.
    world = new cWorld();

    // set the background color of the environment
    // the color is defined by its (R,G,B) components.
    world->setBackgroundColor(0.0, 0.0, 0.0);

    // create a camera and insert it into the virtual world
    camera = new cCamera(world);
    world->addChild(camera);

    // position and oriente the camera
    camera->set( cVector3d (0.2, 0.0, 0.0),    // camera position (eye)
                 cVector3d (0.0, 0.0, 0.0),    // lookat position (target)
                 cVector3d (0.0, 0.0, 1.0));   // direction of the "up" vector

    // set the near and far clipping planes of the camera
    // anything in front/behind these clipping planes will not be rendered
    camera->setClippingPlanes(0.01, 10.0);

    // create a light source and attach it to the camera
    light = new cLight(world);
    camera->addChild(light);                   // attach light to camera
    light->setEnabled(true);                   // enable light source
    light->setPos(cVector3d( 2.0, 0.5, 1.0));  // position the light source
    light->setDir(cVector3d(-2.0, 0.5, 1.0));  // define the direction of the light beam

    //-----------------------------------------------------------------------
    // HAPTIC DEVICES / TOOLS
    //-----------------------------------------------------------------------

    // create a haptic device handler
    handler = new cHapticDeviceHandler();

    // read the number of haptic devices currently connected to the computer
    numHapticDevices = handler->getNumDevices();

	if( numHapticDevices > 1 )
	{
		printf("More than one haptic device detected: %d\nUse device[0] only!\n", numHapticDevices);
	}

    // create a node on which we will attach small labels that display the
    // position of each haptic device
    rootLabels = new cGenericObject();
    camera->m_front_2Dscene.addChild(rootLabels);

    // create a small label as title
    cLabel* titleLabel = new cLabel();
    rootLabels->addChild(titleLabel);

    // define its position, color and string message
    titleLabel->setPos(0, 30, 0);
    titleLabel->m_fontColor.set(1.0, 1.0, 1.0);
    titleLabel->m_string = "Haptic Device Displacement [mm]:";

    // for the haptic device, create a 3D cursor
    // get a handle to the haptic device
    handler->getDevice(hapticDevice, 0);

    // open connection to haptic device
    hapticDevice->open();

	// initialize haptic device
	hapticDevice->initialize();

    // retrieve information about the current haptic device
    cHapticDeviceInfo info = hapticDevice->getSpecifications();

	maxLinearDamping = info.m_maxLinearDamping; // = 20 For Novint Falcon

    // create a cursor by setting its radius
    a3DCursor = new cShapeSphere(0.002);

    // add cursor to the world
    world->addChild(a3DCursor);

    // create a small line to illustrate displacement of the cursor (from the origin)
	displacementVector = new cShapeLine(cVector3d(0,0,0), cVector3d(0,0,0));

    // add line to the world
    world->addChild(displacementVector);

    // create a string that concatenates the device number and model name.
    string strID;
    cStr(strID, 0);
    string strDevice = info.m_modelName + " Cursor";

    // attach a small label next to the cursor to indicate device information
    cLabel* newLabel = new cLabel();
    a3DCursor->addChild(newLabel);
    newLabel->m_string = strDevice;
    newLabel->setPos(0.00, 0.02, 0.00);
    newLabel->m_fontColor.set(1.0, 1.0, 1.0);

    // crate a small label to indicate the position of the device
    cLabel* newPosLabel = new cLabel();
    rootLabels->addChild(newPosLabel);
    newPosLabel->setPos(0, 0, 0);
    newPosLabel->m_fontColor.set(0.6, 0.6, 0.6);
    posiLabel = newPosLabel;

    // here we define the material properties of the cursor when the
    // user button of the device end-effector is engaged (ON) or released (OFF)

    // a light orange material color
    matCursorButtonOFF.m_ambient.set(0.5, 0.2, 0.0);
    matCursorButtonOFF.m_diffuse.set(1.0, 0.5, 0.0);
    matCursorButtonOFF.m_specular.set(1.0, 1.0, 1.0);

    // a blue material color
    matCursorButtonON.m_ambient.set(0.1, 0.1, 0.4);
    matCursorButtonON.m_diffuse.set(0.3, 0.3, 0.8);
    matCursorButtonON.m_specular.set(1.0, 1.0, 1.0);


    //-----------------------------------------------------------------------
    // OPEN GL - WINDOW DISPLAY
    //-----------------------------------------------------------------------

    // initialize GLUT
    glutInit(&argc, argv);

    // retrieve the resolution of the computer display and estimate the position
    // of the GLUT window so that it is located at the center of the screen
    int screenW = glutGet(GLUT_SCREEN_WIDTH);
    int screenH = glutGet(GLUT_SCREEN_HEIGHT);
    int windowPosX = (screenW - WINDOW_SIZE_W) / 2;
    int windowPosY = (screenH - WINDOW_SIZE_H) / 2;

    // initialize the OpenGL GLUT window
    glutInitWindowPosition(windowPosX, windowPosY);
    glutInitWindowSize(WINDOW_SIZE_W, WINDOW_SIZE_H);
    glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
    glutCreateWindow(argv[0]);
    glutDisplayFunc(updateGraphics);
    glutKeyboardFunc(keySelect);
    glutReshapeFunc(resizeWindow);
    glutSetWindowTitle("CHAI 3D");

    // create a mouse menu (right button)
    glutCreateMenu(menuSelect);
    glutAddMenuEntry("full screen", OPTION_FULLSCREEN);
    glutAddMenuEntry("window display", OPTION_WINDOWDISPLAY);
    glutAttachMenu(GLUT_RIGHT_BUTTON);


    //-----------------------------------------------------------------------
    // START SIMULATION
    //-----------------------------------------------------------------------

    // simulation in now running
    simulationRunning = true;

    // create a thread which starts the main haptics rendering loop
    cThread* hapticsThread = new cThread();
    hapticsThread->set(updateHaptics, CHAI_THREAD_PRIORITY_HAPTICS);

	// start the timer
	aTimer.reset();
	aTimer.start();

    // start the main graphics rendering loop
    glutMainLoop();

    // close everything
    close();

	// stop the timer
	aTimer.stop();

	// close data saving file
	posiOFS.close();

    // exit
    return (0);
}

//---------------------------------------------------------------------------

void resizeWindow(int w, int h)
{
    // update the size of the viewport
    displayW = w;
    displayH = h;
    glViewport(0, 0, displayW, displayH);

    // update position of labels
    rootLabels->setPos(10, displayH-70, 0);
}

//---------------------------------------------------------------------------

void keySelect(unsigned char key, int x, int y)
{
    // escape key
    if ((key == 27) || (key == 'x'))
    {
        // close everything
        close();

        // exit application
        exit(0);
    }
}

//---------------------------------------------------------------------------

void menuSelect(int value)
{
    switch (value)
    {
        // enable full screen display
        case OPTION_FULLSCREEN:
            glutFullScreen();
            break;

        // reshape window to original size
        case OPTION_WINDOWDISPLAY:
            glutReshapeWindow(WINDOW_SIZE_W, WINDOW_SIZE_H);
            break;
    }
}

//---------------------------------------------------------------------------

void close(void)
{
    // stop the simulation
    simulationRunning = false;

    // wait for graphics and haptics loops to terminate
    while (!simulationFinished) { cSleepMs(100); }

    // close the haptic devices
	hapticDevice->close();
}

//---------------------------------------------------------------------------

void updateGraphics(void)
{
    // update content of position label
    // read position of device an convert into millimeters
    cVector3d pos;
    hapticDevice->getPosition(pos);
    pos.mul(1000);

    // create a string that concatenates the device number and its position.
    string strID;
    cStr(strID, 0);
    string strLabel = "  x: ";

    cStr(strLabel, pos.x, 2);
    strLabel = strLabel + "   y: ";
    cStr(strLabel, pos.y, 2);
    strLabel = strLabel + "  z: ";
    cStr(strLabel, pos.z, 2);

    posiLabel->m_string = strLabel;

    // render world
    camera->renderView(displayW, displayH);

    // Swap buffers
    glutSwapBuffers();

    // check for any OpenGL errors
    GLenum err;
    err = glGetError();
    if (err != GL_NO_ERROR) printf("Error:  %s\n", gluErrorString(err));

    // inform the GLUT window to call updateGraphics again (next frame)
    if (simulationRunning)
    {
        glutPostRedisplay();
    }
}

//---------------------------------------------------------------------------

void updateHaptics(void)
{
    // main haptic simulation loop
    while(simulationRunning)
    {
        myImplement();
    }
    
    // exit haptics thread
    simulationFinished = true;
}

//---------------------------------------------------------------------------