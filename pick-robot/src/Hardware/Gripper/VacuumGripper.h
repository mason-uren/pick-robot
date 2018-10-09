#ifndef SRC_HARDWARE_GRIPPER_VACUUMGRIPPER_H_
#define SRC_HARDWARE_GRIPPER_VACUUMGRIPPER_H_

#include "Gripper.h"
#include "VacuumSensor.h"
#include "../../Software/ErrorHandler/ErrorHandler.h"

class SlushBoard;

/**
 * @class VacuumGripper
 * @brief A physical implementation of #GripperInterface.
 *
 * The responsibility of the gripper is to toggle the vacuum between a #VC_ON and #VC_OFF state, while also appropriately handling
 * 	suction readings, during either state. If errors persist gripper component will flag errors
 * 	and call for a stop of all motion and suction.
 */
class VacuumGripper : public Gripper  {
private:
	VacuumSensor vacuumSensor = VacuumSensor();
	SlushBoard *board;

	/**
	 * @fn vacuumSensorError
	 * @brief Determines whether the vacuum sensor is in a error state.
	 *
	 *
	 * 	Verify that if the vacuum is in an #VC_OFF state,
	 * 	that no suction should be observed.
	 * @return The result of checking the suction value against the gripper state.
	 */
	bool vacuumSensorError();
public:
	/**
	 * @param[in] slushboard A pointer to the slushboard object.
	 *
	 * Sets:
	 * 		- #state : #VC_OFF
	 */
	VacuumGripper(SlushBoard *slushboard);
	virtual ~VacuumGripper();

	/**
	 * @fn step
	 * @brief Grabs the current suction value, then determines if vacuum is in an error state.
	 *
	 *
	 * If the #Gripper is repeatedly in an error state for more than 2 seconds, #ES_VACUUM_SENSOR_MISREAD
	 *  is reported to ErrorHandler::addError.
	 * @param[in] clockTicks The current clock tick is milliseconds.
	 */
	void step(long long int clockTicks);

	/**
	 * @fn reportStatus
	 * @brief Check if the vacuum is on and report to #ROBOT_OUT.
	 */
	void reportStatus(void * rout);

	void activate();
	void deactivate();
	VacuumSensor * getSensor() {
		return &vacuumSensor;
	}
	void emergencyStop();
};

#endif /* SRC_HARDWARE_GRIPPER_VACUUMGRIPPER_H_ */
