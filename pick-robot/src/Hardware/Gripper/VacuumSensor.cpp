
#include "VacuumSensor.h"

#include <SharedMemoryStructs.h>
#include <array>
#include <cstdint>
#include <cstdio>

#include "../PinInteractions/Registers.h"

void VacuumSensor::step(long long int clockTicks) {
	if (this->activelyListening) {
		this->filter.filterValue(this->getLastResult());
	}
}

void VacuumSensor::reportStatus(void *robotOutPtr) {
	ROBOT_OUT * rout = (ROBOT_OUT *) robotOutPtr;
	rout->vacStatus.sensorValue = getCurrentSuctionValue();
	rout->vacStatus.suctionStatus = this->determineSuction();
}

bool VacuumSensor::hasSuction() {
	return this->determineSuction() == SUCTION::GOOD_SUCTION;
}

bool VacuumSensor::hasIndeterminateSuction() {
	return this->determineSuction() == SUCTION::INDETERMINATE_SUCTION;
}

SUCTION VacuumSensor::determineSuction() {
	double filterValue = this->getCurrentSuctionValue();
	if (filterValue < this->lowThresh) {
		return SUCTION::GOOD_SUCTION;
	}
	else if (filterValue > this->highThresh) {
		return SUCTION::BAD_SUCTION;
	}
	else {
		return SUCTION::INDETERMINATE_SUCTION;
	}
}

double VacuumSensor::getCurrentSuctionValue() {
	return this->activelyListening ? this->filter.getValue() : 66666;
}

void VacuumSensor::printValues() {
	this->filter.printValues();
}

void VacuumSensor::setHighThresh(int high) {
	this->highThresh = high;
}

void VacuumSensor::setLowThresh(int low) {
	this->lowThresh = low;
}

void VacuumSensor::beginReadingVacSensor() {
	if (!this->activelyListening) {
		this->resetVacSensor();
		this->startReadComparator(this->channel + 0x04,ADS1x15_CONFIG_MODE_CONTINUOUS);
		this->activelyListening = true;
	}
}

/*
 * channels must exist between 0 -> 3 (Inclusive)
 * numReadings must be a value of 1, 2, or 4
 * high/low threshold believe to relate to volts? (unclear)
 */
void VacuumSensor::startReadComparator(int mux, uint8_t mode) {
	if (this->channel < 0 && this->channel > 3) {
		perror("Bad channel value. Should exist between 0 and 3 inclusive.");
		exit(EXIT_FAILURE);
	}
	if (this->ADS_numOfReads != 1 && this->ADS_numOfReads != 2 && this->ADS_numOfReads != 4) {
		perror("Bad numReadings value. Should be 1, 2, or 4");
		exit(EXIT_FAILURE);
	}
	// Build up the appropriate config register value
	int config = ADS1x15_CONFIG_OS_SINGLE;
	config |= (mux & 0x07) << ADS1x15_CONFIG_MUX_OFFSET;

	config |= ads1115ConfigGain[this->gain];
	// Set the mode (continuous or single shot)
	config |= mode;
	config |= ads1115ConfigDataRate[this->dataRate];

	// Set number of comparator hits before alerting
	config |= ads1115ConfigComparator[this->ADS_numOfReads];
	// Send the config value to start the ADC conversion.
	std::array<int, 2> configArray { {(config >> 8) & 0xFF, config & 0xFF} };
	Registers::writeByte(ADS1x15_DEFAULT_ADDRESS, ADS1x15_POINTER_CONFIG, configArray);


	/*
	 * When writing new configuration settings, the currently ongoing conversion
	 * completes with the previous configuration settings. Thereafter,
	 * continuous conversions with the new configuration settings start.
	 *
	 * (ie. the first read value is from the previous config and should be ignored)
	 */
	Registers::readByte(ADS1x15_DEFAULT_ADDRESS, ADS1x15_POINTER_CONVERSION);
}

uint16_t VacuumSensor::getLastResult() {
	std::array<uint8_t, 2> reg = Registers::readByte(ADS1x15_DEFAULT_ADDRESS, ADS1x15_POINTER_CONVERSION);
	return this->convertValues(reg[1], reg[0]);
}

void VacuumSensor::stopReadingVacSensor() {
	int config = ADS1x15_STOP_CONFIG;
	std::array<int, 2> configArray { {(config >> 8) & 0xFF, config & 0xFF} };
	Registers::writeByte(ADS1x15_DEFAULT_ADDRESS, ADS1x15_POINTER_CONFIG, configArray);
	this->resetVacSensor();
}

uint16_t VacuumSensor::convertValues(uint8_t low, uint8_t high) {
	// Convert to 16-bit signed value
	uint16_t value = ((high) << 8) | (low);
	// Turn into positive value
	value &= ADS_CHECK_SIGN_BIT;
	return value;
}

void VacuumSensor::emergencyStop() {
	//Nothing to do, keep reading.
}

void VacuumSensor::resetVacSensor() {
	this->filter.clearBuffer();
	this->activelyListening = false;
}
