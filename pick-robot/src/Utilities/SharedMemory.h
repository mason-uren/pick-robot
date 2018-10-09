/*
 * SharedMemory.h
 *
 *  Created on: Jul 16, 2018
 *      Author: michaelkaiser
 */

#ifndef SRC_UTILITIES_SHAREDMEMORY_H_
#define SRC_UTILITIES_SHAREDMEMORY_H_

#include <SharedMemoryStructs.h>

/**
 * @class SharedMemory
 * @brief A module for passing information between the Pick-Robot and the Pick-Trigger-App.
 */
class SharedMemory {
private:
	/** Memory descriptor for received information to the Pick-Robot */
	int robot_in_md;
	/** Memory descriptor for passed information to the Pick-Trigger-App */
	int robot_out_md;
	/** The size of the file being passed between targets */
	long pg_size;
	/** Memory address of robot in */
	void* robot_in_addr;
	/** Memory address of robot out */
	void* robot_out_addr;
public:
	/**
	 * @brief Instance specific to the Pick-Robot.
	 */
	SharedMemory();
	virtual ~SharedMemory();

	/**
	 * @fn readRobotIn
	 * @brief Read the passed information from either the config or
	 * 	Pick-Trigger-App.
	 * @param[in] ROBOT_IN Reference to #ROBOT_IN structure.
	 */
	bool readRobotIn(ROBOT_IN*);

	/**
	 * @fn writeRobotOut
	 * @brief Write current Pick-Robot status information to the Pick-Trigger-App
	 * @param[in] ROBOT_OUT Reference to #ROBOT_OUT structure.
	 */
	bool writeRobotOut(ROBOT_OUT*);
};


#endif /* SRC_UTILITIES_SHAREDMEMORY_H_ */
