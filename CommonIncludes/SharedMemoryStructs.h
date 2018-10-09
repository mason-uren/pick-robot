#ifndef SHAREDMEMORYSTRUCTS_H
#define SHAREDMEMORYSTRUCTS_H

/**
 * @file SharedMemoryStructs.h
 */

#include "ConfigStruct.h"

/**
 * @def SEC_TO_MILL
 * @brief A define value that is used for converting
 */
#define SEC_TO_MILL 1000

/**
 *  Pick-Robot Commands
 *  @brief Commands that the robot can process.
 */
enum COMMAND {
	COMMAND_IDLE = 0, 				/**< IDLE: not implemented */
	COMMAND_EMERGENCY_STOP = 1,		/**< Stop all axis motion and turn off vacuum, requires re-zero */
	COMMAND_PICK_ITEM = 2,			/**< Pick an item */
	COMMAND_VAC_ON = 3,				/**< Turn on the vacuum */
	COMMAND_VAC_OFF = 4,			/**< Turn off the vacuum */
	COMMAND_ZERO_STAGE = 5,			/**< ZERO_STAGE: not implemented. */
	COMMAND_ZERO_RETURN = 6,		/**< Zero each axis */
	COMMAND_AXIS = 7,				/**< Move to a location, within the axis limits */
	COMMAND_LOAD_CONFIG = 8,		/**< Load the 'default_config.json' */
	COMMAND_DROP_ITEM = 9,			/**< Drop an item */
	COMMAND_ZERO_IF_NEEDED = 10,	/**< Re-zero the each axis */
	COMMAND_NEW_BOX_ADDED = 11,		/**< Let the pick-robot know a new box has been added */
	COMMAND_RESET = 12,				/**< Stop all axis motion and turn off vacuum and reset error flags; doesn't require re-zero */
	COMMAND_TARGET = 13,			/**< Pick an item from a specific location (turns on vacuum) */
	COMMAND_PLACE = 14				/**< Place an item in a specific location, within axis limits */
};

/**
 * Pick Controller States
 */
enum PICK_STATE {
	PC_VAC_ON,							/**< The vacuum has been turned on manually */
	PC_VAC_OFF,							/**< The vacuum has been turned off manually */
	PC_ERROR,							/**< The pick controller is in a error state */
	PC_READY,							/**< The pick controller is in a ready state (do nothing state) */
	PC_PICK_COMMAND_RECEIVED,			/**< The pick controller has been commanded to begin the pick routine */
	PC_TARGET_FOUND,					/**< A (x, y, z) pick location has been determined */
	PC_AT_PICK_POSITION_XY,				/**< Command the x and y axes to move, hold z axis steady */
	PC_MOVING_ABOVE_PICK,				/**< Wait till the pick-robot has reached the (x, y) target location */
	PC_AT_PICK_POSITION_XY_ABOVE_Z,		/**< Pick-robot above item, turn on vacuum and command z axis toward item */
	PC_PROBING,							/**< Check if pick-robot has suction or has reached desired z target depth */
	PC_HAS_ITEM,						/**< Pick-robot has good suction, command z axis back to box clearance height */
	PC_RAISING_ARM,						/**< Wait for the z axis to reach clearance height */
	PC_AT_PICK_POSITION_Z_CLEARANCE,	/**< Command x and y axes to drop location, hold z axis steady */
	PC_MOVING_TO_DROPOFF_XY,			/**< Wait for x and y axes to reach drop location (continually check that we have suction) */
	PC_AT_DROPOFF_XY,					/**< At x and y drop off locations, command z axis to drop off location */
	PC_MOVING_TO_DROPOFF_XYZ,			/**< Wait for z axis to reach drop location, hold x and y axes steady */
	PC_AT_DROPOFF_XYZ,					/**< Reached drop location (do nothing state) */
	PC_ITEM_PLACED,						/**< Turn off suction, command robot to staging area */
	PC_AT_Z_CLEARANCE_RETURN,			/**< Wait for robot to reach staging area */
	PC_WAIT_FOR_MOTION,					/**< Wait for axis motion to complete */
	PC_ZERO_RETURN,						/**< Command pick-robot to zero axes */
	PC_ZERO_RETURN_WAIT,				/**< Wait for zeroing to complete */
	PC_NEEDS_ZERO,						/**< Re-zero pick-robot */
	PC_MOVE_TO_NEW_DROPOFF				/**< Command pick-robot to move to new dropoff location, then release item */
};

/**
 * Runtime Errors
 */
enum ERROR_STATUS {
	ES_NONPERFORMABLE_COMMAND = 0,		/**< Slushengine: Cannot perform operation (may still be valid command) */
	ES_WRONG_COMMAND,					/**< Slushengine: Did not find the desired command (command not found) */
	ES_UNDERVOLTAGE_LOCKOUT,			/**< Slushengine: Motor supply voltage low */
	ES_THERMAL_WARNING,					/**< Slushengine: Internal temperature exceeded preset thermal warning threshold */
	ES_THERMAL_SHUTDOWN,				/**< Slushengine: Internal temperature exceeded preset thermal shutdown level (Power bridges have been disabled) */
	ES_OVERCURRENT_DETECTION,			/**< Slushengine: Power MOSFETs have exceeded a programmed over-current threshold */
	ES_SENSOR_STALL_DETECTED_ON_A,		/**< Slushengine: Speed and/or load angle caused motor stall on A */
	ES_SENSOR_STALL_DETECTED_ON_B,		/**< Slushengine: Speed and/or load angle caused motor stall on B */
	ES_AXIS_TARGET_OUT_OF_BOUNDS,		/**< Desired target is beyond axis limits */
	ES_VACUUM_SENSOR_MISREAD,			/**< Incorrect suction value is read (based on suction value and vacuum state) */
	ES_NUM_OF_FLAGS						/**< The number of possible error flags */
};

/**
 * Board Status Flags
 */
enum BOARD_STATUS {
	BF_HIGH_IMPEDANCE_STATE = 0, 		/**< Slushengine: the HIZ bit has been switched high */
	BF_BUSY,							/**< The motors are in motion */
	BF_EXTERNAL_SWITCH_OPEN,
	BF_SWITCH_TURN_ON_EVENT,
	BF_STEP_CLOCK_MODE,
	BF_NUM_OF_FLAGS						/**< The number of possible status flags */
};

/**
 * Suction States
 */
enum SUCTION {
	BAD_SUCTION = 0, 			/**< Vacuum sensor reads no suction */
	INDETERMINATE_SUCTION, 		/**< Vacuum sensor cannot distinguish between good/bad suction */
	GOOD_SUCTION				/**< Vacuum sensor reads good suciton */
};

/**
 * Vacuum States
 */
enum VACUUM_GRIPPER_STATE {
	VC_ON = 0, 	/**< Vacuum is on */
	VC_OFF, 	/**< Vacuum is off */
	VC_ERROR	/**< State not implemented */
};

/**
 * Motor Motion
 */
enum MOTOR_MOTION {
	MM_STOPPED = 0, 		/**< Motor is stopped */
	MM_ACCEL, 				/**< Motor is accelerating */
	MM_DECEL, 				/**< Motor is decelerating */
	MM_CONSTANT_SPEED		/**< Motor is moving at a constant speed */
};

/**
 * Motor Direction
 */
enum DIRECTION {
	DIR_FORWARD = 0, 		/**< Motor is moving in the forward direction */
	DIR_BACKWARD			/**< Motor is moving in the backward direction */
};

/**
 * Error Levels
 */
enum ERROR_LEVEL {
	EL_NO_ERROR = 0, 		/**< No errors */
	EL_INFO, 				/**< Informational (warning) */
	EL_STOP, 				/**< Immediately stop axes */
	EL_STOP_AND_ZERO, 		/**< Immediately stop axes and require re-zero */
	EL_KILL					/**< Unrecoverable error, Immediately stop axes and require system reboot */
};

/**
 * Pick-Robot Axes
 */
enum AXIS {
	X = 0, 			/**< X-axis */
	Y, 				/**< Y-axis */
	Z				/**< Z-axis */
};

/**
 * @typedef Operating Errors
 */
typedef struct {
	ERROR_LEVEL errors[ES_NUM_OF_FLAGS]; 	/**< An array of reported errors */
	ERROR_LEVEL priorityError;				/**< The most critical reported errors */
	int numberOfErrors;						/**< The number of reported errors */
} OPERATING_ERRORS;

/**
 * @typedef Motor Debugging
 */
typedef struct {
	bool board[BF_NUM_OF_FLAGS];	/**< An array of board status flags */
	MOTOR_MOTION motorMotion;		/**< The current motor motion status */
	DIRECTION direction;			/**< The direction of motor motion */
	DIRECTION slushDir;				/**< The direction passed to the Slushengine */
	int motor;						/**< The current motor */
} MOTOR_DEBUG;

/**
 * @typedef Axis Status
 */
typedef struct {
	int targetPosition[3];		/**< The target axis position */
	int axisPosition[3];		/**< The current axis position */
	bool isBusy;				/**< Is the current axis already in motion */
} AXIS_STATUS;

/**
 * @typedef Pick Control Status
 */
typedef struct {
	PICK_STATE state;			/**< The current pick controller state */
	long int itemsPicked;		/**< The number of items successfully picked */
} PC_STATUS;

/**
 * @typedef Vacuum Information
 */
typedef struct {
	double sensorValue;			/**< The current read suction value */
	bool isVacuumOn;			/**< Is the vacuum currently on */
	SUCTION suctionStatus;		/**< The current suction status */
} VAC_STATUS;

/**
 * @typedef Pick-Robot Command Structure
 */
typedef struct {
	char command;			/**< The currently passed command */
	int axisCommand[3];		/**< The coordinates to pass to the axes */
} COMMAND_STRUCT;

/**
 * @typedef Robot In
 * @brief Information receive by the pick-robot
 */
typedef struct {
	COMMAND_STRUCT commandStruct;	/**< The command structure */
	JSON_CONFIG config;				/**< The pick-robot configuration */
	long block_number;				/**< The current block number */
} ROBOT_IN;

/**
 * @typedef Robot Out
 * @brief Information reported by the pick-robot
 */
typedef struct {
	RUNTIME_FLAGS runtimeFlags;				/**< Current runtime flags */
	MOTOR_DEBUG mDebug;						/**< Motor debugging */
	OPERATING_ERRORS operatingErrors;		/**< Current operating errors */
	AXIS_STATUS axisStatus;					/**< Current axis status */
	PC_STATUS pc_status; 					/**< Current pick control status */
	VAC_STATUS vacStatus;					/**< Current vacuum control status */
	long block_number;						/**< Current block number */
} ROBOT_OUT;

#endif /* SHAREDMEMORYSTRUCTS_H */
