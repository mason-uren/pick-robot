/*
 * Registers.h
 *
 *  Created on: Jul 26, 2018
 *      Author: masonuren
 */

#ifndef SRC_HARDWARE_PININTERACTIONS_REGISTERS_H_
#define SRC_HARDWARE_PININTERACTIONS_REGISTERS_H_

#include <stdint.h>
#include <bcm2835.h>
#include <array>
#include <sys/ioctl.h>

using namespace std;

/**
 * @class Registers
 * @brief Allows bitwise interactions with registers by leveraging the
 * 	bcm2835 library.
 */
class Registers {
public:
	Registers() {}
	virtual ~Registers();

	/**
	 * @static writeByte
	 * @brief Writes 16-bits to the specified destination register associated with a specific I2C address.
	 * @param[in] slaveAddress The desired I2C slave address to communicate.
	 * @param[in] reg A reference to the address of the register to write.
	 * @param[in] value A reference to the array of 16-bits orgainize in two 8-bit packets.
	 */
	static void writeByte(uint8_t slaveAddress, const char &reg, std::array<int, 2> &value) {
		bcm2835_i2c_setSlaveAddress(slaveAddress);
		char data[value.size() + 1];
		data[0] = reg & 0xFF;
		for (unsigned int i = 0; i < value.size(); i++) {
			data[i + 1] = value[i];
		}
		bcm2835_i2c_write(data, sizeof(data));
	}

	/**
	 * @static readByte
	 * @brief Reads 16-bits from a desired register.
	 * @param[in] slaveAddress The desired I2C slave address to communicate.
	 * @param[in] reg A reference to the addresss of the register to read.
	 * @return An array contains the 16-bit read register value
	 * 	split into 8-bits.
	 */
	static std::array<uint8_t, 2> readByte(uint8_t slaveAddress, const char &reg) {
		bcm2835_i2c_setSlaveAddress(slaveAddress);
		char buf[2] = {reg};
		bcm2835_i2c_read_register_rs(buf, buf, 2);
		return std::array<uint8_t, 2> { {buf[0], buf[1]} };
	}
};

#endif /* SRC_HARDWARE_PININTERACTIONS_REGISTERS_H_ */
