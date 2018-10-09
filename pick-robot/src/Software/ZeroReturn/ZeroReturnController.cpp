/*
 * ZeroReturnController.cpp
 *
 *  Created on: Jul 25, 2018
 *      Author: michaelkaiser
 */

#include "ZeroReturnController.h"

#include <SharedMemoryStructs.h>

#include "../../Utilities/Axis.h"
#include "../ErrorHandler/ErrorHandler.h"
#include "../MotorController/MotorController.h"

ZeroReturnController::ZeroReturnController(MotorController *mcObj) {
	mc = mcObj;
	state = ZR_IDLE;
	zeroed = false;
}

ZeroReturnController::~ZeroReturnController() {
}

void ZeroReturnController::step(long long int clockTicks) {
	ERROR_LEVEL currLevel = ErrorHandler::getInstance()->getErrorLevel();
	if (currLevel >= EL_STOP_AND_ZERO) {
		this->state = ZR_IDLE;
		this->zeroed = false;
	}
	switch (state) {
	case ZR_IDLE:
		break;
	case ZR_STARTED:
		this->zeroReturnAxisZ();
		break;
	case ZR_ZEROING_Z:
		this->checkLimitSwitchZ();
		break;
	case ZR_AT_Z_ZERO:
		this->zeroReturnAxesXY();
		break;
	case ZR_ZEROING_XY:
		this->checkLimitSwitchXY();
		break;
	case ZR_MOVE_OFF_LIM_SWITCH:
		this->moveToOffset();
		break;
	case ZR_MOVING_OFF_SWITCH:
		this->waitUntilAtOffset();
		break;
	case ZR_AT_ZERO:
		this->moveToStaging();
		break;
	case ZR_MOVING_TO_STAGING:
		this->waitUntilAtStaging();
	}
}

void ZeroReturnController::zeroReturnAxisZ() {
	mc->zeroReturnAxis(Z, DIRECTION::DIR_FORWARD);
	state = ZR_ZEROING_Z;
}

void ZeroReturnController::zeroReturnAxesXY() {
	mc->zeroReturnAxis(X, DIRECTION::DIR_BACKWARD);
	mc->zeroReturnAxis(Y, DIRECTION::DIR_BACKWARD);
	state = ZR_ZEROING_XY;
}

void ZeroReturnController::checkLimitSwitchZ() {
	if (mc->hasReachedTarget()) {
		state = ZR_AT_Z_ZERO;
	}
}

void ZeroReturnController::checkLimitSwitchXY() {
	if (mc->hasReachedTarget()) {
		state = ZR_MOVE_OFF_LIM_SWITCH;
	}
}

void ZeroReturnController::moveToOffset() {
	mc->moveMotorsOffLimitSwitches();
	state = ZR_MOVING_OFF_SWITCH;
}

void ZeroReturnController::waitUntilAtOffset() {
	if (mc->hasReachedTarget()) {
		mc->setMotorHomeLocation();
		zeroed = true;
		state = ZR_AT_ZERO;
	}
}

void ZeroReturnController::moveToStaging() {
	mc->moveToStaging();
	state = ZR_MOVING_TO_STAGING;
}

void ZeroReturnController::waitUntilAtStaging() {
	static bool once = false;
	if (!once) {
		if (mc->hasReachedTarget()) {
			once = true;
			mc->moveToStaging();
		}
	} else {
		if (mc->hasReachedTarget()) {
			once = false;
			state = ZR_IDLE;
		}
	}
}

void ZeroReturnController::start() {
	if (state == ZR_IDLE) {
		state = ZR_STARTED;
		this->zeroed = false;
	}
}

void ZeroReturnController::emergencyStop() {
	clearZero();
}

void ZeroReturnController::reportStatus(void *rout) {
	return;
}
