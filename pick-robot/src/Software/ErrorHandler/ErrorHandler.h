/*
 * ErrorHandler.h
 *
 *  Created on: Aug 21, 2018
 *      Author: masonuren
 */

#ifndef SRC_SOFTWARE_ERRORHANDLER_ERRORHANDLER_H_
#define SRC_SOFTWARE_ERRORHANDLER_ERRORHANDLER_H_

#include <SharedMemoryStructs.h>
#include <array>

#include "../../Utilities/ComponentInterface.h"

class MotorController;

/**
 * @class ErrorHandler
 * @brief An interface for handling all reported errors.
 *
 * A singleton class that handles all reported errors either by the #SlushBoard
 * 	or operational errors that occur during the pick routine. Errors are logged by
 * 	severity, where the highest severity error will be treated as priority and
 * 	be acted upon first. If the #shouldIgnore flag is set, observed errors are still
 * 	recorded, but they are no longer acted upon, with the exception of #ES_AXIS_TARGET_OUT_OF_BOUNDS
 * 	(this is a paramount error that the Pick-Robot cannot afford to be ignored).
 */
class ErrorHandler : public ComponentInterface {
public:
	/**
	 * @static
	 * @brief An instance of error handler.
	 */
	static ErrorHandler * getInstance() {
		static ErrorHandler instance;
		return &instance;
	}

	/**
	 * @fn step
	 * @brief ***Not Implemented***
	 * @param clockTicks The current clock tick in milliseconds.
	 */
	void step(long long int clockTicks);

	/**
	 * @fn reportStatus
	 * @brief Reports to #ROBOT_OUT error status.
	 *
	 * Determines whether any errors have been observed, what
	 * 	errors have been observed, the number of errors, which error has the
	 * 	highest priority, and whether an emergency stop command has been issued.
	 * @param robotOutPtr A reference to #ROBOT_OUT.
	 */
	void reportStatus(void* robotOutPtr);

	/**
	 * @fn emergencyStop
	 * @brief Sets #eStop to `true`.
	 */
	void emergencyStop();

	/**
	 * @fn addError
	 * @brief Add an error to #errorStatus.
	 *
	 * Adds the current #ERROR_LEVEL to #errorStatus at the index of the reported error,
	 * 	if Pick-Robot is not ignoring errors. If #shouldIgnore is true, then any observed
	 * 	errors are still recorded, but only as informational (#EL_INFO).
	 */
	void addError(ERROR_STATUS error, ERROR_LEVEL errorLevel);

	/**
	 * @reset
	 * @brief Reset all errors.
	 *
	 * Reset #errorStatus to #EL_NO_ERROR, unless the priority error #level is #EL_KILL.
	 * 	Also sets #eStop to false.
	 */
	void reset();

	/**
	 * @fn getErrorLevel
	 * @return The current priority error #level.
	 */
	ERROR_LEVEL getErrorLevel();

	/**
	 * @fn shouldIgnoreErrors
	 * @brief Sets the #shouldIgnore flag.
	 * @param[in] shouldIgnore The passed boolean value.
	 */
	void shouldIgnoreErrors(bool shouldIgnore);

private:
	/**
	 * Sets:
	 * 		- #errorStatus : {#EL_NO_ERROR}
	 * 		- #level : #EL_NO_ERROR
	 * 		- #shouldIgnore : `false`
	 * 		- #eStop : `false`
	 */
	ErrorHandler()
		: errorStatus{},
		  level(EL_NO_ERROR),
		  shouldIgnore(false),
		  eStop(false) {}
	ErrorHandler(ErrorHandler const&);
	void operator=(ErrorHandler const&);

	std::array<ERROR_LEVEL, ERROR_STATUS::ES_NUM_OF_FLAGS> errorStatus;		/**< A list of all possible errors, indexed by #ERROR_STATUS and populated with their respective #ERROR_LEVEL */
	ERROR_LEVEL level;														/**< The priority error level. */
	bool shouldIgnore;														/**< Should the pick-robot only report errors as informational. */
	bool eStop;																/**< Has an emergency stop been issued. */
};

#endif /* SRC_SOFTWARE_ERRORHANDLER_ERRORHANDLER_H_ */
