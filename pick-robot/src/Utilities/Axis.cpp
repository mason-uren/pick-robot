#include "Axis.h"

#include <cmath>

/**
 * Goes at max speed
 */
void Axis::goToTarget(axis_pos positionMm) {
	for (MotorInterface *motor : this->assignedMotors) {
		if (motor != nullptr) {
			motor->goTo(motor->mmToSteps(positionMm));
		}
	}
}

/**
 * Goes at specified speed
 */
void Axis::goToTarget(axis_pos positionMm, double mmPerSec) {
	for (MotorInterface *motor : this->assignedMotors) {
		if (motor != nullptr) {
			motor->goTo(motor->mmToSteps(positionMm), motor->mmToSteps(mmPerSec));
		}
	}
}

/*
 * Grouped motors should be the same thus one value will be returned
 */
axis_pos Axis::getCurrentPositionMM() {
	if (this->assignedMotors.size() < 1) {
		return 0;
	}
	return this->assignedMotors[0]->getPositionInMM();
}

std::vector<MotorInterface *> Axis::getMotorObj() {
	return this->assignedMotors;
}

AXIS Axis::getAxis() {
	return this->axis;
}

//TODO:
void Axis::zero(DIRECTION dir) {
	for (MotorInterface *motor : this->assignedMotors) {
		motor->zeroReturn(dir);
	}
}

void Axis::moveAxisOffLimitSwitches() {
	for (MotorInterface *motor : this->assignedMotors) {
		motor->moveOffOfLimitSwitches(HOME_POSITION_OFFSET);
	}
 }

void Axis::setAxisHome() {
	for (MotorInterface *motor : this->assignedMotors) {
		motor->setHome();
	}
}

void Axis::hardStop() {
	for (MotorInterface *motor : this->assignedMotors) {
		motor->hardStop();
	}
}

void Axis::softStop() {
	for (MotorInterface *motor : this->assignedMotors) {
		motor->softStop();
	}
}

void Axis::updateConfiguration(AXIS_CONFIG *axis) {
	this->stagingArea = axis->stagingArea;
	this->travelLimit = axis->travelLimitmm;
	for (unsigned int i = 0; i < this->assignedMotors.size(); i++) {
		this->assignedMotors.at(i)->updateConfig(&(axis->motor[i]));
	}
}

void Axis::returnToHomePosition() {
	for (MotorInterface *motor : this->assignedMotors) {
		motor->goToHome();
	}
}
