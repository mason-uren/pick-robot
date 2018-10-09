/*
 * SimulatedMotor.h
 *
 *  Created on: Aug 3, 2018
 *      Author: masonuren
 */
#include <stdio.h>
#include <math.h>
#include <algorithm>
#include <ConfigStruct.h>

#include "../MotorInterface.h"

#ifndef SRC_HARDWARE_MOTORS_SIMULATEDMOTOR_H_
#define SRC_HARDWARE_MOTORS_SIMULATEDMOTOR_H_

/**
 * @class SimMotor
 * @brief A virtual motor.
 *
 * Does not establish physical interactions with hardware, but exists
 * 	as a way to test the logic of the Pick-Robot stepper motor software.
 */
class SimMotor : public MotorInterface {
public:
	/**
	 * @param[in] motorConfig A reference to how the motor should be configured.
	 */
	SimMotor(MOTOR_CONFIG *motorConfig);
	virtual ~SimMotor() {}

	/**
	 * @fn step
	 * Increment the motor in the appropriate direction till it has reached the
	 * 	desired #targetPosition.
	 */
	void step(long long int clockTicks);

	/**
	 * @fn reportStatus
	 * @brief *** Not Implemented ***
	 */
	void reportStatus(void *);

	/**
	 * @fn emergencyStop
	 * @brief *** Not implemented ***
	 */
	void emergencyStop();

	bool reachedTarget();
	void move(long steps);
	void goTo(axis_pos position);
	void goTo(axis_pos position, int stepsPerSec);
	double getMMPerRev() {
		return mmPerRev;
	}
	int getPositionInSteps();
	int getPositionInMM();

	/**
	 * @fn zeroReturn
	 * @brief Functions instantaneously by setting #home to 0.
	 * @param[in] dir Unused directional value.
	 */
	void zeroReturn(DIRECTION dir);

	/**
	 * @fn setHome
	 * @brief Sets #currentPosition to 0, then sets #home to #currentPosition.
	 */
	void setHome();

	/**
	 * @fn hardStop
	 * @brief Simulates immediate stop by setting #targetPostion to #currentPosition.
	 */
	void hardStop();

	/**
	 * @fn softStop
	 * @brief Simulates immediate deceleration and stop by setting #targetPosition to #currentPosition.
	 */
	void softStop();
	int getStepsPerRev() {
		return stepsPerRev;
	}
	void updateConfig(MOTOR_CONFIG * motorConfig);
	void setSpeed(double speed);
	double getMaxSpeed() {
		return maxStepsPerSec;
	}
	double mmToSteps(long desiredDist);

	/**
	 * @fn goToHome
	 * @brief Simulate going to home by setting the #currentPosition to 0.
	 */
	void goToHome();
	void moveOffOfLimitSwitches(long steps);

	/**
	 * @fn isLimitSwitchDepressed
	 * @return Is #currentPosition of the motor equal to 0.
	 */
	bool isLimitSwitchDepressed();

private:
	int maxStepsPerSec;			/**< The designated maximum steps/second the motor can travel. */
	double mmPerRev;			/**< The required number of millimeters to travel before completing a full motor revolution. */
	long stepsPerRev;			/**< The required number of steps to take before completing a full revolution. */
	int invert;					/**< Flag that determines if motor motions are reversed. */
	int epsilon;				/**< The acceptable margin of error in millimeters. */

	int motorAssignment;		/**< The motor ID. */
	double maxSpeed;			/**< The max speed of the motor in steps/second. */
	double minSpeed;			/**< The min speed of the motor in steps/second. */
	int home;					/**< The home location of the motor in millimeters. */
	float currentPosition;		/**< The current position of the motor in millimeters. */
	float targetPosition;		/**< The target position of the motor in millimeters. */

	/**
	 * @fn isBusy
	 * @return Checks that the #currentPosition is close to the #targetPosition within some
	 * 	reasonable margin of error #epsilon.
	 */
	bool isBusy();

	/**
	 * @fn stepsPerMs
	 * @return Dynamically calculate the steps-per-second based on the current max speed.
	 */
	float stepsPerMs();
};

#endif /* SRC_HARDWARE_MOTORS_SIMULATEDMOTOR_H_ */
