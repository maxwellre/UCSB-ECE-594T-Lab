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
	cVector3d yaxis (0,1,0);

    // apply force 
    double Kp = 500.0; // [N/m]
    cVector3d force(0,0,0);

	//if (newPosition.y < 0.0)
	//{
		force = cMul(-Kp, newPosition);
        
	    // apply viscosity 
		double Kv = 5.5;
	
		if ( Kv < maxLinearDamping )
		{
			force += cMul(-Kv, linearVelocity);
		}

	//	force.x = 0.0; force.z = 0.0;
	//}

    newForce.add(force);

    // send computed force to haptic device
    hapticDevice->setForce(newForce);
}