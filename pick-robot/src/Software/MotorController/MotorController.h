#ifndef SRC_UTILITIES_MOTORCONTROLLER_H_
#define SRC_UTILITIES_MOTORCONTROLLER_H_

#include <ConfigStruct.h>
#include <array>

#include "../../Utilities/Axis.h"
#include "../../Utilities/ComponentInterface.h"
#include "../../Hardware/PinInteractions/StatusRegister.h"


class Axis;

/**
 * @class MotorController
 * @brief The handler class responsible for all axis motion.
 *
 * Provides a unique interface that encapsulates motor motion commands,
 * 	while remaining ignorant to the motor type (ie. stepper, servo, etc.).
 * 	All motor commands are first checks whether the system is in an error state
 * 	then verifies that the designated commands are within axis limits.
 */
class MotorController : public ComponentInterface {
private:
	std::array<axis_pos, NUM_AXES> targets;		/**< The current axes target */
	std::array<Axis *, NUM_AXES> axes;			/**< An array of #Axis */
	ERROR_LEVEL errorLevel;						/**< The current priority error level */

	/**
	 * @fn canMove(AXIS axis, axis_pos position)
	 * @brief Checks whether the current position is valid.
	 *
	 * Using #isTargetValid, checks whether the passed desired position,
	 * 	is within the boundaries for that axis.
	 * @param[in] axis The axis of travel.
	 * @param[in] position The desired target position on the axis.
	 * @return Whether the target position exists on the current axis.
	 */
	bool canMove(AXIS axis, axis_pos position);

	/**
	 * @fn canMove(const std::array<axis_pos, NUM_AXES> &proposedTarget)
	 * @brief Checks whether the proposed target is valid.
	 *
	 * Loops through each axis and a determines using #isTargetValid whether
	 * 	the desired axis position is within the boundaries for that axis.
	 * @param[in] proposerdTarget An (x, y, z) coordinate triplet that represents the target
	 * 	robotic position.
	 * @return Whether the target coordinate triplet is within axes limits.
	 */
	bool canMove(const std::array<axis_pos, NUM_AXES> &proposedTarget);

public:

	/**
	 * @param[in] axes An reference to a set of three axes (x, y, z).
	 *
	 * Sets:
	 * 		- #targets : `{0, 0, 0}`
	 * 		- #axes : \p axes
	 * 		- #errorLevel : #EL_NO_ERROR
	 */
	MotorController(const std::array<Axis *, NUM_AXES> &axes)
		: 	targets{0, 0, 0},
			axes(axes),
			errorLevel(EL_NO_ERROR) {
	}
	virtual ~MotorController() {}

	/**
	 * @fn step
	 * @brief Checks for any persisting errors that would halt axis motion.
	 *
	 * If any errors persist that have an error level higher than #EL_INFO, then
	 * 	all axis motion should be stopped.
	 * 	@param[in] clockTicks The current clock tick in milliseconds.
	 */
	void step(long long int clockTicks);

	/**
	 * @fn reportStatus
	 * @brief Report the current axes positions to #ROBOT_OUT.
	 *
	 * Logs and reports the current axis position, in millimeters, the
	 * 	target axis position, and whether any of the axes are currently
	 * 	in motion.
	 * 	@param[in] rout A reference to #ROBOT_OUT.
	 */
	void reportStatus(void *);

	/**
	 * @fn getPosition
	 * @param[in] axis The desired axis.
	 * @return The current axis position in millimeters.
	 */
	int getPosition(AXIS axis);

	/**
	 * @fn getTarget
	 * @return The current valid target position.
	 */
	std::array<axis_pos, NUM_AXES> getTarget();

	/**
	 * @fn getStagingPosition
	 * @brief Get the set staging position for a desired axis.
	 * @param[in] axis The desired axis.
	 * @return The staging position in millimeters.
	 */
	long getStagingPosition(AXIS axis) {
		return axes[axis]->getStagingArea();
	}

	/**
	 * @fn getStagingLocation
	 * @brief Get the (x, y, z) set staging location.
	 * @return The coordinates of the staging area.
	 */
	std::array<axis_pos, NUM_AXES> getStagingLocation() {
		return std::array<axis_pos, NUM_AXES> {
			axes[X]->getStagingArea(),
			axes[Y]->getStagingArea(),
			axes[Z]->getStagingArea()
		};
	}

	/**
	 * @fn setTarget(std::array<axis_pos, NUM_AXES> &target)
	 * @brief Set the target coordinates (x, y, z), in millimeters.
	 *
	 * The passed target coordinate triplet will only be set if each
	 * 	target axis position exists within boundary limits, as checked by
	 * 	#canMove(const std::array<axis_pos, NUM_AXES> &proposedTarget).
	 * @param[in] target The desired target coordinate triplet.
	 */
	void setTarget(std::array<axis_pos, NUM_AXES> &target);

	/**
	 * @fn setTarget(std::array<axis_pos, NUM_AXES> &position, double speedMMPerSec)
	 * @brief Set the target coordinates (x, y, z), in millimeters, and the desired speed.
	 *
	 * The passed target coordinate triplet will only be set if each
	 * 	target axis position exists within boundary limits, as checked by
	 * 	#canMove(const std::array<axis_pos, NUM_AXES> &proposedTarget). The passed
	 * 	speed is the desired millimeters to travel/second.
	 * @param[in] position The desired target coordinate triplet.
	 * @param[in] speedMMPerSec The desired rate of travel, in millimeters/second.
	 */
	void setTarget(std::array<axis_pos, NUM_AXES> &position, double speedMMPerSec);

	/**
	 * @fn setTarget(AXIS axis, axis_pos position)
	 * @brief Set the target axis position for a given axis.
	 *
	 * Target position will only be set if the passed target exists within
	 * 	the passed axis's limits.
	 * @param[in] axis The desired axis to place target.
	 * @param[in] position The target position, in millimeters.
	 */
	void setTarget(AXIS axis, axis_pos position);

	/**
	 * @fn setTarget(AXIS axis, axis_pos position, double speedMMPerSec)
	 * @brief Set the target axis position, for a given axis, and its desired speed.
	 *
	 * Target position will only be set if the passed target exists within
	 * 	the passed axis's limits. The speeed is the desired millimeters to travel/second.
	 * @param[in] axis The desired axis to place target.
	 * @param[in] position The target position, in millimeters.
	 * @param[in] speedMMPerSec The desired rate of travel, in millimeters/second.
	 */
	void setTarget(AXIS axis, axis_pos position, double speedMMPerSec);

	/**
	 * @fn hasReachedTarget
	 * @brief Checks if any of the axes remain in motion.
	 * @return Determines whether each axis has reached its target location or
	 * 	remains in motion.
	 */
	bool hasReachedTarget();

	/**
	 * @fn addAxes
	 * @brief Change the x, y, and z axes.
	 * @param[in] axes A reference to the new x, y, and z axes.
	 */
	void addAxes(const std::array<Axis *, NUM_AXES> &axes);

	/**
	 * @fn zeroReturnAxis
	 * @brief Command an individual axis to zero return in a specific direction.
	 *
	 * Will check that Pick-Robot has not flagged an error of priority greater than
	 * 	#EL_INFO.
	 * @param[in] axis The current axis to zero.
	 * @param[in] dir The #DIRECTION of travel
	 */
	void zeroReturnAxis(AXIS axis, DIRECTION dir);

	/**
	 * @fn softStop
	 * @brief Issue a soft stop for a particular axis.
	 * @param[in] axis The axis to soft stop.
	 */
	void softStop(AXIS axis);

	/**
	 * @fn setMotorHomeLocation
	 * @brief Set the current motor location to the home location.
	 */
	void setMotorHomeLocation();

	/**
	 * @fn moveToStaging
	 * @brief Command the Pick-Robot to the previous specified staging area.
	 *
	 * Retrieves the current staging area for each axis, then sets next target
	 * 	location to the staging area, if the staging area is within all axes limits.
	 */
	void moveToStaging();

	/**
	 * @fn emergencyStop
	 * @brief Immediately stop all axis motion and reports to #ErrorHandler.
	 */
	void emergencyStop();

	/**
	 * @fn isTargetValid
	 * @brief Check if the target position is valid.
	 * @param[in] axis The target axis of motion.
	 * @param[in] position The passed target position.
	 */
	bool isTargetValid(AXIS axis, axis_pos position);

	/**
	 * @fn updateConfig
	 * @brief Updates each axis configuration.
	 * @param[in] axisConfig A reference to updated axis configuration for each axis.
	 */
	void updateConfig(AXIS_CONFIG *axisConfig);

	/**
	 * @fn getTravelLimits
	 * @brief Get the travel limits for the passed axis.
	 * @param[in] axis The axis to retrieve the travel limits for.
	 * @return The lower bound of the travel limits since the upper bound is always 0.
	 */
	int getTravelLimit(AXIS axis);

	/**
	 * @fn moveMotorsOffLimitSwitches
	 * @brief Move the motors off the limit switches.
	 *
	 * Commands each axis to move their respective motors off their corresponding limit
	 * 	switches, if Pick-Robot doesn't not have a persisting priority error above #EL_INFO.
	 */
	void moveMotorsOffLimitSwitches();

	/**
	 * @fn returnToAxisHome
	 * @brief Commands each motor to return to set home position.
	 *
	 * _IMPORTANT_: all axis will return home at same time.
	 */
	void returnToAxisHome();

	/**
	 * @fn withinAxisLimits
	 * @brief Checks that target is within the designated axis limits.
	 * @param[in] axis The target axis of motion.
	 * @param[in] target The desired axis target.
	 */
	bool withinAxisLimits(AXIS axis, axis_pos target);
};
#endif /* SRC_UTILITIES_MOTORCONTROLLER_H_ */
