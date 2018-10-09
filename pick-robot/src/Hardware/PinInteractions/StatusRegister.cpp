/*
 * StatusRegister.cpp
 *
 *  Created on: Aug 20, 2018
 *      Author: masonuren
 */

#include "StatusRegister.h"

void StatusRegister::updateStatus(SlushMotor *motor) {
	this->numberOfErrors = 0;
	uint16_t retval = motor->getStatus();

	this->boardStatus[BOARD_STATUS::BF_HIGH_IMPEDANCE_STATE] = (retval & 0x1);
	this->boardStatus[BOARD_STATUS::BF_BUSY] = !((retval & 0x2) >> 1);
	this->boardStatus[BOARD_STATUS::BF_EXTERNAL_SWITCH_OPEN] = !((retval & 0x4) >> 2);
	this->boardStatus[BOARD_STATUS::BF_SWITCH_TURN_ON_EVENT] = (retval & 0x8) >> 3;
	this->boardStatus[BOARD_STATUS::BF_STEP_CLOCK_MODE] = (retval & 0x8000) >> 15;

	this->direction = (retval & 0x10) >> 4 ? DIRECTION::DIR_FORWARD : DIRECTION::DIR_BACKWARD;
	switch ((retval & 0x60) >> 5) {
		case 0x00:
			this->motorMotion = MOTOR_MOTION::MM_STOPPED;
			break;
		case 0x01:
			this->motorMotion = MOTOR_MOTION::MM_ACCEL;
			break;
		case 0x02:
			this->motorMotion = MOTOR_MOTION::MM_DECEL;
			break;
		case 0x03:
			this->motorMotion = MOTOR_MOTION::MM_CONSTANT_SPEED;
			break;
		default:
			printf("Bad motor status value returned from STATUS register.");
			break;
	}

	/*
	 * Active Low
	 */
	this->errorStatus[ERROR_STATUS::ES_UNDERVOLTAGE_LOCKOUT] = !((retval & 0x200) >> 9) ? EL_STOP_AND_ZERO : EL_NO_ERROR;
	this->errorStatus[ERROR_STATUS::ES_THERMAL_WARNING] = !((retval & 0x400) >> 10) ? EL_INFO : EL_NO_ERROR;
	this->errorStatus[ERROR_STATUS::ES_THERMAL_SHUTDOWN] = !((retval & 0x800) >> 11) ? EL_KILL : EL_NO_ERROR;
	this->errorStatus[ERROR_STATUS::ES_OVERCURRENT_DETECTION] = !((retval & 0x1000) >> 12) ? EL_STOP : EL_NO_ERROR;
	this->errorStatus[ERROR_STATUS::ES_SENSOR_STALL_DETECTED_ON_A] = !((retval & 0x2000) >> 13) ? EL_STOP_AND_ZERO : EL_NO_ERROR;
	this->errorStatus[ERROR_STATUS::ES_SENSOR_STALL_DETECTED_ON_B] = !((retval & 0x4000) >> 14) ? EL_STOP_AND_ZERO : EL_NO_ERROR;

	/*
	 * Active High
	 */
	this->errorStatus[ERROR_STATUS::ES_NONPERFORMABLE_COMMAND] = (retval & 0x80) >> 7 ? EL_INFO : EL_NO_ERROR;
	this->errorStatus[ERROR_STATUS::ES_WRONG_COMMAND] = (retval & 0x100) >> 8 ? EL_INFO : EL_NO_ERROR;

	this->checkForErrors();
}

void StatusRegister::checkForErrors() {
	this->numberOfErrors = 0;
	for (unsigned int i = 0; i < this->errorStatus.size(); i++) {
		if (this->errorStatus[i] > EL_NO_ERROR) {
			this->numberOfErrors++;
		}
	}
}

void StatusRegister::resetStatusReg(SlushMotor *motor) {
	/*
	 * getStatus() resets all bits in the STATUS reg, but the
	 * HiZ bit. This bit, if set high, will reset to low with any
	 * motion command.
	 */
	this->numberOfErrors = 0;
	motor->getStatus();
}

int StatusRegister::getNumberOfErrors() {
	return this->numberOfErrors;
}

DIRECTION StatusRegister::getDirectionOfTravel() {
	return this->direction;
}

MOTOR_MOTION StatusRegister::getMotionOfMotor() {
	return this->motorMotion;
}

std::array<ERROR_LEVEL, STATUS_REG_FLAGS> *StatusRegister::getErrorStatus() {
	return &this->errorStatus;
}

std::array<bool, BOARD_STATUS::BF_NUM_OF_FLAGS> *StatusRegister::getBoardStatus() {
	return &this->boardStatus;
}



