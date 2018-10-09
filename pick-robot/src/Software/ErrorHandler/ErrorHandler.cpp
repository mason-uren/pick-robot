/*
 * ErrorHandler.cpp
 *
 *  Created on: Aug 21, 2018
 *      Author: masonuren
 */

#include "ErrorHandler.h"

void ErrorHandler::step(long long int clockTicks) {

}

void ErrorHandler::reportStatus(void* robotOutPtr) {
	ROBOT_OUT *rout = (ROBOT_OUT *) robotOutPtr;
	int numberOfErrors = 0;
	for (unsigned int i = 0; i < this->errorStatus.size(); i++) {
		rout->operatingErrors.errors[i] = this->errorStatus[i];
		if (this->errorStatus[i] > EL_NO_ERROR) {
			numberOfErrors++;
		}
	}
	rout->operatingErrors.priorityError = this->level;
	rout->operatingErrors.numberOfErrors = numberOfErrors;
	rout->runtimeFlags.emergencyStop = this->eStop;
}

void ErrorHandler::emergencyStop() {
	this->eStop = true;
}

void ErrorHandler::addError(ERROR_STATUS error, ERROR_LEVEL errorLevel) {
	ERROR_LEVEL currLevel = this->errorStatus.at(error);
	this->errorStatus.at(error) = this->shouldIgnore ? EL_INFO : std::max(currLevel, errorLevel);
	this->level = std::max(this->level, this->errorStatus.at(error));
}

void ErrorHandler::reset() {
	if (this->level < EL_KILL) {
		std::fill(errorStatus.begin(), errorStatus.end(), EL_NO_ERROR);
		this->eStop = false;
	}
}

ERROR_LEVEL ErrorHandler::getErrorLevel() {
	return this->shouldIgnore ? EL_INFO : this->level;
}

void ErrorHandler::shouldIgnoreErrors(bool isListening) {
	this->shouldIgnore = isListening;
}
