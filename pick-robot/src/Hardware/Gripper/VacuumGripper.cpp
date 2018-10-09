#include "VacuumGripper.h"

#include <slushboard.h>

VacuumGripper::VacuumGripper(SlushBoard *slushboard) {
	board = slushboard;
	board->setIOState(SLUSH_IO_PORTA, SLUSH_IO_PIN0, 0);
	state = VC_OFF;
}

VacuumGripper::~VacuumGripper() {

}

void VacuumGripper::activate() {
	switch (state) {
	case VC_ON:
	case VC_ERROR:
		//Ignore
		break;
	case VC_OFF:
		state = VC_ON;
		vacuumSensor.beginReadingVacSensor();
		board->setIOState(SLUSH_IO_PORTA, SLUSH_IO_PIN0, 1);
	}
}

void VacuumGripper::deactivate() {
	switch (state) {
	case VC_OFF:
	case VC_ERROR:
		//Ignore
		break;
	case VC_ON:
		state = VC_OFF;
		vacuumSensor.stopReadingVacSensor();
		board->setIOState(SLUSH_IO_PORTA, SLUSH_IO_PIN0, 0);
	}
}

void VacuumGripper::step(long long int clockTicks) {
	//Read from sensor
	vacuumSensor.step(clockTicks);

	/*
	 * If VAC_OFF and suction persists after two seconds
	 * reset the suction status.
	 */
	if (this->vacuumSensorError()) {
		ErrorHandler::getInstance()->addError(ES_VACUUM_SENSOR_MISREAD, EL_STOP);
		static long long int endTime = 0;
		endTime = endTime == 0 ? clockTicks + 2 * SEC_TO_MILL : endTime;
		if (clockTicks >= endTime) {
			endTime = 0;
			vacuumSensor.resetVacSensor();
		}
	}
}

void VacuumGripper::reportStatus(void * robotOutPtr) {
	ROBOT_OUT * rout = (ROBOT_OUT *) robotOutPtr;
	rout->vacStatus.isVacuumOn = state == VC_ON;
	vacuumSensor.reportStatus(rout);
}

void VacuumGripper::emergencyStop() {
	deactivate();
}

bool VacuumGripper::vacuumSensorError() {
	return this->getVacuumState() == VC_OFF && vacuumSensor.getCurrentSuctionValue() <= 0;
}
