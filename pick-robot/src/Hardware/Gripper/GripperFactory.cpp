/*
 * GripperFactor.cpp
 *
 *  Created on: Aug 14, 2018
 *      Author: masonuren
 */

#include "GripperFactory.h"

Gripper* GripperFactory::create(bool simulate, SlushBoard *slushboard) {
	if (simulate) {
		return new SimVacGripper();
	}
	else {
		return new VacuumGripper(slushboard);
	}
}


