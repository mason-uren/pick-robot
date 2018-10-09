#ifndef SRC_SOFTWARE_TARGETGENERATION_TARGETGENERATOR_H_
#define SRC_SOFTWARE_TARGETGENERATION_TARGETGENERATOR_H_

/**
 * @file TargetGenerator.h
 */

#include <ConfigStruct.h>
#include <array>
#include <cstdlib>
#include <cstring>

#include "../../Hardware/Motors/MotorInterface.h"
#include "../../Utilities/Axis.h"

/**
 * @def MAX_GRID_SIZE
 *
 */
#define MAX_GRID_SIZE 500U

/**
 * @class TargetGenerator
 * @brief Handles creating a series of target locations within in
 * 	a predefined 3D region.
 *
 * Dynamically creates a grid of target locations within the predefined
 * 	box limits. The Pick-Robot generates targets based on the package size
 * 	in a 3D plane such that targets will be processed in a raster pattern, until
 * 	each target has been achieved. When there are no longer targets to process,
 * 	a new box flag is set to indicate that the robot's belief suggests that
 * 	the current RPC is empty. All successful picks at a given (x, y) location, will
 * 	record pick depth. This allows faster picking the next time the (x, y) target
 * 	location is processed, since the Pick-Robot is aware of negative space.
 *
 * 	_Important: %TargetGenerator runs under the guise that a box will be picked
 * 	till empty, before reseting targets. Picked items should not be replaced in the
 * 	RPC since Pick-Robot runs the risks of picking items with too great a speed._
 */
class TargetGenerator {
private:
	/**
	 * A flag to determine if the system has picked through an entire box.
	 */
	bool needNewBox = false;
	/**
	 * Array of values representing the last pick location (x, y, z).
	 */
	std::array<axis_pos, NUM_AXES> lastPick;
	/**
	 * Array of values representing the distance for each pick
	 */
	axis_pos delta[NUM_AXES];
	/**
	 * Array of negative values indicating where the box starts picking from (inclusive), from machine zero
	 * this should INCLUDE any offset from the wall for which we start picking. Values should be larger numerically
	 * than box end values.
	 */
	axis_pos boxStart[NUM_AXES];
	/**
	 * Array of negative values indicating where the box stops picking from (inclusive), from machine zero
	 * this should INCLUDE any offset from the wall for which we stop picking. Values should be smaller numerically
	 * than box start values.
	 */
	axis_pos boxEnd[NUM_AXES];
	/**
	 * Direction of delta travel
	 */
	axis_pos deltaDir[NUM_AXES];

	/**
	 * Axis position we should travel to for dropping items
	 */
	axis_pos dropLocation[NUM_AXES];

	/**
	 * Last pick height for each location
	 */
	axis_pos lastPickHeight[MAX_GRID_SIZE][MAX_GRID_SIZE];

	/**
	 * Index of the x axis.
	 */
	unsigned int xIndex;

	/**
	 * Index of the y axis.
	 */
	unsigned int yIndex;

	/**
	 * The first generated pick location.
	 */
	bool firstTarget;
public:
	/**
	 * @param[in] targetGeneratorConfig Passed items size, box dimensions, and drop location.
	 *
	 * Sets:
	 * 		- #xIndex : 0
	 * 		- #yIndex : 0
	 * 		- #firstTarget : `true`
	 */
	TargetGenerator(TARGET_GENERATOR_CONFIG* targetGeneratorConfig);
	virtual ~TargetGenerator();

	/**
	 * @fn updateConfig
	 * @brief Updates #delta, box dimensions, and #dropLocation.
	 * @param[in] tgConfig A reference to the new box dimensions, delta, and
	 * 	drop location values.
	 */
	void updateConfig(TARGET_GENERATOR_CONFIG* tgConfig);

	/**
	 * @fn setDeltas
	 * @brief Generates the relative distance between each picking target.
	 * @param[in] newDelta Array of integers representing the (x, y, z) dimensions
	 * 	of the item being currently picked.
	 */
	void setDeltas(int newDelta[NUM_AXES]) {
		for (int i = 0; i <= Z; i++) {
			delta[i] = abs(newDelta[i]);
		}
		reset();
	}

	/**
	 * @fn setBoxDimensions
	 * @brief Set the boundary limits of the virtual picking region.
	 *
	 * Box dimensions should always exist as negative values.
	 * @param[in] nexBoxStart One corner of the virtual box represented by an (x, y, z) location.
	 * @param[in] newBoxEnd The opposing corner to #newBoxStart, such that a intersecting line through
	 * 	both points travels from the min to the max of each axis.
	 */
	void setBoxDimensions(int newBoxStart[NUM_AXES], int newBoxEnd[NUM_AXES]) {
		memcpy((void *) boxStart, (void *) newBoxStart, sizeof(int) * NUM_AXES);
		memcpy((void *) boxEnd, (void *) newBoxEnd, sizeof(int) * NUM_AXES);
		for (int i = 0; i <= Z; i++) {
			int x = boxEnd[i] - boxStart[i];
			deltaDir[i] = (x > 0) - (x < 0); //Get the sign of the difference.
		}
		reset();
	}

	/**
	 * @fn setDropLocation
	 * @brief Set the drop location of the Pick-Robot.
	 *
	 * Although location is not explicitly checked to be within axis limits when set,
	 * 	if location remains beyond these limits, axes will never be commanded to this
	 * 	location.
	 * @param[in] newDropLocation The desired drop location (x, y, z).
	 */
	void setDropLocation(int newDropLocation[NUM_AXES]) {
		memcpy((void *) dropLocation, (void *) newDropLocation, sizeof(int) * 3);
	}

	/**
	 * @fn reset
	 * @brief Reset the target generation system.
	 *
	 * Transitions #lastPick to the #boxStart, sets #firstTarget to true,
	 * 	and indicates that a new box is not needed.
	 */
	void reset() {
		for (int i = X; i <= Z; i++) {
			lastPick[i] = boxStart[i];
		}
		firstTarget = true;
		needNewBox = false;
	}

	/**
	 * @fn getNextTarget
	 * @brief Retrieve the next available target.
	 *
	 * If a new box is _not_requried, generate a target that exists within
	 * 	the pre-determined box dimensions, such that previously picked (x, y) target
	 * 	locations exist at a shallower depth.
	 * 	@param[out] targetOut A reference to the next target.
	 */
	void getNextTarget(std::array<axis_pos, NUM_AXES> &targetOut);

	/**
	 * @fn getLastTarget(AXIS axis)
	 * @param[in] axis The desired axis to retrieve target from.
	 * @return The last target for a specific axis.
	 */
	axis_pos getLastTarget(AXIS axis) {
		return lastPick[axis];
	}

	/**
	 * @fn getLastTarget(std::array<axis_pos, NUM_AXES> &targetOut)
	 * @param[out] targetOut The coordinates of the next target location (x, y, z).
	 */
	void getLastTarget(std::array<axis_pos, NUM_AXES> &targetOut);

	/**
	 * @fn getZDepthAboveItem
	 * @return The minimum depth between the last z axis pick distance,
	 * 	for a previously pick (x, y) location, and the bottom of the box.
	 */
	axis_pos getZDepthAboveItem();

	/**
	 * @fn isNeedNewBox
	 * @return Checks whether a new box is needed.
	 */
	bool isNeedNewBox() {
		return needNewBox;
	}

	/**
	 * @fn getLargestDimensionOfDelta
	 * @brief Retrieve the largest dimension of the items being picked.
	 * @return The largest dimension.
	 */
	axis_pos getLargestDimensionOfDelta();

	/**
	 * @fn getSmallestDimensionOfDelta
	 * @brief Retrieve the smallest dimension of the items being picked.
	 * @return The smallest dimension.
	 */
	axis_pos getSmallestDimensionOfDelta();

	/**
	 * @fn getZClearancePlane
	 * @brief Determine the z axis height the arm needs raise, to clear
	 * 	the sides of the box while holding an item.
	 *
	 * Computes the minimum between 0 (maximum height) and the difference
	 * 	of the z axis, plus the box highest and item largest dimension.
	 * @return The minimum distance the arm needs to raise to clear the edges
	 * 	of the box.
	 */
	axis_pos getZClearancePlane();

	/**
	 * @fn getDropLocation
	 * @return The drop location for a specific axis.
	 */
	axis_pos getDropLocation(AXIS axis);

	/**
	 * @fn newBoxAdded
	 * @brief Reset the all previously generated pick locations and restart
	 * 	target generation for a new pick routine.
	 */
	void newBoxAdded();

	/**
	 * @fn markPicked
	 * @brief Mark the z axis location that an item was picked.
	 *
	 * This is used to speed up the picking process.
	 * @param[in] zPos The current position of the z axis.
	 */
	void markPicked(axis_pos zPos);

	/**
	 * @isLastPickHeightSet
	 * @brief Check if the current (x, y) location has z axis depth where
	 * 	an item was previously found.
	 * @return Is there a z axis pick height associated with the current (x, y) location.
	 */
	bool isLastPickHeightSet();

	/**
	 * @fn getZProbeDepth
	 * @brief The minimum depth between the previous pick, at the current (x, y) location, and
	 * 	the bottom of the box.
	 * @return The probing depth.
	 */
	axis_pos getZProbeDepth();

	/**
	 * @fn getTopOfBoxZ
	 * @brief The maximum z axis value between #boxStart and #boxEnd.
	 * @return The z axis height to clear the top of the box.
	 */
	axis_pos getTopOfBoxZ();
	/**
	 * @fn setPickTarget
	 * @brief Generates a target location to pick at based on passed coordinates (x, y, z).
	 *
	 * _IMPORTANT_: This does not follow the normal flow of target generation, be careful with this.
	 * @brief newPickPos The desired new pick target location.
	 */
	void setPickTarget(std::array<axis_pos, NUM_AXES> &newPickPos);
};

#endif /* SRC_SOFTWARE_TARGETGENERATION_TARGETGENERATOR_H_ */
