#include "PickControl.h"

#include "../ErrorHandler/ErrorHandler.h"
#include "../MotorController/MotorController.h"
#include "../TargetGeneration/TargetGenerator.h"
#include "../ZeroReturn/ZeroReturnController.h"

PickControl::PickControl(SharedMemory* sharedMemoryObj, MotorController* motorControlObj, Gripper* vacObj,
		ZeroReturnController* zcObj, TargetGenerator* targetGenerator) {
	tg = targetGenerator;
	sm = sharedMemoryObj;
	mc = motorControlObj;
	vc = vacObj;
	zc = zcObj;
	vs = vc->getSensor();
	state = PC_NEEDS_ZERO;
	block = {0};
	target = {0};
	itemsPicked = 0;
	nextStateFunction = 0;
}

PickControl::~PickControl() {

}

void PickControl::reportStatus(void *ptr) {
	ROBOT_OUT* status = (ROBOT_OUT *) ptr;
	status->pc_status.state = this->state;
	status->pc_status.itemsPicked = this->itemsPicked;
}

void PickControl::step(long long int clockTicks) {
	ERROR_LEVEL currLevel = ErrorHandler::getInstance()->getErrorLevel();
	if (currLevel == EL_STOP) {
		this->state = PC_READY;
	}
	else if(currLevel >= EL_STOP_AND_ZERO) {
		this->state = PC_NEEDS_ZERO;
	}

	switch (this->state) {
		case PC_NEEDS_ZERO:
			if (zc->isZeroed()) {
				state = PC_READY;
			}
			break;
		case PC_VAC_ON:
			vc->activate();
			state = PC_READY;
			break;
		case PC_VAC_OFF:
			vc->deactivate();
			state = PC_READY;
			break;
		case PC_READY:
		case PC_AT_DROPOFF_XYZ:
			//We do nothing but wait for a command to be received.
			break;
		case PC_PICK_COMMAND_RECEIVED:
			PickControl::findTarget();
			break;
		case PC_ZERO_RETURN:
			zc->start();
			state = PC_ZERO_RETURN_WAIT;
			break;
		case PC_ZERO_RETURN_WAIT:
			if (zc->state == ZR_IDLE) {
				state = PC_READY;
			}
			break;
			// Pick Logic
		case PC_AT_PICK_POSITION_XY:
			PickControl::moveToPickPositionZAboveItem();
			break;
		case PC_AT_PICK_POSITION_XY_ABOVE_Z:
			PickControl::moveToPickPositionXYZ();
			break;
		case PC_PROBING:
			PickControl::probeWithVacuum(clockTicks);
			break;
		case PC_HAS_ITEM:
			PickControl::raiseArm();
			break;
		case PC_AT_PICK_POSITION_Z_CLEARANCE:
			PickControl::moveToDropOffPositionXY();
			break;
		case PC_AT_DROPOFF_XY:
			PickControl::dropOffItem();
			break;
		case PC_ITEM_PLACED:
			itemsPicked++;
			PickControl::moveToDropOffPositionZHome();
			break;
		case PC_MOVE_TO_NEW_DROPOFF:
			PickControl::moveToNewDropOff();
			break;
		case PC_MOVING_TO_DROPOFF_XY:
			PickControl::movingToDropoffXY();
			break;
		case PC_TARGET_FOUND:
		case PC_MOVING_ABOVE_PICK:
		case PC_WAIT_FOR_MOTION:
		case PC_MOVING_TO_DROPOFF_XYZ:
		case PC_RAISING_ARM:
		case PC_AT_Z_CLEARANCE_RETURN:
			PickControl::waitForMotion();
			break;
		case PC_ERROR:
		default:
			//Do nothing
			break;
	}
}

void PickControl::findTarget() {
	tg->getNextTarget(target);
	mc->setTarget(X, target[X]);
	mc->setTarget(Y, target[Y]);
	nextState = PC_AT_PICK_POSITION_XY;
	state = PC_TARGET_FOUND;
}

void PickControl::moveToPickPositionZAboveItem() {
	mc->setTarget(Z, std::min(mc->getPosition(Z), tg->getZDepthAboveItem()));
	vc->activate();
	nextState = PC_AT_PICK_POSITION_XY_ABOVE_Z;
	state = PC_MOVING_ABOVE_PICK;
}

void PickControl::moveToPickPositionXYZ() {
	vc->activate();
	mc->setTarget(Z, tg->getZProbeDepth(), 30); //Move to the item slowly
	state = PC_PROBING;
}

void PickControl::probeWithVacuum(long long int clockTicks) {
	static long long int endTime = 0;
	//if(hasReachedTarget() && vs->hasIndeterminateSuction()) //then we will do an optimized repick
	if (mc->hasReachedTarget() && !vs->hasSuction()) { //&& no suction
			//Let's sleep at the bottom for some time to see if we get suction
		endTime = endTime == 0 ? clockTicks + 0.5 * SEC_TO_MILL : endTime;
		if (clockTicks >= endTime) {
			endTime = 0;
			//We reached our target without getting suction for 0.5 second
			nextState = PC_PICK_COMMAND_RECEIVED; //Go back to finding a target
			state = PC_WAIT_FOR_MOTION;
			vc->deactivate();
			//Move back up
			mc->setTarget(Z, tg->getZClearancePlane());
		}
	} else if (vs->hasSuction()) {
		endTime = 0;
		mc->softStop(Z);
		tg->markPicked(mc->getPosition(Z));
		state = PC_WAIT_FOR_MOTION;
		nextState = PC_HAS_ITEM;
	}
}

void PickControl::raiseArm() {
	mc->setTarget(Z, tg->getZClearancePlane());
	state = PC_WAIT_FOR_MOTION;
	nextState = PC_AT_PICK_POSITION_Z_CLEARANCE;
	nextStateFunction = &PickControl::checkVacuumOnReturn;
}

void PickControl::checkVacuumOnReturn() {
	if (!(vs->hasSuction() || vs->hasIndeterminateSuction())) {
		mc->softStop(X);
		mc->softStop(Y);
		mc->setTarget(X, tg->getLastTarget(X));
		mc->setTarget(Y, tg->getLastTarget(Y));
		state = PC_WAIT_FOR_MOTION;
		nextState = PC_PICK_COMMAND_RECEIVED;
		vc->deactivate();
	}
}

void PickControl::deactivateGripper() {
	vc->deactivate();
}

void PickControl::moveToDropOffPositionXY() {
	state = PC_MOVING_TO_DROPOFF_XY;
	nextState = PC_AT_DROPOFF_XY;
	nextStateFunction = &PickControl::checkVacuumOnReturn;
	mc->setTarget(X, tg->getDropLocation(X));
}

void PickControl::dropOffItem() {
	state = PC_MOVING_TO_DROPOFF_XYZ;
	nextState = PC_AT_DROPOFF_XYZ;
	mc->setTarget(Z, tg->getDropLocation(Z));
}

void PickControl::moveToDropOffPositionZHome() {
	state = PC_AT_Z_CLEARANCE_RETURN;
	nextState = PC_WAIT_FOR_MOTION;
	nextStateFunction = &PickControl::moveToStagingArea;
	mc->setTarget(Z, tg->getZClearancePlane());
}

void PickControl::moveToStagingArea() {
	mc->moveToStaging();
	state = PC_WAIT_FOR_MOTION;
	nextState = PC_READY;
}

void PickControl::movingToDropoffXY() {
	if (mc->hasReachedTarget()) {
		state = nextState;
		nextState = PC_READY;
	}
	else {
		this->checkVacuumOnReturn();
	}
}

void PickControl::waitForMotion() {
	if (mc->hasReachedTarget()) {
		state = nextState;
		nextState = PC_READY;
		if (nextStateFunction != 0) {
			//Call the provided function
			(this->*nextStateFunction)();
			nextStateFunction = 0;
		}
	}
}

void PickControl::moveToNewDropOff() {
	state = PC_WAIT_FOR_MOTION;
	nextState = PC_ITEM_PLACED;
	nextStateFunction = &PickControl::deactivateGripper;
}

void PickControl::emergencyStop() {
	mc->emergencyStop();
	state = PC_NEEDS_ZERO;
}

void PickControl::reset() {
	state = PC_READY;
	nextState = PC_READY;
	nextStateFunction = 0;
}
