################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/Hardware/Gripper/GripperFactory.cpp \
../src/Hardware/Gripper/VacuumGripper.cpp \
../src/Hardware/Gripper/VacuumSensor.cpp 

OBJS += \
./src/Hardware/Gripper/GripperFactory.o \
./src/Hardware/Gripper/VacuumGripper.o \
./src/Hardware/Gripper/VacuumSensor.o 

CPP_DEPS += \
./src/Hardware/Gripper/GripperFactory.d \
./src/Hardware/Gripper/VacuumGripper.d \
./src/Hardware/Gripper/VacuumSensor.d 


# Each subdirectory must supply rules for building sources it contributes
src/Hardware/Gripper/%.o: ../src/Hardware/Gripper/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	armv8-rpi3-linux-gnueabihf-g++ -std=c++11 -D__cplusplus=201103L -I"/Users/masonuren/GitHub/pick-robot/pick-robot/includes" -I"/Users/masonuren/GitHub/pick-robot/CommonIncludes" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


