/*
 * I2C.h
 *
 *  Created on: Jul 26, 2018
 *      Author: masonuren
 */

#ifndef SRC_HARDWARE_PININTERACTIONS_I2C_H_
#define SRC_HARDWARE_PININTERACTIONS_I2C_H_

#include <stdint.h>
#include <iostream>
#include "Registers.h"

/**
 * @class I2C
 * @brief Allows %I2C interactions with the hardware through a
 * 	designated %I2C slave address.
 *
 * Register interactions across the Raspberry Pi and SlushBoard are achieved by leveraging
 * the @ref bcm2835 library.
 */
class I2C {
public:
	/**
	 * @param[in] addr The desired I2C bus as a register value.
	 */
	I2C(uint8_t addr);
	virtual ~I2C() {}

	/**
	 * @fn getSlaveAddress
	 * @return Retrieves currently specified I2C slave address.
	 */
	uint8_t getSlaveAddress() {
		return slaveAddress;
	}

	/**
	 * @fn closeI2CBus
	 * Closes the previously designated I2C channel.
	 */
	void closeI2CBus();

private:
	uint8_t slaveAddress; /**< The register value of the desired I2C slave address */
};


#endif /* SRC_HARDWARE_PININTERACTIONS_I2C_H_ */
