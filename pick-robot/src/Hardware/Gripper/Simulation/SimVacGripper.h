/*
 * SimulatedVacuumGripper.h
 *
 *  Created on: Aug 7, 2018
 *      Author: masonuren
 */

#ifndef SRC_HARDWARE_GRIPPER_SIMULATEDVACUUMGRIPPER_H_
#define SRC_HARDWARE_GRIPPER_SIMULATEDVACUUMGRIPPER_H_

#include "../Gripper.h"
#include "SimVacSensor.h"

/**
 * @class SimVacGripper
 * @brief A virtual vacuum gripper.
 *
 * Does not establish physical interactions with hardware, but exists
 * 	as a way to test the logic of the Pick-Robot vacuum gripper software.
 */
class SimVacGripper : public Gripper {
public:
	/**
	 * Sets:
	 * 		- #state : #VC_OFF
	 */
	SimVacGripper();
	virtual ~SimVacGripper() {}

	/**
	 * @fn step
	 * @brief *** Not Implemented ***
	 * @param[in] clockTicks The current clock tick in milliseconds.
	 */
	void step(long long int clockTicks);

	/**
	 * @fn reportStatus
	 * @brief Check if the simulated vacuum is on and report to #ROBOT_OUT
	 * @param[in] rout A pointer to the #ROBOT_OUT.
	 */
	void reportStatus(void *rout);

	/**
	 * @fn activate
	 * @brief Set the simulated vacuum state to #VC_ON
	 */
	void activate();

	/**
	 * @fn deactivate
	 * @brief Set the simulated vacuum state to #VC_OFF
	 */
	void deactivate();
	SimVacSensor *getSensor() {
		return &simVacSensor;
	}

	/**
	 * @fn emergencyStop
	 * @brief Immediately change the simulated vacuum state to #VC_OFF
	 */
	void emergencyStop();

private:
	SimVacSensor simVacSensor = SimVacSensor();
};

#endif /* SRC_HARDWARE_GRIPPER_SIMULATEDVACUUMGRIPPER_H_ */
