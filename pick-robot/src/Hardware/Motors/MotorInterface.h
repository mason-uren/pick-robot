
#ifndef SRC_MOTORS_MOTORINTERFACE_H_
#define SRC_MOTORS_MOTORINTERFACE_H_

/**
 * @file MotorInteface.h
 */

#include <ConfigStruct.h>
#include <SharedMemoryStructs.h>
#include "../../Utilities/ComponentInterface.h"
#include "../PinInteractions/StatusRegister.h"

/**
 * @typedef axis_pos
 * @brief Adds differentiability between an integer and a axis index.
 */
typedef int axis_pos;

/**
 * @def MIN_STEPS_PER_SEC
 * @brief The minimum steps/second the motor is must move.
 */
#define MIN_STEPS_PER_SEC 150
#define MAGIC_MIN_SPEED 20

/**
 * Stepping Style
 */
enum STEP_STYLE {
	FULL 				= 	0x00, /**< Full steps */
	HALF 				= 	0x01, /**< Half steps */
	QUARTER 			= 	0x02, /**< Quarter steps */
	EIGHTH				=	0x03, /**< One eighth of a step */
	SIXTEENTH			= 	0x04, /**< One sixteenth of a step */
	THIRTY_SECOND 		=	0x05, /**< One thirty-second of a step */
	SIXTY_FOURTH 		= 	0x06, /**< One sixty-fourth of a step */
	ONE_TWENTY_EIGHTH 	= 	0x07  /**< One twenty-eighth of a step */
};

/**
 * @interface MotorInterface
 * @brief A generic way of interacting with stepper motors.
 *
 * All motors, live or simulated, must implement this interface to be properly
 * 	integrated into the Pick-Robot software.
 */
class MotorInterface : public ComponentInterface {
public:
	virtual ~MotorInterface() {}

	/**
	 * @fn reachedTarget
	 * @return Has the current motor reached the desired target.
	 */
	virtual bool reachedTarget() = 0;

	/**
	 * @fn move
	 * @brief Move the motor in set number of steps in a desired direction
	 * 	depicted by sign convention (+/-).
	 * @param[in] steps The number of steps to take.
	 */
	virtual void move(long steps) = 0;

	/**
	 * @fn goTo(axis_pos position)
	 * @brief Go to a desired location within the motor's axis limits.
	 *
	 * If position exists outside of the commanding axis limits, the command will
	 * 	not be processed after throwing #ES_AXIS_TARGET_OUT_OF_BOUNDS. (Note: if one
	 * 	axis position if beyond the limits then target position will be rejected and
	 * 	the all axes will remain motionless.)
	 *
	 * @param[in] position The specified position the motor should travel.
	 */
	virtual void goTo(axis_pos position) = 0;

	/**
	 * @fn goTo(axis_pos position, int stepsPerSec)
	 * @brief Go to a desired location within the motor's axis limits,
	 * 	at a desired steps/sec.
	 *
	 * If position exists outside of the commanding axis limits, the command will
	 * 	not be processed after throwing #ES_AXIS_TARGET_OUT_OF_BOUNDS. (Note: if one
	 * 	axis position if beyond the limits then target position will be rejected and
	 * 	the all axes will remain motionless.)
	 * @param[in] position The specified position the motor should travel.
	 * @param[in] stepsPerSec The specified speed the motor should travel.
	 */
	virtual void goTo(axis_pos position, int stepsPerSec) = 0;

	/**
	 * @fn getMMPerRev
	 * @return The number of millimeters covered by one rotation.
	 */
	virtual double getMMPerRev() = 0;

	/**
	 * @fn getPositionInSteps
	 * @return The current motor position in steps from the home location.
	 */
	virtual int getPositionInSteps() = 0;

	/**
	 * @fn getPositionInMM
	 * @return The current motor position in millimeters from the home location.
	 */
	virtual axis_pos getPositionInMM() = 0;

	/**
	 * @fn zeroReturn
	 * @brief Command the motor to zero return in a specific direction.
	 * @param[in] dir The direction the motor should travel.
	 */
	virtual void zeroReturn(DIRECTION dir) = 0;

	/**
	 * @fn setHome
	 * @brief Set the current motor position as home (ie. 0).
	 */
	virtual void setHome() = 0;

	/**
	 * @fn hardStop
	 * @brief Trigger immediately stop motor with infinite deceleration.
	 */
	virtual void hardStop() = 0;

	/**
	 * @fn softStop
	 * @brief Trigger immediate deceleration till motor is stopped.
	 */
	virtual void softStop() = 0;

	/**
	 * @fn getStepsPerRev
	 * @return The necessary steps the motor needs to take to complete a full revolution.
	 */
	virtual int getStepsPerRev() = 0;

	/**
	 * @fn updateConfig
	 * @brief Update the current motor configuration.
	 * @param[in] motorConfig A pointer to the new motor configuration.
	 */
	virtual void updateConfig(MOTOR_CONFIG * motorConfig) = 0;

	/**
	 * @fn setSpeed
	 * @brief Set the speed, in steps per second, that the motor should move.
	 * @param[in] stepsPerSec Steps per second
	 */
	virtual void setSpeed(double stepsPerSec) = 0;

	/**
	 * @fn getMaxSpeed
	 * @return The currently specified max speed of the motor in steps per second.
	 */
	virtual double getMaxSpeed() = 0;

	/**
	 * @fn mmToSteps
	 * @param[in] desiredDist The desired distance the motor should move in millimeters.
	 * @return The necessary amount of steps to take to achieve the passed distance.
	 */
	virtual double mmToSteps(long desiredDist) = 0;

	/**
	 * @fn goToHome
	 * @brief Return the motor to the current home position.
	 *
	 * This command should only be processed if a home position has been
	 * 	previously been explicitly specified.
	 */
	virtual void goToHome() = 0;

	/**
	 * @fn moveOffOfLimitSwitches
	 * @brief Move the motor away from the currently depressed limit switch.
	 *
	 * Will move each axes away from their respective limit switch until the it is no longer depressed.
	 *  The release of the limit switch will immediately stop the axis motion.
	 * @param[in] steps The number of steps necessary to depress the current limit switch.
	 */
	virtual void moveOffOfLimitSwitches(long steps) = 0;
};


#endif /* SRC_MOTORS_MOTORINTERFACE_H_ */
