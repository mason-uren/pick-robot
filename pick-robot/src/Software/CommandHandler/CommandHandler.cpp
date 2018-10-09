/*
 * CommandHandler.cpp
 *
 *  Created on: Aug 15, 2018
 *      Author: michaelkaiser
 */

#include "CommandHandler.h"

#include <ConfigStruct.h>
#include <algorithm>
#include <array>
#include <iterator>

#include "../../Hardware/Gripper/Gripper.h"
#include "../../Hardware/Motors/MotorInterface.h"
#include "../../Utilities/Axis.h"
#include "../ErrorHandler/ErrorHandler.h"
#include "../MotorController/MotorController.h"
#include "../PickControl/PickControl.h"
#include "../TargetGeneration/TargetGenerator.h"
#include "../ZeroReturn/ZeroReturnController.h"

CommandHandler::CommandHandler(SharedMemory * sm, PickControl* pc, ZeroReturnController* zeroController,
		MotorController* motorController, Gripper* gripper, TargetGenerator * targetGenerator) {
	this->sm = sm;
	this->pc = pc;
	this->zeroController = zeroController;
	this->motorController = motorController;
	this->gripper = gripper;
	this->targetGenerator = targetGenerator;
}

CommandHandler::~CommandHandler() {
}

void CommandHandler::processCommand(ROBOT_IN *block) {
	if (pc->getState() == PC_READY && zeroController->getState() == ZR_IDLE && zeroController->isZeroed()) {
		//Commands that require pick process not started and ready
		std::array<axis_pos, NUM_AXES> target;
		switch (block->commandStruct.command) {
			case COMMAND_PICK_ITEM:
				pc->setState(PC_WAIT_FOR_MOTION);
				pc->setNextState(PC_PICK_COMMAND_RECEIVED);
				motorController->setTarget(Z, targetGenerator->getTopOfBoxZ());
				break;
			case COMMAND_TARGET:
				pc->setState(PC_TARGET_FOUND);
				pc->setNextState(PC_AT_PICK_POSITION_XY);
				std::copy(std::begin(block->commandStruct.axisCommand), std::end(block->commandStruct.axisCommand),
						std::begin(target));
				targetGenerator->setPickTarget(target);
				motorController->setTarget(X, block->commandStruct.axisCommand[X]);
				motorController->setTarget(Y, block->commandStruct.axisCommand[Y]);
				motorController->setTarget(Z, targetGenerator->getTopOfBoxZ());
				break;
			case COMMAND_AXIS:
				std::array<axis_pos, NUM_AXES> currentTarget;
				for (int i = 0; i < NUM_AXES; i++) {
					target[i] =
							block->commandStruct.axisCommand[i] <= 0 ?
									block->commandStruct.axisCommand[i] : currentTarget[i];
				}
				motorController->setTarget(target);
				pc->setState(PC_WAIT_FOR_MOTION);
				break;
		}
	}
	//Commands that don't require motion ready
	switch (block->commandStruct.command) {
		case COMMAND_NEW_BOX_ADDED:
			targetGenerator->newBoxAdded();
			break;
		case COMMAND_ZERO_IF_NEEDED:
			if (!zeroController->isZeroed() || pc->getState() == PC_NEEDS_ZERO) {
				pc->setState(PC_ZERO_RETURN);
			}
			break;
		case COMMAND_ZERO_RETURN:
			if ((pc->getState() == PC_READY || pc->getState() == PC_NEEDS_ZERO)
					&& !block->config.runtimeFlags.emergencyStop) {
				pc->setState(PC_ZERO_RETURN);
				ErrorHandler::getInstance()->reset();
			}
			break;
		case COMMAND_DROP_ITEM:
			if (pc->getState() == PC_AT_DROPOFF_XYZ) {
				gripper->deactivate();
				pc->setState(PC_ITEM_PLACED);
			}
			break;
		case COMMAND_PLACE:
			if (pc->getState() == PC_AT_DROPOFF_XYZ) {
				std::array<axis_pos, NUM_AXES> dropLocation;
				dropLocation[X] = block->commandStruct.axisCommand[X];
				dropLocation[Y] = motorController->getPosition(Y);
				dropLocation[Z] = block->commandStruct.axisCommand[Z];
				motorController->setTarget(dropLocation);
				pc->setState(PC_MOVE_TO_NEW_DROPOFF);
			}
			break;
		case COMMAND_LOAD_CONFIG:
			motorController->emergencyStop();
			motorController->updateConfig(block->config.axes);
			targetGenerator->updateConfig(&(block->config.targetGeneratorConfig));
			zeroController->clearZero();
			pc->setState(PC_READY);
			break;
		case COMMAND_VAC_ON:
			gripper->activate();
			break;
		case COMMAND_VAC_OFF:
			gripper->deactivate();
			break;
		case COMMAND_RESET:
			ErrorHandler::getInstance()->reset();
			for (int i = 0; i < NUM_AXES; i++) {
				motorController->softStop(static_cast<AXIS>(i));
			}
			pc->reset();
			zeroController->reset();
			zeroController->isZeroed() ? pc->setState(PC_READY) : pc->setState(PC_NEEDS_ZERO);
			gripper->deactivate();
			block->config.runtimeFlags.emergencyStop = false;
			break;
		case COMMAND_IDLE:
		default:
			break;
	}
}
