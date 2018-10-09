/*
 * GripperSelection.h
 *
 *  Created on: Aug 13, 2018
 *      Author: masonuren
 */

#ifndef SRC_HARDWARE_GRIPPER_GRIPPERFACTORY_H_
#define SRC_HARDWARE_GRIPPER_GRIPPERFACTORY_H_

#include <slushboard.h>
#include "Gripper.h"
#include "VacuumGripper.h"
#include "Simulation/SimVacGripper.h"

/**
 * @class GripperFactory
 * @brief Factory implementation that creates either a real or simulated gripper object.
 */
class GripperFactory {
public:
	/**
	 * @static create
	 * @brief Creates the desired #Gripper (live or simulated).
	 * @param[in] simulate A flag that determines whether the gripper actions should be simulated or not.
	 * @param[in] slushboard A reference to the hardware board the robot is implemented.
	 * @return A reference to the created gripper.
	 */
	static Gripper* create(bool simulate, SlushBoard *slushboard);
};



#endif /* SRC_HARDWARE_GRIPPER_GRIPPERFACTORY_H_ */
