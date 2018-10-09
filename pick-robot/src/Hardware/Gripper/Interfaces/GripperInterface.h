/*
 * GripperInterface.h
 *
 *  Created on: Aug 7, 2018
 *      Author: masonuren
 */

#ifndef SRC_HARDWARE_GRIPPER_GRIPPERINTERFACE_H_
#define SRC_HARDWARE_GRIPPER_GRIPPERINTERFACE_H_

#include "../../../Utilities/ComponentInterface.h"
#include "../Interfaces/VacSensorInterface.h"

/**
 * @interface GripperInterface
 * @brief Provides a generic way to interact with vacuum logic.
 *
 * All gripper interactions, live or simulated, must inherit from this
 * 	interface to be properly integrated with the Pick-Robot software.
 */
class GripperInterface : public ComponentInterface {
public:
	GripperInterface() {}
	virtual ~GripperInterface() {}

	/**
	 * @fn activate
	 * @brief Turn on the vacuum gripper, if the gripper state is #VC_OFF.
	 */
	virtual void activate() = 0;

	/**
	 * @fn deactivate
	 * @brief Turn off the vacuum gripper, if the gripper state is #VC_ON.
	 */
	virtual void deactivate() = 0;

	/**
	 * @fn getSensor
	 * @return An instance of the vacuum sensor.
	 */
	virtual VacSensorInterface *getSensor() = 0;

	/**
	 * @fn emergencyStop
	 * @brief Immediately turn of the vacuum gripper (no suction).
	 */
	virtual void emergencyStop() = 0;

	/**
	 * @fn getVacuumState
	 * @return #VC_ON | #VC_OFF
	 */
	virtual VACUUM_GRIPPER_STATE getVacuumState() = 0;
};



#endif /* SRC_HARDWARE_GRIPPER_GRIPPERINTERFACE_H_ */
