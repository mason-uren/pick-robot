#include "MotorController.h"

#include <SharedMemoryStructs.h>

#include "../../Hardware/Motors/MotorInterface.h"
#include "../ErrorHandler/ErrorHandler.h"


void MotorController::step(long long int clockTicks) {
	ERROR_LEVEL currLevel = ErrorHandler::getInstance()->getErrorLevel();
	if (currLevel >= EL_STOP && this->errorLevel < EL_STOP) {
		this->emergencyStop();
	}
	for (Axis *axis : this->axes) {
		for (MotorInterface *motor : axis->getMotorObj()) {
			motor->step(clockTicks);
		}
	}
	this->errorLevel = currLevel;
}

void MotorController::reportStatus(void *ptr) {
	ROBOT_OUT * rout = (ROBOT_OUT *) ptr;
	for (int i = 0; i < NUM_AXES; i++) {
		rout->axisStatus.axisPosition[i] = axes[i]->getCurrentPositionMM();
		rout->axisStatus.targetPosition[i] = this->targets[i];
	}
	rout->axisStatus.isBusy = !this->hasReachedTarget();
}

void MotorController::setTarget(AXIS axis, axis_pos position) {
	if (this->canMove(axis, position)) {
		targets[axis] = position;
		axes.at(axis)->goToTarget(position);
	}
}

void MotorController::setTarget(AXIS axis, axis_pos position, double speedMMPerSec) {
	if (this->canMove(axis, position)) {
		targets[axis] = position;
		axes.at(axis)->goToTarget(position, speedMMPerSec);
	}
}

void MotorController::setTarget(std::array<axis_pos, NUM_AXES> &position) {
	if (this->canMove(position)) {
		this->targets = position;
		for (Axis *axis : this->axes) {
			axis->goToTarget(this->targets[axis->getAxis()]);
		}
	}
}

void MotorController::setTarget(std::array<axis_pos, NUM_AXES> &position, double speedMMPerSec) {
	if (this->canMove(position)) {
		this->targets = position;
		for (Axis *axis : this->axes) {
			axis->goToTarget(this->targets[axis->getAxis()], speedMMPerSec);
		}
	}
}

std::array<axis_pos, NUM_AXES> MotorController::getTarget() {
	return this->targets;
}

void MotorController::addAxes(const std::array<Axis *, NUM_AXES> &axes) {
	this->axes = axes;
}

bool MotorController::hasReachedTarget() {
	for (Axis *axis : this->axes) {
		for (MotorInterface *motor : axis->getMotorObj()) {
			if (motor && !motor->reachedTarget()) {
				return false;
			}
		}
	}
	return true;
}

void MotorController::zeroReturnAxis(AXIS axis, DIRECTION dir) {
	if (ErrorHandler::getInstance()->getErrorLevel() < EL_STOP) {
		this->axes[axis]->zero(dir);
	}
}

// Called from Zero return control
// Currently the motor is on the limit switch, but the home position
// 	should exist  roughly 10mm off the switch
void MotorController::setMotorHomeLocation() {
	for (Axis *axis : this->axes) {
		axis->setAxisHome();
	}
}

void MotorController::moveToStaging() {
	std::array<axis_pos, NUM_AXES> stagingArea = this->getStagingLocation();
	this->setTarget(stagingArea);
}

void MotorController::softStop(AXIS axis) {
	this->axes[axis]->softStop();
}

void MotorController::emergencyStop() {
	ErrorHandler::getInstance()->emergencyStop();
	for (Axis *axis : this->axes) {
		axis->hardStop();
	}
}

void MotorController::updateConfig(AXIS_CONFIG * axisConfig) {
	for (unsigned char i = 0; i < axes.size(); i++) {
		switch (axisConfig[i].axisLabel) {
		case 'X':
			axes.at(X)->updateConfiguration(&(axisConfig[i]));
			break;
		case 'Y':
			axes.at(Y)->updateConfiguration(&(axisConfig[i]));
			break;
		case 'Z':
			axes.at(Z)->updateConfiguration(&(axisConfig[i]));
			break;
		}
	}
}
axis_pos MotorController::getPosition(AXIS axis) {
	return axes.at(axis)->getCurrentPositionMM();
}

axis_pos MotorController::getTravelLimit(AXIS axis) {
	return axes.at(axis)->getTravelLimit();
}

bool MotorController::canMove(const std::array<axis_pos, NUM_AXES> &proposedTarget) {
	bool isValid = false;
	for (Axis *axis : this->axes) {
		isValid = this->canMove(axis->getAxis(), proposedTarget[axis->getAxis()]);
	}
	return isValid;
}

bool MotorController::canMove(AXIS axis, axis_pos position) {
	if (!this->isTargetValid(axis, position)) {
		ErrorHandler::getInstance()->addError(ES_AXIS_TARGET_OUT_OF_BOUNDS, EL_STOP);
		return false;
	}
	return ErrorHandler::getInstance()->getErrorLevel() < EL_STOP;
}

bool MotorController::isTargetValid(AXIS axis, axis_pos target) {
	return this->withinAxisLimits(axis, target);
}

bool MotorController::withinAxisLimits(AXIS axis, axis_pos target) {
	return target <= 0 && target >= axes.at(axis)->getTravelLimit();
}

void MotorController::moveMotorsOffLimitSwitches() {
	if (ErrorHandler::getInstance()->getErrorLevel() < EL_STOP) {
		for (Axis *axis : this->axes) {
			axis->moveAxisOffLimitSwitches();
		}
	}
}

void MotorController::returnToAxisHome() {
	for (Axis *axes : this->axes) {
		axes->returnToHomePosition();
	}
}
