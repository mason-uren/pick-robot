################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/ConfigParser.cpp \
../src/SharedMemory.cpp \
../src/pick-trigger-app.cpp 

OBJS += \
./src/ConfigParser.o \
./src/SharedMemory.o \
./src/pick-trigger-app.o 

CPP_DEPS += \
./src/ConfigParser.d \
./src/SharedMemory.d \
./src/pick-trigger-app.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	armv8-rpi3-linux-gnueabihf-g++ -std=c++11 -I"/Users/masonuren/GitHub/pick-robot/pick-trigger-app/includes" -I"/Users/masonuren/GitHub/pick-robot/CommonIncludes" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


