################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/Hardware/Motors/Simulation/SimMotor.cpp 

OBJS += \
./src/Hardware/Motors/Simulation/SimMotor.o 

CPP_DEPS += \
./src/Hardware/Motors/Simulation/SimMotor.d 


# Each subdirectory must supply rules for building sources it contributes
src/Hardware/Motors/Simulation/%.o: ../src/Hardware/Motors/Simulation/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	armv8-rpi3-linux-gnueabihf-g++ -std=c++11 -I"/Users/masonuren/GitHub/pick-robot/pick-robot/includes" -I"/Users/masonuren/GitHub/pick-robot/CommonIncludes" -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


