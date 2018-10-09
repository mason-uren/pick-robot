/*
 * PickControl.h
 *
 *  Created on: Jul 18, 2018
 *      Author: michaelkaiser
 */

#ifndef SRC_SOFTWARE_PICKCONTROL_PICKCONTROL_H_
#define SRC_SOFTWARE_PICKCONTROL_PICKCONTROL_H_

#include <ConfigStruct.h>
#include <SharedMemoryStructs.h>
#include <array>

#include "../../Hardware/Gripper/Gripper.h"
#include "../../Hardware/Gripper/Interfaces/VacSensorInterface.h"
#include "../../Utilities/ComponentInterface.h"
#include "../../Utilities/Axis.h"

class SharedMemory;

class TargetGenerator;

class ZeroReturnController;

class MotorController;

/**
 * @class PickControl
 * @brief The controller for the pick routine, which determines appropriate motion commands.
 *
 * This handler is responsible for keeping the Pick-Robot in a _normal_ routine of picking and
 * 	placing items. If any errors persist from the #ErrorHandler, this controller needs to respond
 * 	appropriate through direct communication with the #MotorController. The controller will
 * 	sit in a ready state until a `pick` command is received, assuming the machine has already been
 * 	zeroed. The arm will then attempt to pick an item from the designated target, passed from #TargetGenerator.
 * 	If a pick is successful the arm will move take appropriate action to deliver the item to the drop
 * 	location safely. If the pick is unsuccessful, the suction is turned off and the next target
 * 	is acquired. This routine of picking and placing will continue till the robot's belief suggests
 * 	that the RPC is empty.
 */
class PickControl: public ComponentInterface {
private:
	/** Where targets are parsed from */
	TargetGenerator* tg;

	/** Vacuum trigger interactions */
	Gripper* vc;

	/** Load/pass states between Pick-Robot and Pick-Trigger-App */
	SharedMemory* sm;

	/** Interface that provides motions commands */
	MotorController* mc;

	/** For triggering and checking zero states */
	ZeroReturnController* zc;

	/** Interface to read and determine suction */
	VacSensorInterface *vs;

	/** Current picking state */
	PICK_STATE state;

	/** Next picking state to process (Default: #PC_READY) */
	PICK_STATE nextState = PC_READY;

	/** Pointer to the next function to process */
	void (PickControl::*nextStateFunction)();

	/** Passed information either from the initial configuration or Pick-Trigger-App */
	ROBOT_IN block;

	/** Number of items successfully picked (according to the robot's belief) */
	long int itemsPicked;

	/** Current axis target */
	std::array<axis_pos, NUM_AXES> target;

	//Picking functions
	/**
	 * @fn findTarget
	 * @brief Retrieves next available target from #TargetGenerator.
	 *
	 * Only sets targets for the x and y axes.
	 *
	 * Sets:
	 * 		- #state : #PC_TARGET_FOUND
	 * 		- #nextState : #PC_AT_PICK_POSITION_XY
	 */
	void findTarget();

	/**
	 * @fn moveToPickPositionZAboveItem
	 * @brief Move to ready position above target.
	 *
	 * Takes the minimum of the Pick-Robot z-axis current position and the
	 * 	position above the item, commands the arm to that depth, then activates
	 * 	the vacuum gripper.
	 *
	 * Sets:
	 * 		- #state : #PC_MOVING_ABOVE_PICK
	 * 		- #nextState : #PC_AT_PICK_POSITION_XY_ABOVE_Z
	 * 		- #VC_ON
	 */
	void moveToPickPositionZAboveItem();

	/**
	 * @fn moveToPickPositionXYZ
	 * @brief Determine the target z-axis probe depth.
	 *
	 * Sets:
	 *  	- #state : #PC_PROBING
	 */
	void moveToPickPositionXYZ();

	/**
	 * @probeWithVacuum
	 * @brief Lower the arm with #VC_ON..
	 *
	 * The arm will be lower either until the #VacuumSensor reads suction or the
	 * 	arm has reach it's target depth. If the target location is reached with out suction the
	 * 	the arm is raised and a new pick target is generated. If #VacuumSensor has #GOOD_SUCTION, then a
	 * 	soft Z-axis stop is triggered and the depth is marked.
	 *
	 * **if** (#BAD_SUCTION | #INDETERMINATE_SUCTION)
	 * 		- #state : #PC_WAIT_FOR_MOTION
	 * 		- #nextState : #PC_PICK_COMMAND_RECEIVED
	 * 		- #VC_OFF
	 *
	 * **if** #GOOD_SUCTION
	 * 		- #state : #PC_WAIT_FOR_MOTION
	 * 		- #nextState : #PC_HAS_ITEM
	 */
	void probeWithVacuum(long long int clockTicks);

	/**
	 * @fn raiseArm
	 * @brief Raise the Pick-Robot arm.
	 *
	 * Raises the arm to the z-axis clearance height, then checks to see if the
	 *  #VacuumSensor still has the item.
	 *
	 * Sets:
	 * 		- #state : #PC_WAIT_FOR_MOTION
	 * 		- #nextState : #PC_AT_PICK_POSITION_Z_CLEARANCE
	 * 		- #nextStateFunction : #checkVacuumOnReturn
	 */
	void raiseArm();

	/**
	 * @checkVacuumOnReturn
	 * @brief Checks the current state of the #VacuumSensor.
	 *
	 * If #VacuumSensor no longer has #GOOD_SUCTION, then we soft stop any x and y axis motion and
	 *  proceed to the next pick target.
	 *
	 * **If** #BAD_SUCTION
	 * 		- #state : #PC_WAIT_FOR_MOTION
	 * 		- #nextState : #PC_PICK_COMMAND_RECEIVED
	 */
	void checkVacuumOnReturn();

	/**
	 * @fn moveToDropOffPositionXY
	 * @brief Commands the arm to move the X-axis towards the drop-off location.
	 *
	 * Sets:
	 * 		- #state : #PC_MOVING_TO_DROPOFF_XY
	 * 		- #nextState : #PC_AT_DROPOFF_XY
	 * 		- #nextStateFunction : #checkVacuumOnReturn
	 */
	void moveToDropOffPositionXY();

	/**
	 * @fn movingToDropoffXY
	 * @brief Monitors the suction status as arm moves to drop-off (x, y) coordinates.
	 */
	void movingToDropoffXY();

	/**
	 * @fn dropOffItem
	 * @brief Command the robot to lower the arm to the z-axis drop-off location.
	 *
	 * Sets:
	 * 		- #state : #PC_MOVING_TO_DROPOFF_XYZ
	 * 		- #nextState : #PC_AT_DROPOFF_XYZ
	 */
	void dropOffItem();

	/**
	 * @fn moveToDropOffPositionZHome
	 * @brief Pick-Robot has placed the item at the drop location.
	 *
	 * The picked item has been successfully drop at the drop-off location, the
	 * 	vacuum sensor is turned off, and the arm is commanded back to the z-axis clearance position.
	 *
	 * Sets:
	 * 		- #state : #PC_AT_Z_CLEARANCE_RETURN
	 * 		- #nextState : #PC_WAIT_FOR_MOTION
	 * 		- #nextStateFunction : #moveToStagingArea
	 * 		- #VC_OFF
	 */
	void moveToDropOffPositionZHome();

	/**
	 * @fn waitForMotion
	 * @brief Handles motion commands by holding the current states till motors have reached their targets.
	 *
	 * After each motor has reached its target the #nextStateFunction is processed.
	 *
	 * Sets:
	 * 		- #state : #nextState
	 * 		- #nextState : #PC_READY
	 * 		- #nextStateFunction : 0
	 */
	void waitForMotion();

	/**
	 * @fn deactiveGripper
	 * @brief Deactivates the vacuum gripper. #VC_OFF
	 */
	void deactivateGripper();
	// Zero Return
	/**
	 * @fn moveToStagingArea
	 * @brief Move the motors to the staging area.
	 *
	 * Sets:
	 *  	- #state : #PC_WAIT_FOR_MOTION
	 *  	- #nextState : #PC_READY
	 */
	void moveToStagingArea();

	/**
	 * @fn moveToNewDropOff
	 * @brief Moves arm to new drop-off location then places item.
	 *
	 * This is a special routine command that is called using the `place=-x,-y,-z` instruction.
	 * 	If the new drop-off location is within axis limits the arm with re-locate to this position
	 * 	then drop the item, at which point it will fall back into normal pick-routine.
	 * 	_IMPORTANT_: This command assumes that any position within the axis limits is valid, thus it
	 *  is not constrained to a drop-off location. It is possible to hit the RPC using this command.
	 */
	void moveToNewDropOff();

public:

	/**
	 * @param[in] sm A reference to #SharedMemory.
	 * @param[in] mc A reference to the #MotorController, for issuing motion commands.
	 * @param[in] vc A reference to the #Gripper, for controlling #VACUUM_GRIPPER_STATE.
	 * @param[in] zc A reference to the #ZeroReturnController.
	 * @param[in] targetGeneratorConfig A reference to the #TargetGenerator.
	 *
	 * Sets:
	 * 		- #state : #PC_NEEDS_ZERO
	 * 		- #nextState : #PC_READY
	 * 		- #nextStateFunction : 0
	 * 		- #target : `{0, 0, 0}`
	 * 		- #itemsPicked : 0
	 */
	PickControl(SharedMemory* sm, MotorController* mc, Gripper* vc, ZeroReturnController* zc,
			TargetGenerator* targetGeneratorConfig);

	virtual ~PickControl();

	/**
	 * @fn step
	 * @brief Responsible for determining next appropriate #PICK_STATE.
	 *
	 * If no errors persist, this function handles the state-machine which drives the
	 * 	picking and placing motions of the system.
	 * @param[in] clockTicks The current clock tick in milliseconds.
	 */
	void step(long long int clockTicks);

	/**
	 * @fn reportStatus
	 * @brief Outputs #state and #itemsPicked to #ROBOT_OUT.
	 * @param[in] ptr A reference to the #ROBOT_OUT struct.
	 */
	void reportStatus(void *);

	/**
	 * @fn emergencyStop
	 * @brief Immediate termination of pick routine.
	 *
	 * Stops all motor motion through communication with the #MotorController and
	 * 	sets #state to #PC_NEEDS_ZERO.
	 */
	void emergencyStop();

	/**
	 * @fn setState
	 * @brief Set the current #PICK_STATE from outside the PickController.
	 *
	 * _IMPORTANT_: Be careful with how this function is used since it has the potential
	 * 	to break the normal process flow.
	 * @param[in] newState The desired new pick state.
	 */
	void setState(PICK_STATE newState) {
		state = newState;
	}

	/**
	 * @fn setNextState
	 * @brief Set the next #PICK_STATE from outside the PickController, to be processed after
	 * 	the current state.
	 *
	 * _IMPORTANT_: Be careful with how this function is used since it has the potential
	 * 	to break the normal process flow.
	 * @param[in] newState The desired next pick state.
	 */
	void setNextState(PICK_STATE newState) {
		nextState = newState;
	}

	/**
	 * @fn getState
	 * @brief Get the current #state.
	 * @return The current pick control #state.
	 */
	PICK_STATE getState() {
		return state;
	}

	/**
	 * @fn reset
	 * @brief Reset the pick routine.
	 *
	 * Sets:
	 * 		- #state : #PC_READY
	 * 		- #nextState : #PC_READY
	 * 		- #nextStateFunction : 0
	 */
	void reset();
};

#endif /* SRC_SOFTWARE_PICKCONTROL_PICKCONTROL_H_ */
