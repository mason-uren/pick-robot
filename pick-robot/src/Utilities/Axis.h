#ifndef SRC_UTILITIES_AXIS_H_
#define SRC_UTILITIES_AXIS_H_

/**
 * @file Axis.h
 */
#include <ConfigStruct.h>
#include <SharedMemoryStructs.h>
#include <iostream>
#include <vector>

#include "../Hardware/Motors/MotorInterface.h"

/** The number of allowable axes. */
#define NUM_AXES 3
/** Number of allowable motors per axis. */
#define MAX_MOTORS_PER_AXIS 2

/** Distance in millimeters to travel off of limit switches, when setting home postion. */
#define HOME_POSITION_OFFSET -10

inline char getAxisLetter(AXIS axis) {
	switch (axis) {
	case X:
		return 'X';
	case Y:
		return 'Y';
	case Z:
		return 'Z';
	}
	return '\0';
}

/**
 * @class Axis
 * @brief Responsible for motion on passed axis.
 *
 * Handles the motion of either one or two motors, depending on how many
 * 	are allocated to the desired axis. Encapsulates the #StepperMotor object, by
 * 	providing #MotorController a generic way to produces axis motion.
 */
class Axis {
public:
	/**
	 * @param[in] axisConfig A reference to the configuration for each axis.
	 * @param[in] assignedMotors Vector of the allocated motors for the desired axis.
	 *
	 * Sets:
	 * 		- #axis : @p axis
	 * 		- #travelLimit : @p axisConfig->travelLimitmm
	 * 		- #stagingArea : @p axisConfig->stagingArea
	 */
	Axis(AXIS axis, AXIS_CONFIG * axisConfig, const std::vector<MotorInterface *> &assignedMotors) :
		axis(axis),
		travelLimit(axisConfig->travelLimitmm),
		assignedMotors(assignedMotors),
		stagingArea(axisConfig->stagingArea)
		{
			std::cout << "Axis Created." << std::endl;
		}
	virtual ~Axis() {}

	/**
	 * @fn goToTarget(axis_pos positionMm, double mmPerSec)
	 * @brief Command the motors to a specific axis position, at a set speed.
	 *
	 * If position is within axis limits, move assigned motors to desired postion
	 *  at designated speed, in millimeters/second.
	 * @param[in] postion Desired axis position.
	 * param[in] mmPerSec Set speed of motor travel in millimeters/second.
	 */
	void goToTarget(axis_pos positionMm, double mmPerSec);

	/**
	 * @fn goToTarget(axis_pos positionMm)
	 * @brief Command the motors to a specific axis position, if with in axis limits.
	 * @param[in] positionMm Target axis location in millimeters.
	 */
	void goToTarget(axis_pos positionMm);

	/**
	 * @fn getCurrentPositionMM
	 * @return Current axis position in millimeters.
	 */
	axis_pos getCurrentPositionMM();

	/**
	 * @fn getMotorObj
	 * @return A reference to the allocated motors for the axis.
	 */
	std::vector<MotorInterface *> getMotorObj();

	/**
	 * @fn getAxis
	 * @return The axis ID.
	 */
	AXIS getAxis();

	/**
	 * @fn zero
	 * @brief An axis motion command to zero the current axis.
	 * @param[in] dir The direction to zero.
	 */
	void zero(DIRECTION dir);

	/**
	 * @fn moveAxisOffLimitSwitches
	 * @brief Move the axis motors off of the limit switches.
	 */
	void moveAxisOffLimitSwitches();

	/**
	 * @fn setAxisHome
	 * @brief Set the current motor position to the home position for the allocated motors.
	 */
	void setAxisHome();

	/**
	 * @fn hardStop
	 * @brief Immediately stop axis motion.
	 */
	void hardStop();

	/**
	 * @fn softStop
	 * @brief Stop axis motion, respecting set deceleration.
	 */
	void softStop();

	/**
	 * @fn getTravelLimit
	 * @return Lower bound of axis travel limit since upper bound is always 0.
	 */
	axis_pos getTravelLimit() {
		return travelLimit;
	}

	/**
	 * @fn mmToSteps
	 * @brief Convert millimeters into steps.
	 * @param[in] mmDist
	 * @return The number of full steps needed to travel the millimeter distance.
	 */
	int mmToSteps(long mmDist);

	/**
	 * @fn updateConfiguration
	 * @brief Update the current axis configurations.
	 *
	 * Updates the #stagingArea, #travelLimit, and #assignedMotors.
	 */
	void updateConfiguration(AXIS_CONFIG *axis);

	/**
	 * @fn getStagingArea
	 * @return The set staging area.
	 */
	axis_pos getStagingArea() {
		return stagingArea;
	}

	/**
	 * @fn returnToHomePosition
	 * @brief Returns the allocated axis motors to their home positions.
	 */
	void returnToHomePosition();

private:
	/** Axis ID. */
	AXIS axis;
	/** Axis travel limit. */
	axis_pos travelLimit;
	/** Allocated motors for the axis. */
	std::vector<MotorInterface *> assignedMotors;
	/** Desired staging area. */
	axis_pos stagingArea;
};

#endif /* SRC_UTILITIES_AXIS_H_ */
