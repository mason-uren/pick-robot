/*
 * SimulatedVacuumSensor.cpp
 *
 *  Created on: Aug 7, 2018
 *      Author: masonuren
 */

#include "SimVacSensor.h"

void SimVacSensor::step(long long int) {

}

void SimVacSensor::reportStatus(void *robotOutPtr) {
	ROBOT_OUT * rout = (ROBOT_OUT *) robotOutPtr;
	rout->vacStatus.sensorValue = getCurrentSuctionValue();
	rout->vacStatus.suctionStatus = this->hasSuction() ? SUCTION::GOOD_SUCTION : SUCTION::BAD_SUCTION;
}

bool SimVacSensor::hasSuction() {
	return this->vacState == VACUUM_GRIPPER_STATE::VC_ON;
}

bool SimVacSensor::hasIndeterminateSuction() {
	return false; // Not being tested
}

double SimVacSensor::getCurrentSuctionValue() {
	return this->hasSuction() ? 0 : 1; // Arbitrary values
}

void SimVacSensor::setHighThresh(int high) {
	this->highThresh = high;
}

void SimVacSensor::setLowThresh(int low) {
	this->lowThresh = low;
}

void SimVacSensor::emergencyStop() {
	// Nothing to do
}

void SimVacSensor::setVacState(VACUUM_GRIPPER_STATE state) {
	this->vacState = state;
}
