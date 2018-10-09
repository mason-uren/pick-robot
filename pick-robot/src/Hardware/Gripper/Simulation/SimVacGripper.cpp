/*
 * SimulatedVacuumGripper.cpp
 *
 *  Created on: Aug 7, 2018
 *      Author: masonuren
 */

#include "SimVacGripper.h"

SimVacGripper::SimVacGripper() {
	this->state = VC_OFF;
}

void SimVacGripper::activate() {
	this->state = VC_ON;
	simVacSensor.setVacState(this->state);
}

void SimVacGripper::deactivate() {
	this->state = VC_OFF;
	simVacSensor.setVacState(this->state);
}

void SimVacGripper::step(long long int clockTicks) {

}

void SimVacGripper::reportStatus(void *robotOutPtr) {
	ROBOT_OUT * rout = (ROBOT_OUT *) robotOutPtr;
	rout->vacStatus.isVacuumOn = state == VC_ON;
	simVacSensor.reportStatus(rout);
}

void SimVacGripper::emergencyStop() {
	this->deactivate();
}
