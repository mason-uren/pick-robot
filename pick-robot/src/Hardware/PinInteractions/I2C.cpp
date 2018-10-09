/*
 * I2C.cpp
 *
 *  Created on: Jul 26, 2018
 *      Author: masonuren
 */

#include "I2C.h"

#include <cstdio>
#include <cstdlib>
#include <stdio.h>

I2C::I2C(uint8_t addr) : slaveAddress(addr)  {
	if (!bcm2835_init()) {
		perror("Failed to initialize bcm2835");
		exit(EXIT_FAILURE);
	}
	if (!bcm2835_i2c_begin()) {
		perror("Failed to initialize I2C. Are you running as root.");
		exit(EXIT_FAILURE);
	}
	bcm2835_i2c_setSlaveAddress(this->slaveAddress);
}

void I2C::closeI2CBus() {
	bcm2835_i2c_end();
	bcm2835_close();
}
