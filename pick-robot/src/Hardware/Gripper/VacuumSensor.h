/*
 * VacuumSensor.h
 *
 *  Created on: Jul 27, 2018
 *      Author: masonuren
 *
 *      ADS1115 Documentation: http://www.ti.com/lit/ds/symlink/ads1115.pdf
 */

#ifndef SRC_HARDWARE_PININTERACTIONS_VACUUMSENSOR_H_
#define SRC_HARDWARE_PININTERACTIONS_VACUUMSENSOR_H_

/**
 * @file VacuumSensor.h
 */

#include <cstdint>
#include <map>

#include "../../Utilities/ComponentInterface.h"
#include "../../Utilities/FIRFilter.h"
#include "Interfaces/VacSensorInterface.h"


/**
 * @def ADS1x15_CONFIG_MODE_CONTINUOUS
 * @brief Register representation of mode that triggers continuous reading.
 *
 * When ADS1115 chip set in this mode, the reading will be taken from
 * 	the sensor continuously until, at the set #dataRate, until it is explicitly
 * 	instructed to stop. Although values are being produced, they need to be retrieved
 * 	using #getLastResult.
 */
#define ADS1x15_CONFIG_MODE_CONTINUOUS  	0x0000

/**
 * @def ADS1x15_CONFIG_MODE_SINGLE
 * @brief Register representation of mode that triggers a single sensor read.
 *
 * The result of this single read can be retrieved using #getLastResult, but all further
 * 	reads will need to start the comparator again.
 */
#define ADS1x15_CONFIG_MODE_SINGLE      	0x0100
#define ADS1x15_CONFIG_COMP_WINDOW      	0x0010
#define ADS1x15_CONFIG_COMP_ACTIVE_HIGH 	0x0008
#define ADS1x15_CONFIG_COMP_LATCHING    	0x0004
#define ADS1x15_CONFIG_COMP_QUE_DISABLE 	0x0003
#define ADS1x15_STOP_CONFIG					0x8583
#define ADS_CHECK_SIGN_BIT					0x7FFF

enum CONFIG {
	ADS1x15_DEFAULT_ADDRESS        		= 	0x48,
	ADS1x15_POINTER_CONVERSION     		= 	0x00,
	ADS1x15_POINTER_CONFIG        		= 	0x01,
	ADS1x15_POINTER_LOW_THRESHOLD  		= 	0x02,
	ADS1x15_POINTER_HIGH_THRESHOLD 		= 	0x03,
	ADS1x15_CONFIG_OS_SINGLE       		= 	0x8000,
	ADS1x15_CONFIG_MUX_OFFSET      		= 	12
};

#define FILTER_LENGTH 64

using namespace std;

/*
 * Channel	:	0	(DEFAULT) - port connection
 * DataRate	:	860	(DEFAULT) - samples/second
 * Gain		:	1	(DEFAULT) - input/output voltage magnification (2/3 in Ads1115 documentation)
 */

/**
 * @class VacuumSensor
 * @brief A physical implementation of #VacSensorInterface.
 *
 * Vacuum sensor interacts with an Adafruit1115 ADC (Analog Digital Converter) that translates analog signals that
 *  are then interpreted as suction values. Sensor operates by sampling continuously, when
 *  #activelyListening, or reports a arbitrarily high value greater than #highThresh. Sensor
 *  has the capability to change sampling rate, input voltage range, and differentiate between
 *  channels.
 */
class VacuumSensor: public VacSensorInterface {
public:
	/**
	 * @param[in] channel Which register to communicate through.
	 * @param[in] ADS_numReads The number of comparator hits necessary before reporting a sensor value.
	 * The #ads1115ConfigComparator map specific to the ADS1115 analog-to-digital chip.
	 *
	 * Sets:
	 * 		- #channel : \p channel
	 * 		- #dataRate : 860
	 * 		- #gain : 1
	 * 		- #highThresh : #HIGH_THRESH
	 * 		- #lowThresh : #LOW_THRESH
	 * 		- #activelyListening : `false`
	 * 		- #ADS_numOfReads : \p ADS_numReads
	 */
	VacuumSensor(int channel = 0, int ADS_numReads = 1 )
		:	channel(channel),
			dataRate(860),
			gain(1),
			highThresh(HIGH_THRESH),
			lowThresh(LOW_THRESH),
			activelyListening(false),
			ADS_numOfReads(ADS_numReads),
			ads1115ConfigGain { // <-- Key: 0 is used in place of 2/3 (Value of 2/3 does not store neatly as a fraction)
				{0,    	0x0000},
				{1,    	0x0200},
				{2,    	0x0400},
				{4,    	0x0600},
				{8,    	0x0800},
				{16,   	0x0A00}
			},
			// Sampling rates in Samples/Sec
			ads1115ConfigDataRate {
				{8, 	0x0000},
				{16,   	0x0020},
				{32,   	0x0040},
				{64,   	0x0060},
				{128,  	0x0080},
				{250,  	0x00A0},
				{475,  	0x00C0},
				{860,  	0x00E0}
			},
			ads1115ConfigComparator {
				{1, 	0x0000},
				{2, 	0x0001},
				{4,		0x0002}
			} {
	}
	virtual ~VacuumSensor() {}

	/**
	 * @fn step
	 * @brief If #activelyListening report the read value to #FIRFilter.
	 * @param[in] clockTicks The current iteration of #clockTicks
	 */
	void step(long long int clockTicks);

	/**
	 * @fn reportStatus
	 * @brief Report the current suction value and whether the gripper
	 * 	has suction to #ROBOT_OUT.
	 */
	void reportStatus(void *);

	bool hasSuction();
	bool hasIndeterminateSuction();

	/**
	 * @fn getCurrentSuctionValue
	 * @return The current suction value if #activelyListening, else an arbitrarily high value above #highThresh.
	 */
	double getCurrentSuctionValue();
	void setHighThresh(int high);
	void setLowThresh(int low);

	/**
	 * @fn emergencyStop
	 * @brief *** Not implemented ***
	 *
	 * Handled by #VacuumGripper
	 */
	void emergencyStop();

	/**
	 * @fn printValues
	 * @brief Prints the observed sensor values to the console.
	 */
	void printValues();

	/**
	 * @fn stopReadingVacSensor
	 * @brief Stop reading suction values and terminate.
	 *
	 * Halts all interactions with the ADS1115 chip and resets all previously
	 * 	read sensor values by calling #resetVacSensor.
	 */
	void stopReadingVacSensor();

	/**
	 * @fn beginReadingVacSensor
	 * @brief Configure the sensor to read continuous at a specified #dataRate.
	 *
	 * The sensor can also be configured to read in a single shot mode #ADS1x15_CONFIG_MODE_SINGLE,
	 * 	but by default continuous mode is used #ADS1x15_CONFIG_MODE_CONTINUOUS.
	 */
	void beginReadingVacSensor();

	/**
	 * @fn resetVacSensor
	 * brie Reset the #filter values and change #activelyListening to false.
	 */
	void resetVacSensor();

private:
	int channel; 				/**< The communication channel. */
	int dataRate;				/**< The data sampling rate in samples/second. */
	float gain;					/**< A key to #ads1115ConfigGain that determines input voltage range. */
	int highThresh;				/**< The high threshold of read suction values (ie. values above are considered #BAD_SUCTION). */
	int lowThresh;				/**< The low threshold of read suction values (ie. values below are considered #GOOD_SUCTION). */
	bool activelyListening;		/**< Is the vacuum sensor reading values or ignoring them. */

	// ADS1115 Chip
	int ADS_numOfReads;
	std::map<int, uint16_t> ads1115ConfigGain;			/**< Mapping of acceptable input voltage ranges to register values. */
	std::map<int, uint16_t> ads1115ConfigDataRate;		/**< Mapping of acceptable data sampling rates to register values. */
	std::map<int, uint16_t> ads1115ConfigComparator;	/**< Mapping of acceptable comparator hits before triggering a read result to register values. */
	long buffer[FILTER_LENGTH];														/**< Buffer array of length #FILTER_LENGTH, used for capturing sensor readings. */
	FIRFilter<long, FILTER_LENGTH> filter = FIRFilter<long, FILTER_LENGTH>(buffer);	/**< A template class that implements an @ref FIRFilter for averaging read sensor values. */

	/**
	 * @fn startReadComparator
	 * @brief Perform an ADC read.
	 *
	 * Read value is processed with the provided mux, gain, data_rate, and mode
	 * 	values and with the comparator enabled as specified.
	 * @param[in] mux Input multiplexer that supports two differential or four single-ended inputs.
	 * @param[in] mode The sensor reading mode, single-shot or continuous.
	 */
	void startReadComparator(int mux, uint8_t mode);

	/**
	 * @fn getLastResult
	 * @brief Read the last conversion result.
	 * @return A signed integer value representing the currently read suction.
	 */
	uint16_t getLastResult();

	/**
	 * @fn determineSuction
	 * @brief Determine where filtered suction values lie when compared to #highThresh and #lowThresh.
	 * @retval #GOOD_SUCTION If suction values are below #lowThresh.
	 * @retval #INDETERMINATE_SUCTION If suction values exist between #lowThresh and #highThresh.
	 * @retval #BAD_SUCTION If suction values are above #highThresh.
	 */
	SUCTION determineSuction();

	/**
	 * @fn convertValues
	 * @brief Convert to a 16-bit signed value.
	 * @return 16-bit signed value.
	 */
	uint16_t convertValues(uint8_t low, uint8_t high);
};

#endif /* SRC_HARDWARE_PININTERACTIONS_VACUUMSENSOR_H_ */
