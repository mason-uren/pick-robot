/*
 * ZeroReturnController.h
 *
 *  Created on: Jul 25, 2018
 *      Author: michaelkaiser
 */

#ifndef SRC_SOFTWARE_ZERORETURN_ZERORETURNCONTROLLER_H_
#define SRC_SOFTWARE_ZERORETURN_ZERORETURNCONTROLLER_H_

#include "../../Utilities/ComponentInterface.h"

class MotorController;

/**
 * Zero Return States
 */
enum ZERO_RETURN_STATE {
	ZR_IDLE,												/**< The motors are not zeroing */
	ZR_STARTED,												/**< Command z axis to zero */
	ZR_ZEROING_Z, 											/**< Wait for z axis to finish zeroing */
	ZR_AT_Z_ZERO, 											/**< Command x and y axes to zero */
	ZR_ZEROING_XY, 											/**< Wait for x and y axes to finish zeroing */
	ZR_MOVE_OFF_LIM_SWITCH, 								/**< Command x, y, and z axes to move off limit switches */
	ZR_MOVING_OFF_SWITCH,									/**< Wait for x, y, and z axes to finish moving off switches */
	ZR_AT_ZERO,												/**< Command x, y, and z axes to staging area */
	ZR_MOVING_TO_STAGING									/**< Wait for x, y, and z axes to reach staging area */
};

/**
 * @class ZeroReturnController
 * @brief Responsible for zeroing each axis of the Pick-Robot.
 *
 * Handles the independent motion of zeroing each access, by decoupling from
 * 	the actions within the #PickController. First the z axis will zero, while x and y axes
 * 	remain static, to ensure that arm is free of any obstructions that may have resulted
 * 	from previous picks (if any). Once the z axis has finished zeroing, x and y will
 * 	zero together until each axis is against their respective limit switches.
 * 	Once axes zero they will move, only slightly, off the limit switches, mark their
 * 	current location as the home location, then move to the designated staging area.
 * 	This action, of moving off limit switches, is in place to ensure that they never
 * 	are unintentionally re-depressed during normal operations, which would stop axes motion.
 */
class ZeroReturnController: public ComponentInterface {
private:
	/** Has the machine already been zeroed. */
	bool zeroed;
	/** A reference to the #MotorController, to provide motion. */
	MotorController *mc;

	/**
	 * @fn zeroReturnAxisZ
	 * @brief Command the z axis in the direction of the z axis limit switch.
	 *
	 * Instructs the z axis motors to move in the direction of their limit
	 * 	switches. Moves at a pre-defined slow speed since relative motor
	 * 	position is unknown.
	 *
	 * Sets:
	 * 		- #state : #ZR_ZEROING_Z
	 */
	void zeroReturnAxisZ();

	/**
	 * @fn zeroReturnAxesXY
	 * @brief Command the x and y axes in the direction of their limit switches.
	 *
	 * Instructs the x and y axes motors to move in the direction of their limit
	 * 	switches. Moves at a pre-defined slow speed since relative motor
	 * 	position is unknown.
	 *
	 * Sets:
	 * 		- #state : #ZR_ZEROING_XY
	 */
	void zeroReturnAxesXY();

	/**
	 * @fn checkLimitSwitchZ
	 * @brief Check if the z limit switch is depressed.
	 *
	 * Sets:
	 * 		- #state : #ZR_AT_Z_ZERO
	 */
	void checkLimitSwitchZ();

	/**
	 * @fn checkLimitSwitchXY
	 * @brief Check if x and y limit switches are depressed.
	 *
	 * Sets:
	 * 		- #state : #ZR_MOVE_OFF_LIM_SWITCH
	 */
	void checkLimitSwitchXY();

	/**
	 * @fn moveToOffset
	 * @brief Command motors off of limit switches.
	 *
	 * Sets:
	 * 		- #state : #ZR_MOVING_OFF_SWITCH
	 */
	void moveToOffset();

	/**
	 * @fn waitUntilAtOffset
	 * @brief Wait until each motor has move off their respective limit switches.
	 *
	 * Each motor will come to a stop once, the limit switch is no longer depressed,
	 * 	at which point the current location of each motor will be set as their home
	 * 	position. This ensures the limit switches are not accidently depressed at anytime
	 * 	during normal pick-routine operations.
	 *
	 * Sets:
	 * 		- #state : #ZR_AT_ZERO
	 */
	void waitUntilAtOffset();

	/**
	 * @fn moveToStaging
	 * @brief Command each motor to the preset staging area.
	 *
	 * Sets:
	 * 		- #state : #ZR_MOVING_TO_STAGING
	 */
	void moveToStaging();

	/**
	 * @fn waitUntilAtStaging
	 * @brief Wait until each motor has reached their staging location, then
	 * 	stop motion.
	 *
	 * Once axes are at the staging location:
	 * 		- #state : #ZR_IDLE
	 */
	void waitUntilAtStaging();
public:
	/**
	 * @param mcObj Reference to the #MotorController.
	 *
	 * Sets:
	 * 		- #mc : @p mcObj
	 * 		- #state : #ZR_IDLE
	 * 		- #zeroed : `false`
	 */
	ZeroReturnController(MotorController *mcObj);
	virtual ~ZeroReturnController();
	/** Current state of ZeroReturnController */
	ZERO_RETURN_STATE state;

	/**
	 * @fn step
	 * @brief Responsible for determining next appropriate #ZERO_RETURN_STATE.
	 *
	 * If no errors persist, this function handles the state-machine which drives the
	 * 	zeroing motions of the system.
	 * @param[in] clockTicks The current clock tick in milliseconds.
	 */
	void step(long long int clockTicks);

	/**
	 * @fn reportStatus
	 * @brief *** Not Implemented ***
	 */
	void reportStatus(void * rout);

	/**
	 * @fn emergencyStop
	 * @brief Calls #clearZero.
	 */
	void emergencyStop();

	/**
	 * @fn getState
	 * @return The current state of the ZeroReturnController.
	 */
	ZERO_RETURN_STATE getState() {
		return state;
	}

	/**
	 * @fn isZeroed
	 * @return Whether the system has been zeroed.
	 */
	bool isZeroed() {
		return zeroed;
	}

	/**
	 * @fn clearZero
	 * @brief Resets #state and #zeroed, to indicate the system needs to be
	 * 	re-zeroed.
	 */
	void clearZero() {
		state = ZR_IDLE;
		zeroed = false;
	}

	/**
	 * @fn start
	 * @brief Trigger the zeroing process.
	 */
	void start();

	/**
	 * @fn reset
	 * @brief Sets #stated to #ZR_IDLE.
	 */
	void reset() {
		state = ZR_IDLE;
	}
};

#endif /* SRC_SOFTWARE_ZERORETURN_ZERORETURNCONTROLLER_H_ */
