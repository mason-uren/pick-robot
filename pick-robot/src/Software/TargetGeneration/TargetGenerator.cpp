#include "TargetGenerator.h"

#include <algorithm>
#include <string.h>

inline int sign(int x) {
	return (x > 0) - (x < 0);
}


TargetGenerator::TargetGenerator(TARGET_GENERATOR_CONFIG* tgConfig) {
	xIndex = 0;
	yIndex = 0;
	firstTarget = true;
	this->updateConfig(tgConfig);
	newBoxAdded();
}

void TargetGenerator::updateConfig(TARGET_GENERATOR_CONFIG* tgConfig) {
	this->setDeltas(tgConfig->delta);
	this->setBoxDimensions(tgConfig->boxStart, tgConfig->boxEnd);
	this->setDropLocation(tgConfig->dropLocation);
}

TargetGenerator::~TargetGenerator() {

}

void TargetGenerator::getNextTarget(std::array<axis_pos, NUM_AXES> &targetOut) {
	if (needNewBox) {
		for (int i = 0; i <= Z; i++) {
			targetOut[i] = 0;
		}
		return;
	}

	if (firstTarget) {
		firstTarget = false;
		targetOut = lastPick;
		return;
	}

	do {
		xIndex = std::min(xIndex + 1U, MAX_GRID_SIZE);
		lastPick[X] += delta[X] * deltaDir[X];

		if (sign(boxEnd[X] - lastPick[X]) != deltaDir[X]) {
			xIndex = 0;
			lastPick[X] = boxStart[X];
			yIndex = std::min(yIndex + 1U, MAX_GRID_SIZE);
			lastPick[Y] += delta[Y] * deltaDir[Y];
			if (sign(boxEnd[Y] - lastPick[Y]) != deltaDir[Y]) {
				yIndex = 0;
				lastPick[Y] = boxStart[Y];
				lastPick[Z] += delta[Z] * deltaDir[Z];
				needNewBox = (sign(boxEnd[Z] - lastPick[Z]) != deltaDir[Z]);
			}
		}
		if (!needNewBox) {
			targetOut = lastPick;
		} else {
			xIndex = 0;
			yIndex = 0;
			break;
		}
	} while (!needNewBox && lastPickHeight[xIndex][yIndex] == boxEnd[Z]);
}

void TargetGenerator::newBoxAdded() {
	memset(lastPickHeight, 0, sizeof(axis_pos) * MAX_GRID_SIZE * MAX_GRID_SIZE);
	reset();
	needNewBox = false;
}

void TargetGenerator::getLastTarget(std::array<axis_pos, NUM_AXES> &targetOut) {
	targetOut = lastPick;
}

axis_pos TargetGenerator::getZDepthAboveItem() {
	if (lastPickHeight[xIndex][yIndex] == 0) {
		return std::min(0, this->lastPick[Z] - (deltaDir[Z] * 20));
	} else {
		axis_pos zAboveItem = lastPickHeight[xIndex][yIndex]
				+ (deltaDir[Z] * getSmallestDimensionOfDelta());
		if (deltaDir[Z] > 0) {
			return std::min(boxEnd[Z], zAboveItem);
		} else {
			return std::max(boxEnd[Z], zAboveItem);
		}
	}
}

axis_pos TargetGenerator::getDropLocation(AXIS axis) {
	return dropLocation[axis];
}

axis_pos TargetGenerator::getLargestDimensionOfDelta() {
	return std::max(delta[X], std::max(delta[Y], delta[Z]));
}

axis_pos TargetGenerator::getSmallestDimensionOfDelta() {
	return std::min(delta[X], std::min(delta[Y], delta[Z]));
}

axis_pos TargetGenerator::getZClearancePlane() {
	//1.414 is just a crude approximation of hypotenuse (think, bag grabbed at corner)
	int largestDimension = getLargestDimensionOfDelta() * 1.414;
	//20mm clearance above box + size of item, but make sure it's within travel limits
	int clearancePlane = this->boxStart[Z] + (-deltaDir[Z] * largestDimension) + (-deltaDir[Z] * 20);
	return std::min(clearancePlane, 0);
}

bool TargetGenerator::isLastPickHeightSet() {
	return lastPickHeight[xIndex][yIndex] == 0;
}

axis_pos TargetGenerator::getZProbeDepth() {
	if (lastPickHeight[xIndex][yIndex] == 0) {
		//Set to the bottom, assuming we fail
		lastPickHeight[xIndex][yIndex] = boxEnd[Z];
		return boxEnd[Z];
	} else if (lastPickHeight[xIndex][yIndex] == boxEnd[Z]) {
		return getZClearancePlane() - 1; //Skip this, shouldn't even hit this line of code
	} else {
		axis_pos nextStop = lastPickHeight[xIndex][yIndex]
				+ getLargestDimensionOfDelta() * deltaDir[Z];
		if (deltaDir[Z] == -1) {
			return std::max(nextStop, boxEnd[Z]);
		} else {
			return std::min(nextStop, boxEnd[Z]);
		}
	}
}

axis_pos TargetGenerator::getTopOfBoxZ() {
	return std::max(boxStart[Z], boxEnd[Z]);
}

void TargetGenerator::markPicked(axis_pos zPos) {
	lastPickHeight[abs(xIndex)][abs(yIndex)] = zPos;
}

//This does not follow the normal flow of target generation, be careful with this
void TargetGenerator::setPickTarget(std::array<axis_pos, NUM_AXES> &newPickPos) {
	lastPick = newPickPos;
}
