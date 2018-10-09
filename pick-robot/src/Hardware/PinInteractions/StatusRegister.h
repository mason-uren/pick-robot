/*
 * StatusRegister.h
 *
 *  Created on: Aug 20, 2018
 *      Author: masonuren
 */

#ifndef SRC_HARDWARE_PININTERACTIONS_STATUSREGISTER_H_
#define SRC_HARDWARE_PININTERACTIONS_STATUSREGISTER_H_

/**
 * @file StatusRegister.h
 */

#include <array>
#include <slushmotor.h>
#include <SharedMemoryStructs.h>
#include "../Motors/MotorInterface.h"

/**
 * @def STATUS_REG_FLAGS
 * @brief The number of possible errors reported by the l6470 chip.
 */
#define STATUS_REG_FLAGS 8

/*
 * When the HiZ flag is high, it indicates that the bridges are in high impedance state.
 * 		Any motion command makes the device exit from High Z state (HardStop and SoftStop included),
 * 		unless error flags forcing a High Z state are active.
 * The UVLO flag is active low and is set by an undervoltage lockout or reset events (power-up included).
 * The TH_WRN, TH_SD, OCD flags are active low and indicate, respectively, thermal warning,
 * 		thermal shutdown and overcurrent detection events.
 * STEP_LOSS_A and STEP_LOSS_B flags are forced low when a stall is detected on bridge A or bridge B respectively.
 * The NOTPERF_CMD and WRONG_CMD flags are active high and indicate, respectively,
 * 		that the command received by SPI cannot be performed or does not exist at all.
 * The SW_F flag reports the SW input status (low for open and high for closed).
 * The SW_EVN flag is active high and indicates a switch turn-on event (SW input falling edge).
 * The UVLO, TH_WRN, TH_SD, OCD, STEP_LOSS_A, STEP_LOSS_B, NOTPERF_CMD, WRONG_CMD and SW_EVN flags are latched:
 * 		when the respective conditions make them active (low or high), they remain in that state until a GetStatus command is sent to the IC.
 * The BUSY bit reflects the BUSY pin status. The BUSY flag is low when a constant speed,
 * 		positioning or motion command is under execution and is released (high) after the command has been completed.
 * The SCK_MOD bit is an active high flag indicating that the device is working in Step-clock mode.
 * 		In this case the step-clock signal should be provided through the STCK input pin. The DIR bit indicates the current motor direction:
 */

/**
 * @class StatusRegister
 * @brief Reports the status of the @ref SlushBoard and each @ref SlushMotor.
 *
 * Provides the ability to monitor the working (and idle) states of the #SlushBoard and individual
 * 	#SlushMotor. The status of each #SlushMotor is reported through a L6470 fully integrated
 * 	microstepping motor driver. Information passed via the status register includes:
 * 		- Motor high impedance status
 * 		- Under-voltage lockout detection (minimum motor voltage not met)
 * 		- Board thermal warning threshold detection
 * 		- Board thermal shutdown threshold detection
 * 		- Power MOSFET over-current detection
 * 		- Motor stall detection due to speed or load angle
 * 		- Is motor busy
 * 		- Stepping mode
 * 		- Non-performable command (register attempted to write it busy)
 * 		- Wrong command (command does not exist)
 */
class StatusRegister {
private:
	DIRECTION direction;											/**< Interpreted direction of travel of the corresponding motor as interpretted by the L6470 library */
	MOTOR_MOTION motorMotion;										/**< %Motor state (ie. #MM_STOPPED | #MM_ACCEL | #MM_DECEL | #MM_CONSTANT_SPEED) */
	std::array<ERROR_LEVEL, STATUS_REG_FLAGS> errorStatus;			/**< An array of #ERROR_LEVEL where the index of the error level indicates the corresponding error. */
	std::array<bool, BOARD_STATUS::BF_NUM_OF_FLAGS> boardStatus;	/**< An array of bool that represent reported board status messages, where the index of the bool indicates the corresponding status message. */
	int numberOfErrors;												/**< The number of flagged error returned from #updateStatus. */

	/**
	 * @fn checkForErrors
	 * Count the number of flagged errors in #errorStatus.
	 */
	void checkForErrors();

public:
	/**
	 * Sets:
	 * 		- #direction : #DIR_FORWARD
	 * 		- #motorMotion : #MM_STOPPED
	 * 		- #numberOfErrors : 0
	 */
	StatusRegister()
		: direction(DIRECTION::DIR_FORWARD),
		  motorMotion(MOTOR_MOTION::MM_STOPPED),
		  numberOfErrors(0) {}
	virtual ~StatusRegister() {}

	/**
	 * @fn updateStatus
	 * @brief Report the number of errors and/or status messages signaled by the l6470 chip.
	 *
	 *
	 * 	Each error is logged by index and marked by the associated severity of error (#ERROR_LEVEL). Board status messages
	 * 	are similarly logged by index but only mark whether that particular message was received. All
	 * 	errors/status messages are cleared and re-populated with every function call.
	 * 	@param[in] motor A reference to the particular motor associated with the l6470 chip.
	 */
	void updateStatus(SlushMotor *motor);

	/**
	 * @fn resetStatusReg
	 * @brief Sets #numberOfErrors to 0 and resets the errors observed by the l6470 chip.
	 *
	 *
	 * Does not record any returned messages.
	 * @param[in] motor A reference to the particular motor associated with the l6470 chip.
	 */
	void resetStatusReg(SlushMotor *motor);

	/**
	 * @fn getNumberOfErrors
	 * @return The number of flagged errors #numberOfErrors.
	 */
	int getNumberOfErrors();

	/**
	 * @fn getDirectionOfTravel
	 * @return The direction the motor is moving.
	 */
	DIRECTION getDirectionOfTravel();

	/**
	 * @fn getMotionOfMotor
	 * @return #MOTOR_MOTION The movement of the motor.
	 */
	MOTOR_MOTION getMotionOfMotor();

	/**
	 * @fn getErrorStatus
	 * @return A reference to #errorStatus.
	 */
	std::array<ERROR_LEVEL, STATUS_REG_FLAGS> *getErrorStatus();

	/**
	 * @fn getBoardStatus
	 * @return A reference to #boardStatus.
	 */
	std::array<bool, BOARD_STATUS::BF_NUM_OF_FLAGS> *getBoardStatus();
};

#endif /* SRC_HARDWARE_PININTERACTIONS_STATUSREGISTER_H_ */
