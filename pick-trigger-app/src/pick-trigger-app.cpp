#include <asm-generic/socket.h>
#include <ConfigStruct.h>
#include <errno.h>
#include <json.hpp>
#include <netinet/in.h>
#include <pthread.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <SharedMemoryStructs.h>
#include <unistd.h>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <fstream>
#include <sstream>
#include <string>
#include <iostream>
#include <arpa/inet.h>
#include "ConfigParser.h"
#include "SharedMemory.h"

#define PORT 6000
#define USEC_PER_SEC		1000000L
#define NSEC_PER_SEC		1000000000L
#define NANO_INC            1000000L
static inline void tsnorm(struct timespec *ts) {
	while (ts->tv_nsec >= NSEC_PER_SEC) {
		ts->tv_nsec -= NSEC_PER_SEC;
		ts->tv_sec++;
	}
}

using namespace std;

using json = nlohmann::json;


json *robotStatusToJSON(json *jsonObj, ROBOT_OUT robotout);
void displayErrors(ROBOT_OUT rout);
void writeToFile(std::ostream &file, std::string values);
std::string getSuctionString(SUCTION suck);
std::string getPickStatusString(PICK_STATE status);
std::string getErrorFlag(ERROR_STATUS status);
std::string getErrorLevel(ERROR_LEVEL level);
std::string getErrorLevelInfo(ERROR_LEVEL level);
std::string getErrorInfo(ERROR_STATUS status);
bool compareCommands(char * str1, const char *str2);
void *connectionListener(void*);
void parseStringForCommand(char *buffer, int length);
bool nextInt(char** buffer);
void sendDefaultConfig();
void printCommandInformation();
volatile bool sendCommand = false;
volatile COMMAND command = COMMAND_PICK_ITEM;
int target[3];
int invalidTarget[] = { 1, 1, 1 };
ConfigParser configParser;
ROBOT_OUT robotout = { 0 };
ROBOT_IN robotin = { 0 };
SharedMemory *sm;
ERROR_LEVEL lastErrorLevel = EL_NO_ERROR;
json robotStatus;

int main() {
	printCommandInformation();
	sm = new SharedMemory();
	sendDefaultConfig();
	int rc;
	//Create thread
	pthread_t thread;
	rc = pthread_create(&thread, NULL, connectionListener, NULL);
	if (rc) {
		perror("Thread failed to be created");
		exit(1);
	}
	struct timespec timespec;
	clock_gettime(CLOCK_MONOTONIC, &timespec);
	timespec.tv_nsec += NANO_INC;
	tsnorm(&timespec);
	ROBOT_OUT oldStatus;
	oldStatus.block_number = -1;

	std::string filename = "Data/positionData.txt";
	std::ofstream file;
	std::ostringstream loggedValues;
	bool shouldWrite = false;

	while (true) {
		static int clockReturn = 0;
		clockReturn = clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &timespec, NULL);
		if (clockReturn != 0) {
			//if (clockReturn != EINTR)
			printf("clock_nanosleep failed. errno: %d clockReturn: %d\n", errno, clockReturn);
			break;
		}
		timespec.tv_nsec += NANO_INC;
		tsnorm(&timespec);

		bool newData = sm->readRobotOut(&robotout);
		if (newData) {

			if (oldStatus.block_number > robotout.block_number) {
				//Resend default config, robot must have been restarted
				sendDefaultConfig();
			}

			if (robotout.runtimeFlags.logAxesData && !file.is_open()) {
				file.open(filename, std::ofstream::out | std::ofstream::app);
				loggedValues << "******** NEW SESSION ********\n";
				robotout.runtimeFlags.realtime ? loggedValues << "REALTIME\n" : loggedValues << "SIMULATED\n";
				writeToFile(file, loggedValues.str());
				loggedValues.str(std::string());
			}

			// Check error states
			displayErrors(robotout);
			lastErrorLevel = robotout.operatingErrors.priorityError;

			oldStatus.block_number = robotout.block_number;
			if (robotout.pc_status.itemsPicked != oldStatus.pc_status.itemsPicked
					|| robotout.pc_status.state != oldStatus.pc_status.state) {
				if (robotout.runtimeFlags.logAxesData) {
					loggedValues << robotout.block_number << " Number of items picked: " <<
						robotout.pc_status.itemsPicked << " Status state: " << getPickStatusString(robotout.pc_status.state) << "\n";
					shouldWrite = true;
				}
				printf("(%ld) Number of items picked: %ld Status state: %s\n", robotout.block_number,
						robotout.pc_status.itemsPicked, getPickStatusString(robotout.pc_status.state).c_str());

				if (robotout.runtimeFlags.logAxesData) {
					loggedValues << robotout.axisStatus.axisPosition[0] << ", " <<
							robotout.axisStatus.axisPosition[1] << ", " <<
							robotout.axisStatus.axisPosition[2] << "\n";
				}
				if (!robotout.axisStatus.isBusy) {//Comment out this line to print live feed of position data. Otherwise prints endpoints
					loggedValues << "Currently " << (robotout.axisStatus.isBusy ? "Moving" : "Idle") << "\n";
					shouldWrite = true;

					printf("Currently %s X: %d Y: %d Z: %d\n", robotout.axisStatus.isBusy ? "Moving" : "Idle",
							robotout.axisStatus.axisPosition[0], robotout.axisStatus.axisPosition[1],
							robotout.axisStatus.axisPosition[2]);
				}
			}

			if (robotout.vacStatus.suctionStatus != oldStatus.vacStatus.suctionStatus
					|| abs(robotout.vacStatus.sensorValue - oldStatus.vacStatus.sensorValue) > 5) {
				if (robotout.runtimeFlags.logAxesData) {
					loggedValues << "Suction on: " << (robotout.vacStatus.isVacuumOn ? "true" : "false") << "\n";
					loggedValues << "Suction: " << robotout.vacStatus.sensorValue << " = " << getSuctionString(robotout.vacStatus.suctionStatus) << "\n";
					shouldWrite = true;
				}
				printf("Suction on: %s\n", robotout.vacStatus.isVacuumOn ? "true" : "false");
				printf("Suction: %f = %s\n", robotout.vacStatus.sensorValue,
						getSuctionString(robotout.vacStatus.suctionStatus).c_str());
			}
			oldStatus = robotout;
		}
		if (sendCommand) {
			robotin.block_number++;
			robotin.commandStruct.command = command;
			memcpy(robotin.commandStruct.axisCommand, target, sizeof(int) * 3);
			sm->writeRobotIn(&robotin);
			printf("Sent command to robot\n");
			sendCommand = false;
		}

		if (shouldWrite) {
			writeToFile(file, loggedValues.str()) ;
			loggedValues.str(std::string());
			shouldWrite = false;
		}
	}
	return 0;
}

json *robotStatusToJSON(json *jsonObj, ROBOT_OUT robotout) {
	*jsonObj = {
			{ "runtimeFlags", {
					{ "realtime", robotout.runtimeFlags.realtime },
					{ "simulated", robotout.runtimeFlags.simulate },
					{ "ignoreErrorFlags", robotout.runtimeFlags.ignoreErrorFlags }
			}},
			{ "pickControlStatus", {
					{ "pickState", getPickStatusString(robotout.pc_status.state).c_str() },
					{ "itemsPicked", robotout.pc_status.itemsPicked },
					{ "isZeroed", robotout.pc_status.state != PC_NEEDS_ZERO }
			}},
			{ "axisStatus", {
					{ "currentPostion", {
							{ "X", robotout.axisStatus.axisPosition[X] },
							{ "Y", robotout.axisStatus.axisPosition[Y] },
							{ "Z", robotout.axisStatus.axisPosition[Z] }
					}},
					{ "targetPostion", {
							{ "X", robotout.axisStatus.targetPosition[X] },
							{ "Y", robotout.axisStatus.targetPosition[Y] },
							{ "Z", robotout.axisStatus.targetPosition[Z] }
					}}
			}},
			{ "vacuumStatus", {
					{ "suctionOn", robotout.vacStatus.isVacuumOn },
					{ "suctionStatus", getSuctionString(robotout.vacStatus.suctionStatus).c_str() },
					{ "sensorValue", robotout.vacStatus.sensorValue }
			}},
			{ "inErrorState", robotout.operatingErrors.numberOfErrors > 0 },
			{ "emergencyStop", robotout.runtimeFlags.emergencyStop }
	};

	if (robotout.operatingErrors.numberOfErrors) {
		json errors;
			for (unsigned int error = 0; error < ES_NUM_OF_FLAGS; error++) {
				ERROR_LEVEL errorLevel = robotout.operatingErrors.errors[error];
				if (errorLevel > EL_NO_ERROR) {
					errors += {
						{ getErrorFlag(static_cast<ERROR_STATUS>(error)).c_str(), {
							{ "description", getErrorInfo(static_cast<ERROR_STATUS>(error)).c_str() },
							{ "severity", getErrorLevel(static_cast<ERROR_LEVEL>(errorLevel)).c_str() }
						}}
					};
				}
			}
		errors += { "numberOfErrors" ,robotout.operatingErrors.numberOfErrors };
		jsonObj->push_back(json::object_t::value_type("operatingErrors", errors));
	}
	return jsonObj;
}

void printCommandInformation() {
	printf("Command Help:\n");
	printf("estop:\t\tImmediately stops machine motion and requires a zero return to resume picking.\n");
	printf("pick:\t\tBegins the pick sequence, if we have been zeroed, and we're waiting in the ready state.\n");
	printf(
			"drop:\t\tTurns off the gripper if we're waiting in the drop location after a pick. Returns back to the staging area for the next pick after dropping the item.\n");
	printf(
			"newbox:\t\tResets the target generation to the top of the box. This is necessary after all pick locations have been attempted.\n");
	printf(
			"status:\t\tReports the current state of the machine and number of items picked.\n");
	printf("zero:\t\tZero returns the machine.\n");
	printf("zneeded:\tZero returns the machine only if it is needed.\n");
}

std::string getSuctionString(SUCTION suck) {
	switch (suck) {
		case BAD_SUCTION:
			return "BAD SUCTION";
		case GOOD_SUCTION:
			return "GOOD SUCTION";
		case INDETERMINATE_SUCTION:
			return "INDETERMINATE SUCTION";
		default:
			return "undefined suction";
	}
}

void sendDefaultConfig() {
	json fileConfig;
	if (!configParser.loadJSONFromFile("/home/pi/default_config.json", &fileConfig)) {
		printf("/home/pi/default_config.json is missing.\n");
		exit(1);
	}
	configParser.parseConfig(&robotin, &fileConfig);
	robotin.block_number++;
	sm->writeRobotIn(&robotin);
	printf("Configuration sent.\n");
}

std::string getPickStatusString(PICK_STATE status) {
	switch (status) {
		case PC_VAC_ON:
			return "PC_VAC_ON";
		case PC_VAC_OFF:
			return "PC_VAC_OFF";
		case PC_ERROR:
			return "PC_ERROR";
		case PC_READY:
			return "PC_READY";
		case PC_PICK_COMMAND_RECEIVED:
			return "PC_PICK_COMMAND_RECEIVED";
		case PC_TARGET_FOUND:
			return "PC_TARGET_FOUND";
		case PC_AT_PICK_POSITION_XY:
			return "PC_AT_PICK_POSITION_XY";
		case PC_HAS_ITEM:
			return "PC_HAS_ITEM";
		case PC_AT_PICK_POSITION_XY_ABOVE_Z:
			return "PC_AT_PICK_POSITION_XY_ABOVE_Z";
		case PC_PROBING:
			return "PC_PROBING";
		case PC_RAISING_ARM:
			return "PC_RAISING_ARM";
		case PC_AT_PICK_POSITION_Z_CLEARANCE:
			return "PC_AT_PICK_POSITION_Z_CLEARANCE";
		case PC_MOVING_TO_DROPOFF_XY:
			return "PC_MOVING_TO_DROPOFF_XY";
		case PC_AT_DROPOFF_XY:
			return "PC_AT_DROPOFF_XY";
		case PC_MOVING_TO_DROPOFF_XYZ:
			return "PC_MOVING_TO_DROPOFF_XYZ";
		case PC_AT_Z_CLEARANCE_RETURN:
			return "PC_AT_Z_CLEARANCE_RETURN";
		case PC_ITEM_PLACED:
			return "PC_ITEM_PLACED";
		case PC_WAIT_FOR_MOTION:
			return "PC_WAIT_FOR_MOTION";
		case PC_ZERO_RETURN:
			return "PC_ZERO_RETURN";
		case PC_ZERO_RETURN_WAIT:
			return "PC_ZERO_RETURN_WAIT";
		case PC_MOVING_ABOVE_PICK:
			return "PC_MOVING_ABOVE_PICK";
		case PC_AT_DROPOFF_XYZ:
			return "PC_AT_DROPOFF_XYZ";
		case PC_NEEDS_ZERO:
			return "PC_NEEDS_ZERO";
		case PC_MOVE_TO_NEW_DROPOFF:
			return "PC_MOVE_TO_NEW_DROPOFF";
		default:
			return "STATE UNDEFINED";
	}
}

void *connectionListener(void*) {
	int server_fd;
	int new_socket;
	int valread;
	struct sockaddr_in address;
	int opt = 1;
	int addrlen = sizeof(address);
	int bufferLength = 2048;
	char buffer[bufferLength] = { 0 };
	// Creating socket file descriptor
	if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
		perror("socket failed");
		exit(EXIT_FAILURE);
	}

	if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
		perror("setsockopt");
		exit(EXIT_FAILURE);
	}
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons( PORT);
	if (bind(server_fd, (struct sockaddr *) &address, sizeof(address)) < 0) {
		perror("bind failed");
		exit(EXIT_FAILURE);
	}

	if (listen(server_fd, 3) < 0) {
		perror("listen");
		exit(EXIT_FAILURE);
	}
	while (true) {
		if ((new_socket = accept(server_fd, (struct sockaddr *) &address, (socklen_t*) &addrlen)) < 0) {
			perror("accept");
		}
		while (true) {
			valread = read(new_socket, buffer, 1024);
			buffer[1023] = 0;
			if (valread > 0) {
				if (!sendCommand) {
					parseStringForCommand(buffer, valread);
					if (sendCommand) {
						printf("%s: Command sent by client: %d\n", inet_ntoa(address.sin_addr), command);
					}
				}
				if (compareCommands(buffer, "exit")) {
					close(new_socket);
					break;
				}
				if (compareCommands(buffer, "status")) {
					int size = snprintf(buffer, bufferLength, "\n%s\n %s\n", "======== Status ========",
							robotStatusToJSON(&robotStatus, robotout)->dump().c_str());
					send(new_socket, buffer, size, MSG_DONTWAIT);
				}
				if (compareCommands(buffer, "pstatus")) {
					int prettyPrint = 4;
					int size = snprintf(buffer, bufferLength, "\n%s\n %s\n", "======== Status ========",
							robotStatusToJSON(&robotStatus, robotout)->dump(prettyPrint).c_str()); // Pretty printing
					send(new_socket, buffer, size, MSG_DONTWAIT);
				}
			}
		}
	}
	return NULL;
}

bool compareCommands(char * str1, const char *str2) {
	unsigned int length = strlen(str2);
	return length <= strlen(str1) && strncmp(str1, str2, length) == 0;
}

void parseStringForCommand(char *buffer, int length) {
	bool hasComma = false;
	for (int i = 0; i < length; i++) {
		if (buffer[i] == ',') {
			hasComma = true;
			break;
		}
	}
	if (compareCommands(buffer, "json={")) {
		sendCommand = true;
		command = COMMAND_LOAD_CONFIG;
		json json;
		configParser.loadJSONFromString(std::string(buffer).substr(5), &json);
		configParser.parseConfig(&robotin, &json);
	}

	else if (!hasComma) {
		sendCommand = true;
		if (compareCommands(buffer, "estop")) {
			command = COMMAND_EMERGENCY_STOP;
		} else if (compareCommands(buffer, "pick")) {
			command = COMMAND_PICK_ITEM;
		} else if (compareCommands(buffer, "vcon")) {
			command = COMMAND_VAC_ON;
		} else if (compareCommands(buffer, "vcoff")) {
			command = COMMAND_VAC_OFF;
		} else if (compareCommands(buffer, "zero")) {
			command = COMMAND_ZERO_RETURN;
		} else if (compareCommands(buffer, "zstg")) {
			command = COMMAND_ZERO_STAGE;
		} else if (compareCommands(buffer, "zneeded")) {
			command = COMMAND_ZERO_IF_NEEDED;
		} else if (compareCommands(buffer, "drop")) {
			command = COMMAND_DROP_ITEM;
		} else if (compareCommands(buffer, "reset")) {
			command = COMMAND_RESET;
		} else if (compareCommands(buffer, "newbox")) {
			command = COMMAND_NEW_BOX_ADDED;
		} else {
			sendCommand = false;
		}

	} else {
		//Positive values are invalid - valid will be commanded
		memcpy(target, invalidTarget, sizeof(int) * 3);

		if (compareCommands(buffer, "target=")) {
			buffer += sizeof(char) * strlen("target=");
			command = COMMAND_TARGET;
		}
		else if (compareCommands(buffer, "place=")) {
			buffer += sizeof(char) * strlen("place=");
			command = COMMAND_PLACE;
		}
		else {
			command = COMMAND_AXIS;
		}

		target[0] = strtol(buffer, NULL, 10);
		if (nextInt(&buffer)) {
			target[1] = strtol(buffer, NULL, 10);
			if (nextInt(&buffer)) {
				target[2] = strtol(buffer, NULL, 10);
			}
		}
		sendCommand = true;
	}
}

bool nextInt(char** buffer) {
	while (**buffer != 0 && **buffer != ',') {
		(*buffer)++;
	};
	(*buffer)++;
	return *(*buffer - 1) == ',';
}

void writeToFile(std::ostream &file, std::string values) {
	file << values << std::endl;
	file.flush();
}

std::string getErrorFlag(ERROR_STATUS error) {
	switch (error) {
		case ES_NONPERFORMABLE_COMMAND:
			return "EF_NONPERFORMABLE_COMMAND";
		case ES_WRONG_COMMAND:
			return "EF_WRONG_COMMAND";
		case ES_UNDERVOLTAGE_LOCKOUT:
			return "EF_UNDERVOLTAGE_LOCKOUT";
		case ES_THERMAL_WARNING:
			return "EF_THERMAL_WARNING";
		case ES_THERMAL_SHUTDOWN:
			return "EF_THERMAL_SHUTDOWN";
		case ES_OVERCURRENT_DETECTION:
			return "EF_OVERCURRENT_DETECTION";
		case ES_SENSOR_STALL_DETECTED_ON_A:
			return "EF_SENSOR_STALL_DETECTED_ON_A";
		case ES_SENSOR_STALL_DETECTED_ON_B:
			return "EF_SENSOR_STALL_DETECTED_ON_B";
		case ES_AXIS_TARGET_OUT_OF_BOUNDS:
			return "EF_AXIS_TARGET_OUT_OF_BOUNDS";
		case ES_VACUUM_SENSOR_MISREAD:
			return "ES_VACUUM_SENSOR_MISREAD";
		default: return "BAD_ERROR";
	}
}

std::string getErrorLevel(ERROR_LEVEL level) {
	switch(level) {
			case EL_INFO:
				return "EL_INFO";
			case EL_STOP:
				return "EL_STOP";
			case EL_STOP_AND_ZERO:
				return "EL_STOP_AND_ZERO";
			case EL_KILL:
				return "EL_KILL";
			default: return "BAD_ERROR_LEVEL";
		}
}

std::string getErrorLevelInfo(ERROR_LEVEL level) {
	switch(level) {
		case EL_INFO:
			return "Informational. (NO ACTION NEEDED).";
		case EL_STOP:
			return "Stop all motion and sensor readings. (RESET NEEDED).";
		case EL_STOP_AND_ZERO:
			return "Stop all motion, sensor readings, and re-zero the machine. (RESET NEEDED).";
		case EL_KILL:
			return "Hardware failure. Stop all motion and sensor readings. (HARD_REBOOT NEEDED).";
		default: return "BAD_ERROR_LEVEL";
	}
}

std::string getErrorInfo(ERROR_STATUS status) {
	switch (status) {
		case ES_NONPERFORMABLE_COMMAND:
			return "Command cannot be performed. Register attempted to write to is busy.";
		case ES_WRONG_COMMAND:
			return "Command does not exist.";
		case ES_UNDERVOLTAGE_LOCKOUT:
			return "Motor supply voltage low. The current voltage used to power the motors has fallen below the preset allowable minimum."
					"No motion commands can be performed.";
		case ES_THERMAL_WARNING:
			return "Internal temperature exceeded preset thermal warning threshold.";
		case ES_THERMAL_SHUTDOWN:
			return "Internal temperature exceeded preset thermal shutdown level. Power bridges have been disabled.";
		case ES_OVERCURRENT_DETECTION:
			return "Power MOSFETs have exceeded a programmed over-current threshold.";
		case ES_SENSOR_STALL_DETECTED_ON_A:
		case ES_SENSOR_STALL_DETECTED_ON_B:
			return "Speed and/or load angle caused motor stall.";
		case ES_AXIS_TARGET_OUT_OF_BOUNDS:
			return "Commanded target is out of bounds.";
		case ES_VACUUM_SENSOR_MISREAD:
			return "Vacuum sensor misread.";
		default: return "BAD_ERROR_STATUS";
	}
}

void displayErrors(ROBOT_OUT rout) {
	if (rout.operatingErrors.numberOfErrors > 0 && lastErrorLevel < rout.operatingErrors.priorityError) {
		lastErrorLevel = rout.operatingErrors.priorityError;
		printf("======== Errors ========\n");
		for (unsigned int error = 0; error < ES_NUM_OF_FLAGS; error++) {
			if (rout.operatingErrors.errors[error] > EL_NO_ERROR) {
				printf("Flagged Error: %s\n", getErrorFlag(static_cast<ERROR_STATUS>(error)).c_str());
			}
		}
		printf("Number of errors: %d\n", rout.operatingErrors.numberOfErrors);
	}
}
