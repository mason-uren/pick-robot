#ifndef CONFIGPARSER_H_
#define CONFIGPARSER_H_

#include <SharedMemoryStructs.h>
#include <string>
#include <json.hpp>

// for convenience
using json = nlohmann::json;

/**
 * @class ConfigParser
 * @brief Parses JSON configuration file.
 *
 * Handles parsing the JSON configuration file, only if the
 * 	file is passed the necessary arguements.
 */
class ConfigParser {
public:
	ConfigParser();
	virtual ~ConfigParser();

	/**
	 * @fn parseConfig
	 * @brief Parse the JSON configuration and pass to the Pick-Robot.
	 *
	 * Handles the checking and assigning values passed from the configuration
	 * 	to the Pick-Robot. If values are invalid or improperly formatted then
	 * 	configuration is abandoned, otherwise the set config is assigned to
	 * 	#ROBOT_IN, which feeds the data to the Pick-Robot.
	 * @param[in] rin Reference to the %ROBOT_IN structure.
	 * @param[in] data JSON file.
	 */
	void parseConfig(ROBOT_IN* rin, json *data);

	/**
	 * @loadJSONFromFile
	 * @brief If file path is valid, read from file.
	 * @param[in] filePath Patht to the desired JSON file.
	 * @param[out] json JSON object.
	 */
	bool loadJSONFromFile(std::string filePath, json *json);

	/**
	 * @loadJSONFromString
	 * @brief Loads passed JSON string to JSON object.
	 * @param[in] jsonData The passed string value.
	 * @param[out] json JSON object.
	 */
	bool loadJSONFromString(std::string jsonData, json *json);
};

#endif /* CONFIGPARSER_H_ */
