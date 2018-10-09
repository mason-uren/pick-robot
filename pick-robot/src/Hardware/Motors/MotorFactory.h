/*
 * Motor.h
 *
 *  Created on: Aug 10, 2018
 *      Author: masonuren
 */

#ifndef SRC_HARDWARE_MOTORS_MOTORFACTORY_H_
#define SRC_HARDWARE_MOTORS_MOTORFACTORY_H_

#include "StepperMotor.h"
#include "Simulation/SimMotor.h"
#include <ConfigStruct.h>
#include "MotorInterface.h"

/**
 * @class MotorFactory
 * @brief Factory implementation that creates either a live or simulated motor.
 */
class MotorFactory {
public:

	/**
	 * @static create
	 * @brief Creates the desired #MotorInterface (live or simulated).
	 * @param[in] simulate A flag the determines whether the motor actions should be
	 * 	live or simulated.
	 * @param[in] motorConfig A reference to how the motor should be configured.
	 * @return A reference to the created motor.
	 */
	static MotorInterface* create(bool simulate, MOTOR_CONFIG *motorConfig);

};



#endif /* SRC_HARDWARE_MOTORS_MOTORFACTORY_H_ */
