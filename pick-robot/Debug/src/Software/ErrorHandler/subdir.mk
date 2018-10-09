################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/Software/ErrorHandler/ErrorHandler.cpp 

OBJS += \
./src/Software/ErrorHandler/ErrorHandler.o 

CPP_DEPS += \
./src/Software/ErrorHandler/ErrorHandler.d 


# Each subdirectory must supply rules for building sources it contributes
src/Software/ErrorHandler/%.o: ../src/Software/ErrorHandler/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	armv8-rpi3-linux-gnueabihf-g++ -std=c++11 -D__cplusplus=201103L -I"/Users/masonuren/GitHub/pick-robot/pick-robot/includes" -I"/Users/masonuren/GitHub/pick-robot/CommonIncludes" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


