//===========================================================================
// Your implementation of the spring-damper system 
//===========================================================================

#include <stdafx.h>
#include "MyImplementation.h"

bool isEventOn, isOutOfWall;
int eCount;

double eventResponse[1000];

// Implement the model inside the function (This function will be called at each haptic update loop)
void myUpdate()
{
	// read position of haptic device
    cVector3d newPosition;
    hapticDevice->getPosition(newPosition);

    // update position of cursor
    a3DCursor->setPos(newPosition);

	// save position of cursor
	posiOFS << newPosition.x << "," << newPosition.y << "," << newPosition.z << "," << aTimer.getCurrentTimeSeconds() << std::endl;

    // read linear velocity from device
    cVector3d linearVelocity;
    hapticDevice->getLinearVelocity(linearVelocity);

	// update arrow
    displacementVector->m_pointA = cVector3d(0,0,0);
    displacementVector->m_pointB = newPosition;

    // compute a reaction force
    cVector3d force (0,0,0);

	if (newPosition.y < 0.0)
	{
		double Kp = 8000.0; // [N/m]
		double Kv = 2.0; // Kv < 20.0

		// apply force field
		force = cMul(-Kp, newPosition);
        
	    // apply viscosity 
		
		if ( Kv < maxLinearDamping )
		{
			force += cMul(-Kv, linearVelocity);
		}

		if( isOutOfWall ) isEventOn = true;

			if (isEventOn)
			{
				force.y += eventResponse[eCount];
				eCount++;
				if (eCount == 1000)
				{
					eCount = 0;
					isEventOn = false;
				}
			}

		isOutOfWall = false;

		force.x = 0.0; force.z = 0.0;
	}
	else
	{
		isOutOfWall = true;
		isEventOn = false;
		eCount = 0;
	}

    // send computed force to haptic device
    hapticDevice->setForce(force);
}

// Prepare your model inside the function (This function will be called only once during initialization)
void myInitialization()
{
	const double Fs = 1000.0; // Haptic device sampling frequency = 1000.0 Hz by default
	const double PI = 3.14159265359;
	
	// Event response = amplitude * e^(- alpha * t) * sin(2 * pi * fo * t), where t is the time.
	double amplitude = 4.0; // Event amplitude
	double fo = 200.0; // Event oscillating frequency (Hz)
	double alpha = 10.0; // Event attenutation factor

	std::string eventFileName;
	eventFileName = "../../EventData.csv";
	eventOFS.open(eventFileName);

	for (int i = 0; i < Fs; i++)
	{
		eventResponse[i] = amplitude * exp(-alpha * i / Fs) * sin(2 * PI * fo * i / Fs);

		eventOFS << i << "," << eventResponse[i] << std::endl;
	}

	eventOFS.close();

	// Initialize status flags
	isEventOn = false;
	isOutOfWall = true;

	eCount = 0;
}