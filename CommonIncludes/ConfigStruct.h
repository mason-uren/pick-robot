#ifndef CONFIGSTRUCT_H
#define CONFIGSTRUCT_H
#include <stddef.h>
/**
 * @file ConfigStruct.h
 */


/**
 * @typedef Target Gereration Config
 * @brief Coordinates that determine box dimensions, drop location, and distance between picks.
 */
typedef struct {
	int boxStart[3];			/**< The beginning of the box (XYZ) */
	int boxEnd[3];				/**< The end of the box (XYZ) */
	int delta[3];				/**< The dimensions of the items to be picked */
	int dropLocation[3];		/**< The desired drop location (XYZ) */
} TARGET_GENERATOR_CONFIG;

/**
 * @typedef Motor Configuration
 * @brief Configuration of the motor that determines motor ID, speeds/accel (max/min), and steps.
 */
typedef struct {
	bool valid;				/**< Is passed motor configuration valid */
	int motorNumber;		/**< Motor ID */
	int accelCurrent;		/**< Passed current used for motor acceleration */
	int decelCurrent;		/**< Passed current used for motor deceleration */
	int holdCurrent;		/**< Passed current used to hold the motor steady */
	int runCurrent;			/**< Passed current used to move the motor */
	int maxStepsPerSec;		/**< Max steps the motor can take per second */
	int stepsPerRev;		/**< Steps necessary for one revolution of motor */
	double mmPerRev;		/**< Millimeters traveled per motor revolution */
	bool invert;			/**< Slushengine: invert the commanded motor direction */
} MOTOR_CONFIG;

/**
 * @typedef Axis Configuration
 * @brief Determines which axis, the motors on the axis, the staging area, and travel limits.
 */
typedef struct {
	bool valid;				/**< Is passed axis configuration valid */
	MOTOR_CONFIG motor[2];	/**< Number of motors on axis */
	char axisLabel;			/**< Label of axis */
	int stagingArea;		/**< Desired staging area for axis */
	int travelLimitmm;		/**< Travel limits of axis */
} AXIS_CONFIG;

/**
 * @typedef Runtime Flags
 * @brief Flags that determine the high level actions of pick-robot
 */
typedef struct {
	bool realtime;			/**< Is the pick-robot running in realtime */
	bool simulate;			/**< Is axis motion simulated */
	bool logAxesData;		/**< Is the pick-robot recording all axes motion */
	bool ignoreErrorFlags;	/**< Is the pick-robot ignoring error messages */
	bool emergencyStop; 	/**< Has an emergency stop been commanded */
} RUNTIME_FLAGS;

/**
 * @typedef JSON Configuration
 */
 typedef struct {
	RUNTIME_FLAGS runtimeFlags;						/**< Desired runtime flags */
	AXIS_CONFIG axes[3];							/**< Desired axes configurations */
	TARGET_GENERATOR_CONFIG targetGeneratorConfig;	/**< Target generation */
	size_t hash;
} JSON_CONFIG;

#endif
