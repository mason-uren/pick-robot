################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/Hardware/Gripper/Simulation/SimVacGripper.cpp \
../src/Hardware/Gripper/Simulation/SimVacSensor.cpp 

OBJS += \
./src/Hardware/Gripper/Simulation/SimVacGripper.o \
./src/Hardware/Gripper/Simulation/SimVacSensor.o 

CPP_DEPS += \
./src/Hardware/Gripper/Simulation/SimVacGripper.d \
./src/Hardware/Gripper/Simulation/SimVacSensor.d 


# Each subdirectory must supply rules for building sources it contributes
src/Hardware/Gripper/Simulation/%.o: ../src/Hardware/Gripper/Simulation/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -std=c++11 -D__cplusplus=201103L -I"/Users/masonuren/GitHub/pick-robot/pick-robot/includes" -I"/Users/masonuren/GitHub/pick-robot/CommonIncludes" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


