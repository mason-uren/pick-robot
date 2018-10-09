/*
 * SimulatedMotor.cpp
 *
 *  Created on: Aug 3, 2018
 *      Author: masonuren
 */

#include "SimMotor.h"

int sign(double value) {
	return (value > 0) - (value < 0);
}

bool almostEqual(double x, double y, double thresh) {
	return abs(x - y) < thresh;
}

SimMotor::SimMotor(MOTOR_CONFIG *motorConfig)
	: maxStepsPerSec(motorConfig->maxStepsPerSec),
	  mmPerRev(motorConfig->mmPerRev),
	  stepsPerRev(motorConfig->stepsPerRev),
	  invert(motorConfig->invert ? -1 : 1),
	  epsilon(0.5),
	  motorAssignment(motorConfig->motorNumber),
	  maxSpeed(maxStepsPerSec),
	  minSpeed(MIN_STEPS_PER_SEC),
	  home(0), // Arbitrary
	  currentPosition(0), // Arbitrary
	  targetPosition(0) // Arbitrary
	{}

void SimMotor::step(long long int clockTicks) {
	if (isBusy()) {
		float increment = std::min(this->stepsPerMs(), std::fabs(this->currentPosition - this->targetPosition));
		this->currentPosition += increment * sign(this->targetPosition - this->currentPosition);
	}
}

void SimMotor::reportStatus(void *) {

}

void SimMotor::emergencyStop() {

}

bool SimMotor::reachedTarget() {
	return !this->isBusy();
}

// Move in the direction based on the sign of the steps
void SimMotor::move(long steps) {
	this->targetPosition = this->currentPosition + steps;
}

void SimMotor::goTo(axis_pos position) {
	this->setSpeed(this->maxStepsPerSec);
	this->targetPosition = position;
}

void SimMotor::goTo(axis_pos position, int stepsPerSec) {
	this->setSpeed(stepsPerSec);
	this->targetPosition = position;
}

int SimMotor::getPositionInSteps() {
	return this->currentPosition;
}

axis_pos SimMotor::getPositionInMM() {
	return (this->getMMPerRev() / this->getStepsPerRev()) * this->currentPosition;
}

void SimMotor::zeroReturn(DIRECTION dir) {
	this->setHome();
}

void SimMotor::setHome() {
	this->currentPosition = 0;
	this->home = this->currentPosition;
}

void SimMotor::hardStop() {
	this->targetPosition = this->currentPosition;
}

void SimMotor::softStop() {
	this->targetPosition = this->currentPosition;
}

void SimMotor::updateConfig(MOTOR_CONFIG * motorConfig) {
	this->invert = motorConfig->invert ? -1 : 1;
	this->maxStepsPerSec = motorConfig->maxStepsPerSec;
	this->mmPerRev = motorConfig->mmPerRev;
	this->stepsPerRev = motorConfig->stepsPerRev;
	this->maxSpeed = this->maxStepsPerSec;
}

double SimMotor::mmToSteps(long desiredDist) {
	return (this->getStepsPerRev() / this->getMMPerRev()) * desiredDist;
}

void SimMotor::goToHome() {
	this->currentPosition = 0;
}

// Intentionally left blank
void SimMotor::moveOffOfLimitSwitches(long steps) {

}

/*
 * Mock the depression of the limit switch by checking:
 * - currentPosition = 0
 */
bool SimMotor::isLimitSwitchDepressed() {
	return this->currentPosition == 0;
}

bool SimMotor::isBusy() {
	return this->currentPosition >= this->targetPosition + this->epsilon ||
			this->currentPosition <= this->targetPosition - this->epsilon;
}

float SimMotor::stepsPerMs() {
	return (float) this->maxSpeed / 1000.0;
}

void SimMotor::setSpeed(double speed) {
	float speedToSet = std::max(1.0, std::min(speed, (double) maxStepsPerSec));
	this->maxSpeed = speedToSet;
	this->minSpeed = MAGIC_MIN_SPEED;
}
