/*
 * CommandHandler.h
 *
 *  Created on: Aug 15, 2018
 *      Author: michaelkaiser
 */

#ifndef SRC_SOFTWARE_COMMANDHANDLER_COMMANDHANDLER_H_
#define SRC_SOFTWARE_COMMANDHANDLER_COMMANDHANDLER_H_

#include <SharedMemoryStructs.h>

class Gripper;

class TargetGenerator;

class SharedMemory;

class MotorController;

class ZeroReturnController;

class PickControl;

/**
 * @class CommandHandler
 * @brief Responsible for handling commands passed through the socket connection.
 *
 * If a command is deemed valid by the @ref pick-trigger-app, then said command is then
 * 	passed to the robot through #ROBOT_IN. Based on the command, the current state
 * 	#PICK_STATE, and whether the machine has been zeroed, the command will either be handled
 * 	appropriately or ignored.
 *
 * 	_Processed commands include:_
 *
 * 	If #PC_READY, #ZR_IDLE, and #ZeroReturnController::isZeroed...
 * 		- `pick` 			: pick an item from the next target location.
 * 		- `target=-x,-y,-z` : pick an item at the given (x, y, z) location.
 * 		- `-x,-y,-z` 		: move the arm to the desired (x, y, z) location.
 *
 * 	If #PC_NEEDS_ZERO or the pick-robot is *not* #ZeroReturnController::isZeroed...
 * 		- `zneeded`			: zero the pick-robot only if not already zeroed.
 *
 * 	If #PC_NEEDS_ZERO or #PC_READY...
 * 		- `zero`			: zero the pick-robot if needed or not performing a pick routine.
 *
 * 	If #PC_AT_DROPOFF_XYZ...
 * 		- `drop`			: drop the item and return to the staging area.
 * 		- `place=-x,-y-z1	: move to the new drop-off location, release the item, then return to staging.
 *
 * 	Unchecked Commands...
 * 		- `json={`			: load the JSON configuration (only performed on-boot or when updating the current configuration).
 * 		- `vcon`			: turn on #VacuumGripper::activate, if #VacuumGripper not already in #VC_ON state.
 * 		- `vcoff`			: turn off #VacuumGripper::deactivate, if #VacuumGripper not already in #VC_OFF state.
 * 		- `reset`			: soft emergency stop. Doesn't require re-zero, but stops all motion, turns off #VacuumGripper::deactivate,
 * 								and removes any actively reported errors, but doesn't disrupt current routine.
 *
 * All commands must also respect axis limits set by #AXIS_CONFIG::travelLimitmm
 */
class CommandHandler {
private:
	SharedMemory * sm;
	PickControl* pc;
	ZeroReturnController* zeroController;
	MotorController* motorController;
	Gripper* gripper;
	TargetGenerator * targetGenerator;

public:
	/**
	 * @param[in] sm A reference to #SharedMemory.
	 * @param[in] pc A reference to the #PickControl logic.
	 * @param[in] zeroController A reference to the #ZeroReturnController.
	 * @param[in] motorController A reference to the #MotorController.
	 * @param[in] gripper A reference to the #Gripper interface.
	 * @param[in] targetGenerator A reference to #TargetGenerator.
	 */
	CommandHandler(SharedMemory * sm, PickControl* pc, ZeroReturnController* zeroController,
			MotorController* motorController, Gripper* gripper, TargetGenerator * targetGenerator);
	virtual ~CommandHandler();

	/**
	 * @fn processCommand
	 * @brief Handles valid commands passed from @ref pick-trigger-app via #ROBOT_IN.
	 * @param rin A reference to #ROBOT_IN
	 */
	void processCommand(ROBOT_IN* rin);
};

#endif /* SRC_SOFTWARE_COMMANDHANDLER_COMMANDHANDLER_H_ */
