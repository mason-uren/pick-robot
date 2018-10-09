/*
 * SimulatedVacuumSensor.h
 *
 *  Created on: Aug 7, 2018
 *      Author: masonuren
 */

#ifndef SRC_HARDWARE_GRIPPER_SIMULATEDVACUUMSENSOR_H_
#define SRC_HARDWARE_GRIPPER_SIMULATEDVACUUMSENSOR_H_

#include "../../../Utilities/ComponentInterface.h"
#include "../Interfaces/VacSensorInterface.h"

/**
 * @class SimVacSensor
 * @brief A virtual vacuum sensor.
 *
 * Does not establish physical interactions with hardware, but exists
 * 	as a way to test the logic of the Pick-Robot vacuum sensor software.
 */
class SimVacSensor : public VacSensorInterface {
public:
	/**
	 * Sets:
	 * 		- #vacState : #VC_OFF
	 * 		- #highThresh : #HIGH_THRESH
	 * 		- #lowThresh : #LOW_THRESH
	 */
	SimVacSensor()
		: vacState(VACUUM_GRIPPER_STATE::VC_OFF),
		  highThresh(HIGH_THRESH),
		  lowThresh(LOW_THRESH) {}
	virtual ~SimVacSensor() {}

	/**
	 * @fn step
	 * @brief *** Not implemented ***
	 */
	void step(long long int clockTicks);

	/**
	 * @fn reportStatus
	 * @brief Report the current suction value and whether the gripper
	 * 	has suction to #ROBOT_OUT.
	 */
	void reportStatus(void *);

	bool hasSuction();
	bool hasIndeterminateSuction();

	/**
	 * @fn getCurrentSuctionValue
	 * @brief Get the current suction value based on vacuum state.
	 * @retval 0 #VC_ON
	 * @retval 1 #VC_OFF.
	 */
	double getCurrentSuctionValue();
	void setHighThresh(int high);
	void setLowThresh(int low);

	/**
	 * @fn emergencyStop
	 * @brief *** Not Implemented ***
	 *
	 * Handled by #SimVacGripper.
	 */
	void emergencyStop();

	/**
	 * @fn setVacState
	 * @param[in] state Sets the state of #vacState.
	 */
	void setVacState(VACUUM_GRIPPER_STATE state);

private:
	VACUUM_GRIPPER_STATE vacState;		/**< The current simulated vacuum sensor state. */
	int highThresh;						/**< The high threshold of read suction values (ie. values above are considered #BAD_SUCTION). */
	int lowThresh;						/**< The low threshold of read suction values (ie. values below are considered #GOOD_SUCTION). */
};

#endif /* SRC_HARDWARE_GRIPPER_SIMULATEDVACUUMSENSOR_H_ */
