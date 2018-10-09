#include "ConfigParser.h"

#include <json.hpp>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <string>

#include <ConfigStruct.h>
#include <SharedMemoryStructs.h>


// for convenience
using json = nlohmann::json;

ConfigParser::ConfigParser() {
}

ConfigParser::~ConfigParser() {
}

bool ConfigParser::loadJSONFromFile(std::string filePath, json* jsonPtr) {
	std::ifstream configFile;
	configFile.open(filePath);
	if (configFile.is_open()) {
		try {
			configFile >> *jsonPtr;
			return true;
		} catch (nlohmann::detail::parse_error& e) {
			printf("Parse error: %s\n", e.what());
		}
	}
	return false;
}

bool ConfigParser::loadJSONFromString(std::string jsonData, json* jsonPtr) {
	try {
	*jsonPtr = json::parse(jsonData);
		return true;
	} catch (nlohmann::detail::parse_error& e) {
		printf("Parse error: %s\n", e.what());
	}
	return false;
}

void ConfigParser::parseConfig(ROBOT_IN *rin, json *dataPtr) {
	std::hash<nlohmann::json> hasher;

	json data = *dataPtr;

	JSON_CONFIG *config = &rin->config;
	TARGET_GENERATOR_CONFIG *targetConfig;
	AXIS_CONFIG *axisConfig;
	MOTOR_CONFIG *motorConfig;

	config->hash = hasher(data);

	int numAxes = 3;
	int maxMotors = 2;

	/*
	 * Check runtime status flags
	 * DEFAULT VALUES:
	 * Realtime -> true
	 * Simulate -> false
	 * LogAxesData -> false
	 * EmergencyStop -> false
	 */
	if (!data["runtimeFlags"]["realtime"].is_null()) {
		config->runtimeFlags.realtime = data["runtimeFlags"]["realtime"].get<bool>();
	}
	else {
		config->runtimeFlags.realtime = true;
	}
	if (!data["runtimeFlags"]["simulate"].is_null()) {
		config->runtimeFlags.simulate = data["runtimeFlags"]["simulate"].get<bool>();
	}
	if (!data["runtimeFlags"]["logAxesData"].is_null()) {
		config->runtimeFlags.logAxesData = data["runtimeFlags"]["logAxesData"].get<bool>();
	}
	if (!data["runtimeFlags"]["ignoreErrorFlags"].is_null()) {
		config->runtimeFlags.ignoreErrorFlags = data["runtimeFlags"]["ignoreErrorFlags"].get<bool>();
	}
	config->runtimeFlags.emergencyStop = false;

	try {
		for (int axisIndex = 0; axisIndex < numAxes; axisIndex++) {
			targetConfig = &config->targetGeneratorConfig;
			targetConfig->boxEnd[axisIndex] = data["targetGenerator"]["boxEnd"][axisIndex].get<int>();
			targetConfig->boxStart[axisIndex] =
					data["targetGenerator"]["boxStart"][axisIndex].get<int>();
			targetConfig->delta[axisIndex] = data["targetGenerator"]["delta"][axisIndex].get<int>();
			targetConfig->dropLocation[axisIndex] = data["targetGenerator"]["dropLocation"][axisIndex].get<int>();

			axisConfig = &(config->axes[axisIndex]);
			if (!data["axes"][axisIndex].is_null()) {
				axisConfig->valid = true;
				axisConfig->axisLabel = data["axes"][axisIndex]["label"].get<std::string>().c_str()[0];
				axisConfig->stagingArea = data["axes"][axisIndex]["stagingArea"].get<int>();
				axisConfig->travelLimitmm = data["axes"][axisIndex]["travelLimitmm"].get<int>();
				for (int motorIndex = 0; motorIndex < maxMotors; motorIndex++) {
					motorConfig = &(axisConfig->motor[motorIndex]);
					if (!data["axes"][axisIndex]["motors"][motorIndex].is_null()) {
						motorConfig->valid = true;
						motorConfig->motorNumber =
								data["axes"][axisIndex]["motors"][motorIndex]["motorNumber"].get<int>();
						motorConfig->accelCurrent =
							data["axes"][axisIndex]["motors"][motorIndex]["accelCurrent"].get<
								int>();
						motorConfig->decelCurrent = data["axes"][axisIndex]["motors"][motorIndex]["decelCurrent"].get<
								int>();
						motorConfig->accelCurrent = data["axes"][axisIndex]["motors"][motorIndex]["accelCurrent"].get<
								int>();
						motorConfig->holdCurrent =
								data["axes"][axisIndex]["motors"][motorIndex]["holdCurrent"].get<
								int>();
						motorConfig->runCurrent =
								data["axes"][axisIndex]["motors"][motorIndex]["runCurrent"].get<
								int>();
						motorConfig->maxStepsPerSec =
								data["axes"][axisIndex]["motors"][motorIndex]["maxStepsPerSec"].get<
								int>();
						motorConfig->stepsPerRev =
								data["axes"][axisIndex]["motors"][motorIndex]["stepsPerRev"].get<
								int>();
						motorConfig->invert = data["axes"][axisIndex]["motors"][motorIndex]["invert"].get<bool>();
						motorConfig->mmPerRev = data["axes"][axisIndex]["motors"][motorIndex]["mmPerRev"].get<double>();
					} else {
						motorConfig->valid = false;
					}
				}
			} else {
				axisConfig->valid = false;
			}
		}
	} catch (nlohmann::detail::type_error& e) {
		printf("Type error: %s\n", e.what());
	}
}
