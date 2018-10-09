#ifndef _COMPONENT_INTERFACE
#define _COMPONENT_INTERFACE

/**
 * @interface ComponentInterface
 * @brief A template class for each component of the Pick-Robot.
 */
class ComponentInterface
{
public:
	virtual ~ComponentInterface() {}

	/**
	 * @fn step
	 * Move the Pick-Robot along to the next event if one exists.
	 * @param clockTicks The current clock tick in milliseconds.
	 */
	virtual void step(long long int clockTicks) = 0;

	/**
	 * @fn reportStatus
	 * Report pertinent information about the desired component.
	 */
	virtual void reportStatus(void *) = 0;

	/**
	 * @fn emergencyStop
	 * Immediately stop component interactions.
	 */
	virtual void emergencyStop() = 0;
//	virtual void reset() = 0;
};
#endif
