#include "StepperMotor.h"

#include <l6470.h>
#include <l6470constants.h>
#include <slushmotor.h>
#include <algorithm>
#include <cstdio>

StepperMotor::StepperMotor(MOTOR_CONFIG * motorConfig) {
	this->maxStepsPerSec = motorConfig->maxStepsPerSec;
	this->invert = motorConfig->invert ? -1 : 1;
	this->mmPerRev = motorConfig->mmPerRev;
	this->stepsPerRev = motorConfig->stepsPerRev;
	this->motor = new SlushMotor(motorConfig->motorNumber);
	this->motor->resetDev();
	this->motor->setCurrent(motorConfig->holdCurrent, motorConfig->runCurrent, motorConfig->accelCurrent,
			motorConfig->decelCurrent);
	this->motor->setMaxSpeed(maxStepsPerSec);
	this->motor->setFullSpeed(1500);
	this->motor->setMinSpeed(MIN_STEPS_PER_SEC);
	this->statusRegister = StatusRegister();
}

StepperMotor::~StepperMotor() {

}

void StepperMotor::step(long long int clockTicks) {
	// Caution: checking the flags set by the STATUS register resets the warning/error flags
	this->statusRegister.updateStatus(this->motor);
	std::array<ERROR_LEVEL, STATUS_REG_FLAGS> *errorStatus = this->statusRegister.getErrorStatus();
	if (this->statusRegister.getNumberOfErrors()) {
		for (unsigned int status = 0; status < errorStatus->size(); status++) {
			if (errorStatus->at(status) > EL_NO_ERROR) {
				ErrorHandler::getInstance()->addError(static_cast<ERROR_STATUS>(status), errorStatus->at(status));
			}
		}
	}
}

void StepperMotor::reportStatus(void *robotOutPtr) {
	ROBOT_OUT *rout = (ROBOT_OUT*) robotOutPtr;
	std::array<bool, BOARD_STATUS::BF_NUM_OF_FLAGS> *boardStatus = this->statusRegister.getBoardStatus();
	for (unsigned int i = 0; i < boardStatus->size(); i++) {
		rout->mDebug.board[i] = boardStatus->at(i);
	}

	DIRECTION dir = this->statusRegister.getDirectionOfTravel();
	rout->mDebug.slushDir = dir;
	rout->mDebug.direction = (this->invert) ? this->invertDirection(dir) : dir;
	rout->mDebug.motorMotion = this->statusRegister.getMotionOfMotor();
	rout->mDebug.motor = this->motor->GetMotorNumber();
}

void StepperMotor::emergencyStop() {
	this->motor->hardStop();
}

bool StepperMotor::reachedTarget() {
	// If motor is busy, has not reached target
	return !this->motor->isBusy();
}

bool StepperMotor::isLimitSwitchDepressed() {
	return !(this->motor->getStatus() & L6470_STATUS_SW_F);
}

void StepperMotor::hardStop() {
	this->motor->hardStop();
}

void StepperMotor::softStop() {
	this->motor->softStop();
}

void StepperMotor::move(long steps) {
	this->motor->move(this->stepsToMicroSteps(steps));
}

void StepperMotor::goTo(axis_pos position) {
	setSpeed(maxStepsPerSec);
	this->motor->goTo((int) this->stepsToMicroSteps(position));
}

void StepperMotor::goTo(axis_pos position, int stepsPerSec) {
	setSpeed(stepsPerSec);
	this->motor->goTo((int) this->stepsToMicroSteps(position));
}

void StepperMotor::setSpeed(double speed) {
	float speedToSet = std::max(1.0, std::min(speed, (double) maxStepsPerSec));
	this->motor->setMaxSpeed(std::abs(speedToSet));
	this->motor->setMinSpeed(MAGIC_MIN_SPEED);
}


void StepperMotor::zeroReturn(DIRECTION dir) {
	this->motor->setMaxSpeed(400.0);
	this->motor->setMinSpeed(200.0);
	if (StepperMotor::isLimitSwitchDepressed()) {
		this->motor->setAsHome();
	} else {
		switch (dir) {
			case DIRECTION::DIR_FORWARD:
				this->motor->releaseSw(L6470_ABSPOS_RESET, L6470_DIR_FWD);
				break;
			case DIRECTION::DIR_BACKWARD:
				this->motor->releaseSw(L6470_ABSPOS_RESET, L6470_DIR_REV);
				break;
			default:
				perror("Bad directional value in StepperMotor::zeroReturn()");
				break;
		}
	}
}

void StepperMotor::setHome() {
	this->motor->setAsHome();
}

int StepperMotor::getPositionInSteps() {
	return this->motor->getPos();
}

int StepperMotor::stepsToMicroSteps(int steps) {
	/*
	 * The move(steps) commands is a microstep command
	 * (ie 128 micro-steps = 1 step)
	 */
	int microStepsPerStep = 128;
	return steps * microStepsPerStep * this->invert;
}

int StepperMotor::microstepsToSteps(int microsteps) {
	int microStepsPerStep = 128;
	return microsteps / microStepsPerStep * this->invert;
}

void StepperMotor::setSteppingMode(STEP_STYLE mode) {
	this->motor->setParam(L6470_PARAM_STEP_MODE, (unsigned long) mode);
}

axis_pos StepperMotor::getPositionInMM() {
	return round((this->getMMPerRev() / this->getStepsPerRev()) * microstepsToSteps(this->motor->getPos()));
}

void StepperMotor::updateConfig(MOTOR_CONFIG * motorConfig) {
	this->invert = motorConfig->invert ? -1 : 1;
	this->maxStepsPerSec = motorConfig->maxStepsPerSec;
	this->mmPerRev = motorConfig->mmPerRev;
	this->stepsPerRev = motorConfig->stepsPerRev;
	this->motor->setCurrent(motorConfig->holdCurrent, motorConfig->runCurrent, motorConfig->accelCurrent,
			motorConfig->decelCurrent);
	this->motor->setMaxSpeed(maxStepsPerSec);
}

double StepperMotor::getMaxSpeed() {
	return maxStepsPerSec;
}

double StepperMotor::mmToSteps(long desiredDist) {
	return (this->getStepsPerRev() / this->getMMPerRev()) * desiredDist;
}

void StepperMotor::goToHome() {
	motor->goHome();
}

void StepperMotor::moveOffOfLimitSwitches(long steps) {
	this->motor->setMaxSpeed(100);
	this->motor->setMinSpeed(50);
	this->motor->move(this->stepsToMicroSteps(steps));
}

DIRECTION StepperMotor::invertDirection(DIRECTION dir) {
	return dir == DIR_FORWARD ? DIR_BACKWARD : DIR_FORWARD;
}
