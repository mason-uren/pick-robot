
#ifndef SRC_SOFTWARE_FILTERS_VACUUMSENSORFILTER_H_
#define SRC_SOFTWARE_FILTERS_VACUUMSENSORFILTER_H_

#include <stddef.h>
#include <cstdio>
#include <string.h>
#include <cmath>

/**
 * @class FIRFilter
 * @brief First impulse response filter used to eliminate noise in #VacuumSensor readings.
 *
 * Receives #VacuumSensor readings into fix buffer-array of size 64 integers. Sensor
 * 	readings are averaged based on the number of values currently in the buffer, in
 * 	an attempt to eliminate erroneous reported values. %FIRFilter is a template class
 * 	that accepts a primitive data type, buffer length, and a reference to the passed buffer.
 */
template<typename T, size_t N>
class FIRFilter {
public:
	/**
	 * @param[in] buf A reference to the passed buffer.
	 * @param[in] N The size of the passed buffer.
	 *
	 * Sets:
	 * 		- #head : 0
	 * 		- #bufAtCapacity : `false`
	 * 		- #accumTotal : 0
	 * 		- #buf : @p buf
	 * 		- #bufMaxCapacity : @p N
	 */
	FIRFilter(T (&buf)[N])
		: 	head(0),
			bufAtCapacity(false),
			accumTotal(0),
			buf(buf),
			bufMaxCapacity(N) {
		memset(buf, 0, sizeof(T) * bufMaxCapacity);
	}
	virtual ~FIRFilter() {}

	/**
	 * @fn filterValue
	 * @brief Filter the received sensor readings.
	 * @param[in] sensorInput Read value from #VacuumSensor
	 * @return Filtered value, based on the number of values currently in the buffer.
	 */
	double filterValue(T sensorInput) {
		if (!bufAtCapacity) {
			double returnValue;
			accumTotal += sensorInput;
			buf[head] = sensorInput;
			head++;
			returnValue = accumTotal / head;
			if (head == bufMaxCapacity) {
				bufAtCapacity = true;
				head = 0;
			}
			return returnValue;
		}
		else {
			accumTotal -= buf[head];
			accumTotal += sensorInput;
			buf[head++] = sensorInput;
			head = head % bufMaxCapacity;
			return accumTotal / bufMaxCapacity;
		}
	}

	/**
	 * @fn getValue
	 * @brief Returns the current filtered value.
	 * @return Current suction value.
	 */
	double getValue() {
		if (!bufAtCapacity && head == 0) {
			return nan("");
		}
		return bufAtCapacity ? (accumTotal / bufMaxCapacity) : (accumTotal / head);
	}

	/**
	 * @fn printValues
	 * @brief Print the currently loaded buffer values.
	 *
	 * _Note_: Should only be used for testing purposes, when in non-realtime mode.
	 */
	void printValues() {
		printf("Suction History: ");
		for (int i = 0; i < bufMaxCapacity; i++) {
			printf("%ld ", buf[i]);
		}
		printf("\n");
	}

	/**
	 * @fn clearBuffer
	 * @brief Reset the values in the buffer to 0, but do not change the buffers capacity.
	 */
	void clearBuffer() {
		head = 0;
		bufAtCapacity = false;
		accumTotal = 0;
		memset(buf, 0, sizeof(T) * bufMaxCapacity);
	}

private:
	/** Index of next position to place sensor value */
	int head;
	/** Flag to determine whether buffer is at capacity */
	bool bufAtCapacity;
	/** The current cumulative total of the read sensor values */
	double accumTotal;
	/** Reference to the buffer */
	T *buf;
	/** Max capacity of the buffer */
	int bufMaxCapacity;
};

#endif /* SRC_SOFTWARE_FILTERS_VACUUMSENSORFILTER_H_ */

