################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/Hardware/Motors/MotorFactory.cpp \
../src/Hardware/Motors/StepperMotor.cpp 

OBJS += \
./src/Hardware/Motors/MotorFactory.o \
./src/Hardware/Motors/StepperMotor.o 

CPP_DEPS += \
./src/Hardware/Motors/MotorFactory.d \
./src/Hardware/Motors/StepperMotor.d 


# Each subdirectory must supply rules for building sources it contributes
src/Hardware/Motors/%.o: ../src/Hardware/Motors/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	armv8-rpi3-linux-gnueabihf-g++ -std=c++11 -D__cplusplus=201103L -I"/Users/masonuren/GitHub/pick-robot/pick-robot/includes" -I"/Users/masonuren/GitHub/pick-robot/CommonIncludes" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


