#ifndef SRC_UTILITIES_SHAREDMEMORY_H_
#define SRC_UTILITIES_SHAREDMEMORY_H_

#include <SharedMemoryStructs.h>

/**
 * @class SharedMemory
 * @brief A module for passing information between Pick-Trigger-App and the Pick-Robot.
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
	 * @brief Instance specific to the Pick-Trigger-App.
	 */
	SharedMemory();
	virtual ~SharedMemory();

	/**
	 * @fn readRobotOut
	 * @brief Read the passed information from the Pick-Robot. Used for reporting
	 * 	status of the system.
	 * @param[in] status Current status of the system.
	 */
	bool readRobotOut(ROBOT_OUT*);

	/**
	 * @fn writeRobotIn
	 * @brief Write input commands to the Pick-Robot.
	 * @param[in] block Current commands or configurations.
	 */
	bool writeRobotIn(ROBOT_IN*);
};

#endif /* SRC_UTILITIES_SHAREDMEMORY_H_ */
