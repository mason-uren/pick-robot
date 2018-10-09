#include "main.h"

#include <ConfigStruct.h>
#include <errno.h>
#include <json.hpp>
#include <sched.h>
#include <slushboard.h>
#include <sys/time.h>
#include <SharedMemoryStructs.h>
#include <unistd.h>
#include <array>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "Hardware/Gripper/GripperFactory.h"
#include "Hardware/Gripper/VacuumGripper.h"
#include "Hardware/Gripper/VacuumSensor.h"
#include "Hardware/Motors/MotorFactory.h"
#include "Hardware/Motors/MotorInterface.h"
#include "Hardware/PinInteractions/I2C.h"
#include "Software/CommandHandler/CommandHandler.h"
#include "Software/ErrorHandler/ErrorHandler.h"
#include "Software/MotorController/MotorController.h"
#include "Software/PickControl/PickControl.h"
#include "Software/TargetGeneration/TargetGenerator.h"
#include "Software/ZeroReturn/ZeroReturnController.h"
#include "Utilities/Axis.h"
#include "Utilities/SharedMemory.h"

// for convenience
using json = nlohmann::json;

#define USEC_PER_SEC		1000000L
#define NSEC_PER_SEC		1000000000L
#define NANO_INC            1000000L

static std::vector<ComponentInterface*> components;
static inline void tsnorm(struct timespec *ts) {
	while (ts->tv_nsec >= NSEC_PER_SEC) {
		ts->tv_nsec -= NSEC_PER_SEC;
		ts->tv_sec++;
	}
}

static bool prioritySet = false;
static ROBOT_OUT status;
static SharedMemory* sharedMemory;
static SlushBoard * slushboard;
static PickControl *pickControl;
static MotorController *motorController;
static ZeroReturnController * zc;
static Gripper *vc;
static ROBOT_IN robotIn;
static TargetGenerator* tg;
static CommandHandler* commandHandler;
static I2C *i2c;

// For Testing
void writeToFile(std::string filename, std::string values);
void collectSuctionData();
void testTargetGenerator();

int main(int argc, char **argv) {
	jsonInitialization();
	realTimeLoop();
	return 0;
}

void jsonInitialization() {
	status = {0};
	sharedMemory = new SharedMemory();
	slushboard = new SlushBoard();

	printf("Waiting for config over shared memory\n");
	while (true) {
		if (!sharedMemory->readRobotIn(&robotIn) && robotIn.config.hash != 0) {
			printf("Configuration received.\n");
			break;
		}
		usleep(500);
	}

	if (robotIn.config.runtimeFlags.simulate) {
		std::cout << "SIMULATE MODE" << std::endl;
	}
	else {
		std::cout << "LIVE MODE" << std::endl;
	}
	if (robotIn.config.runtimeFlags.realtime) {
		std::cout << "REALTIME MODE" << std::endl;
	}
	else {
		std::cout << "NON-REALTIME MODE" << std::endl;
	}


	std::array<Axis *, NUM_AXES> axes;
	AXIS_CONFIG *axisConfig = robotIn.config.axes;
	for(int i = 0; i < NUM_AXES; i++) {
		if(axisConfig[i].valid) {
			std::vector<MotorInterface *> motors;
			MOTOR_CONFIG* motorConfig = axisConfig[i].motor;
			for(int j = 0; j < MAX_MOTORS_PER_AXIS; j++) {
				MOTOR_CONFIG motor = motorConfig[j];
				if(motor.valid) {
					motors.push_back(MotorFactory::create(robotIn.config.runtimeFlags.simulate, &motor));
					components.push_back(MotorFactory::create(robotIn.config.runtimeFlags.simulate, &motor));
				}
			}
			switch(axisConfig[i].axisLabel)
			{
				case 'X':
					axes[X] = new Axis(AXIS::X, &axisConfig[i], motors);
					break;
				case 'Y':
					axes[Y] = new Axis(AXIS::Y, &axisConfig[i], motors);
					break;
				case 'Z':
					axes[Z] = new Axis(AXIS::Z, &axisConfig[i], motors);
					break;
			}
		}
	}
	i2c = new I2C(CONFIG::ADS1x15_DEFAULT_ADDRESS);
	vc = GripperFactory::create(robotIn.config.runtimeFlags.simulate, slushboard);

	status.runtimeFlags.realtime = robotIn.config.runtimeFlags.realtime;
	status.runtimeFlags.simulate = robotIn.config.runtimeFlags.simulate;
	status.runtimeFlags.logAxesData = robotIn.config.runtimeFlags.logAxesData;
	status.runtimeFlags.ignoreErrorFlags = robotIn.config.runtimeFlags.ignoreErrorFlags;

	motorController = new MotorController(axes);
	zc = new ZeroReturnController(motorController);

	tg = new TargetGenerator( &robotIn.config.targetGeneratorConfig);
	pickControl = new PickControl(sharedMemory, motorController, vc, zc, tg);
	ErrorHandler::getInstance()->shouldIgnoreErrors(robotIn.config.runtimeFlags.ignoreErrorFlags);
	components.push_back(ErrorHandler::getInstance());
	components.push_back(pickControl);
	components.push_back(motorController);
	components.push_back(vc);
	components.push_back(zc);
	commandHandler = new CommandHandler(sharedMemory, pickControl, zc, motorController, vc, tg);
}

void setPriority() {
#ifndef LOCAL
	int ret;
	struct sched_param params;
	struct sched_param sysparams;
	int pid = getpid();
	// We'll set the priority to the maximum.
	params.__sched_priority = sched_get_priority_max(SCHED_FIFO);

	// Attempt to set thread real-time priority to the SCHED_FIFO policy
	ret = sched_setscheduler(pid, SCHED_FIFO, &params);
	//ret = pthread_setschedparam(this_thread, SCHED_FIFO, &params);
	if (ret != 0) {
		// Print the error
		int error = errno;
		printf("Unsuccessful in setting thread realtime prio: %d\n", error);
		return;
	}
	// Now verify the change in thread priority
	int policy = 0;
	ret = sched_getparam(pid, &sysparams);
	if (ret != 0) {
		printf("Couldn't retrieve real-time scheduling paramers\n");
		return;
	}

	policy = sched_getscheduler(pid);

	// Check the correct policy was applied
	prioritySet = policy == SCHED_FIFO;
	if (prioritySet) {
		printf("SCHED_FIFO OK\n");
	} else {
		printf("Scheduling is NOT SCHED_FIFO!\n");
		printf("Policy is: %d\n", policy);
	}

	// Print thread scheduling priority
	printf("Process priority is %d\n", sysparams.sched_priority);
#endif
}

/**
 * Run the real time loop and monitor for deadline violations
 */
void realTimeLoop() {
	setPriority();
	static long long int clockTicks;
	struct timespec timespec;
	clock_gettime(CLOCK_MONOTONIC, &timespec);
	timespec.tv_nsec += NANO_INC;
	tsnorm(&timespec);
	struct timespec after;
	long int secs = timespec.tv_sec;
	bool deadlineViolation = false;

	while (true) {
#ifndef LOCAL
		static int clockReturn = 0;
		clockReturn = clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &timespec, NULL);
		if (clockReturn != 0) {
			//if (clockReturn != EINTR)
				printf("clock_nanosleep failed. errno: %d clockReturn: %d\n",
						errno, clockReturn);
			break;
		}
		clock_gettime(CLOCK_MONOTONIC, &after);
		if (after.tv_sec > timespec.tv_sec) {
			after.tv_nsec += NSEC_PER_SEC * (after.tv_sec - timespec.tv_sec);
		}
		//Only enforce deadlines if we're running in real time mode (priority set)
		if (!deadlineViolation && prioritySet && timespec.tv_nsec + NANO_INC < after.tv_nsec) {
			printf("Deadline violation: %f ms.\n", ((double) after.tv_nsec - timespec.tv_nsec) / NSEC_PER_SEC * 1000);
			printf("after.tv_sec: %ld\ntimespec.tv_sec: %ld\n", after.tv_sec, timespec.tv_sec);
			printf("after.tv_nsec: %ld\ntimespec.tv_nsec: %ld\n", after.tv_nsec, timespec.tv_nsec);
			if (robotIn.config.runtimeFlags.realtime) {
				break;
			}
			deadlineViolation = true;
		}
#else
		usleep(1000);
#endif
		timespec.tv_nsec += NANO_INC;
		tsnorm(&timespec);
		clockTicks++;

		if (sharedMemory->readRobotIn(&robotIn)) {
			if (robotIn.commandStruct.command == COMMAND_EMERGENCY_STOP) {
				for (unsigned int index = 0; index < components.size(); index++) {
					components.at(index)->emergencyStop();
				}
			} else {
				commandHandler->processCommand(&robotIn);
			}
		}

		//Do real time stuff
		tick(clockTicks);

//		if (clockTicks % 50 == 0) {
			reportStatus();
//		}
	}
#ifndef LOCAL
	clock_gettime(CLOCK_MONOTONIC, &timespec);
	printf("Total Runtime: %ld seconds.\n", timespec.tv_sec - secs);
#endif
}

void tick(long long int systime) {
	static unsigned int index = 0;
	for (index = 0; index < components.size(); index++) {
		components.at(index)->step(systime);
	}
}

void reportStatus() {
	static unsigned int index = 0;
	for (index = 0; index < components.size(); index++) {
		components.at(index)->reportStatus(&status);
	}
	status.block_number++;
	sharedMemory->writeRobotOut(&status);
}

//Test functions
void testTargetGenerator() {
	int boxEnd[] = { -655, -100, -490 };
	int boxStart[] = { -455, -370, -250 };
	int delta[] = { 50, 50, 17 };
	int boxDrop[] = { -949, 0, -305 };
	TARGET_GENERATOR_CONFIG config;
	memcpy(config.boxEnd, boxEnd, sizeof(int) * 3);
	memcpy(config.boxStart, boxStart, sizeof(int) * 3);
	memcpy(config.delta, delta, sizeof(int) * 3);
	memcpy(config.dropLocation, boxDrop, sizeof(int) * 3);
	TargetGenerator * tg = new TargetGenerator(&config);
	printf("Top of box: %d\n", tg->getTopOfBoxZ());
	printf("Z Clearance Plane: %d\n", tg->getZClearancePlane());

	std::array<axis_pos, NUM_AXES> targets;
	while (!tg->isNeedNewBox()) {
		tg->getNextTarget(targets);
		printf("[%d, %d, %d]\n", targets[X], targets[Y], targets[Z]);
		printf("Z depth above item: %d\n", tg->getZDepthAboveItem());
		printf("Z probe depth: %d\n", tg->getZProbeDepth());
		tg->markPicked(targets[Z]);

	}
	exit(0);
}

void writeToFile(std::string filename, std::string values) {
	ofstream fileObj;
	fileObj.open(filename, std::ios::out | std::ios::app);
	fileObj << values;
	fileObj.close();
}

void collectSuctionData() {
	VacuumSensor *vs = new VacuumSensor();
	VacuumGripper *vg = new VacuumGripper(new SlushBoard());
	std::string values;
	int counter = 0;
	vg->activate();
	while(counter > 10000) {
		values.append(to_string(vs->getCurrentSuctionValue()));
		values.append(", ");
		counter++;
		usleep(1000);
	}
	values.pop_back();
	values.pop_back();
	vg->deactivate();
	writeToFile("data/suctionNoBag.txt", values);
}
