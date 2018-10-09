/*
 * VacSensorInterface.h
 *
 *  Created on: Aug 8, 2018
 *      Author: masonuren
 */

#ifndef SRC_HARDWARE_GRIPPER_SENSORINTERFACE_H_
#define SRC_HARDWARE_GRIPPER_SENSORINTERFACE_H_

/**
 * @file VacSensorInterface.h
 */

#include <SharedMemoryStructs.h>

/*
 * @def HIGH_THRESH
 * @brief Determined high threshold of vacuum sensor.
 *
 * If read sensor value exceeds defined value then bad
 * 	suction is reported.
 */
#define HIGH_THRESH 30000
/**
 * @def LOW_THRESH
 * @brief Determined low threshold of vacuum sensors.
 *
 * If read sensor value exceeds defined value then good
 * 	suction is reported.
 */
#define LOW_THRESH 20000

/**
 * @interface VacSensorInterface
 * @brief Provides a generic way to interact with the vacuum sensor.
 *
 * All vacuum sensor, live or simulated, must inherit from this interface
 * 	to be properly integrated with the Pick-Robot software.
 */
class VacSensorInterface : public ComponentInterface {
public:
	VacSensorInterface() {}
	virtual ~VacSensorInterface() {}

	/**
	 * @fn hasSuction
	 * @return Is the suction value below the #LOW_THRESH.
	 */
	virtual bool hasSuction() = 0;

	/**
	 * @fn hasIndeterminateSuction
	 * @return Is the suction value greater than #LOW_THRESH and less than #HIGH_THRESH.
	 */
	virtual bool hasIndeterminateSuction() = 0;

	/**
	 * @fn getCurrentSuctionValue
	 * @return The currently read suction value.
	 */
	virtual double getCurrentSuctionValue() = 0;

	/**
	 * @fn setHighThresh
	 * @brief Set the high threshold of the the vacuum gripper.
	 * @param[in] high The high threshold value.
	 */
	virtual void setHighThresh(int high) = 0;

	/**
	 * @fn setLowThresh
	 * @brief Set the low threshold of the vacuum gripper.
	 * @param[in] low The low threshold value.
	 */
	virtual void setLowThresh(int low) = 0;
};

#endif /* SRC_HARDWARE_GRIPPER_SENSORINTERFACE_H_ */
