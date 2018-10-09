/*
 * MotorFactory.cpp
 *
 *  Created on: Aug 14, 2018
 *      Author: masonuren
 */

#include "MotorFactory.h"

MotorInterface* MotorFactory::create(bool simulate, MOTOR_CONFIG *motorConfig) {
	if (simulate) {
		return new SimMotor(motorConfig);
	}
	else {
		return new StepperMotor(motorConfig);
	}
}


