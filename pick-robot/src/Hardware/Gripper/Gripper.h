/*
 * GripperAbstraction.h
 *
 *  Created on: Aug 10, 2018
 *      Author: masonuren
 */

#ifndef SRC_HARDWARE_GRIPPER_GRIPPER_H_
#define SRC_HARDWARE_GRIPPER_GRIPPER_H_

#include "Interfaces/GripperInterface.h"

class Gripper : virtual public GripperInterface {
public:
	Gripper()
		: state(VC_OFF) {}
	virtual ~Gripper() {}

	VACUUM_GRIPPER_STATE getVacuumState() {
		return state;
	}

protected:
	VACUUM_GRIPPER_STATE state; /**< The current state of the vacuum gripper */
};



#endif /* SRC_HARDWARE_GRIPPER_GRIPPER_H_ */
