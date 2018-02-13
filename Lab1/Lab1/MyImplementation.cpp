//===========================================================================
// Your implementation of the spring-damper system 
//===========================================================================

#include <stdafx.h>
#include "MyImplementation.h"

void myImplement()
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
    cVector3d newForce (0,0,0);

    // apply force field
    double Kp = 100.0; // [N/m]
    cVector3d force = cMul(-Kp, newPosition);
        
    // apply viscosity 
    double Kv = 1.0; // Kv < 20.0
	if ( Kv < maxLinearDamping )
	{
		force += cMul(-Kv, linearVelocity);
	}

    newForce.add(force);

    // send computed force to haptic device
    hapticDevice->setForce(newForce);
}