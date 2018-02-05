#include <stdafx.h>
#include "MyImplement.h"

void myImplement()
{
	// read position of haptic device
    cVector3d newPosition;
    hapticDevice->getPosition(newPosition);

    // read orientation of haptic device
    cMatrix3d newRotation;
    hapticDevice->getRotation(newRotation);

    // update position and orientation of cursor
    a3DCursor->setPos(newPosition);
    a3DCursor->setRot(newRotation);

    // read linear velocity from device
    cVector3d linearVelocity;
    hapticDevice->getLinearVelocity(linearVelocity);

	// update arrow
    displacementVector->m_pointA = cVector3d(0,0,0);
    displacementVector->m_pointB = newPosition;

    // read user button status
    bool buttonStatus;
    hapticDevice->getUserSwitch(0, buttonStatus);

    // adjustthe  color of the cursor according to the status of
    // the user switch (ON = TRUE / OFF = FALSE)
    if (buttonStatus)
    {
        a3DCursor->m_material = matCursorButtonON;
    }
    else
    {
        a3DCursor->m_material = matCursorButtonOFF;
    }

    // compute a reaction force
    cVector3d newForce (0,0,0);

    // apply force field
    double Kp = 100.0; // [N/m]
    cVector3d force = cMul(-Kp, newPosition);
        
    // apply viscosity 
    double Kv = 10;
	if ( Kv < maxLinearDamping )
	{
		force += cMul(-Kv, linearVelocity);
	}

    newForce.add(force);

    // send computed force to haptic device
    hapticDevice->setForce(newForce);
}