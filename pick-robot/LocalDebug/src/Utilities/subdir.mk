################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/Utilities/Axis.cpp \
../src/Utilities/ExampleComponent.cpp \
../src/Utilities/SharedMemory.cpp 

OBJS += \
./src/Utilities/Axis.o \
./src/Utilities/ExampleComponent.o \
./src/Utilities/SharedMemory.o 

CPP_DEPS += \
./src/Utilities/Axis.d \
./src/Utilities/ExampleComponent.d \
./src/Utilities/SharedMemory.d 


# Each subdirectory must supply rules for building sources it contributes
src/Utilities/%.o: ../src/Utilities/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -std=c++11 -D__cplusplus=201103L -I"/Users/masonuren/GitHub/pick-robot/pick-robot/includes" -I"/Users/masonuren/GitHub/pick-robot/CommonIncludes" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


