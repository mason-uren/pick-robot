################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include src/Utilities/subdir.mk
-include src/Software/ZeroReturn/subdir.mk
-include src/Software/TargetGeneration/subdir.mk
-include src/Software/PickControl/subdir.mk
-include src/Software/MotorController/subdir.mk
-include src/Software/ErrorHandler/subdir.mk
-include src/Software/CommandHandler/subdir.mk
-include src/Hardware/PinInteractions/subdir.mk
-include src/Hardware/Motors/Simulation/subdir.mk
-include src/Hardware/Motors/subdir.mk
-include src/Hardware/Gripper/Simulation/subdir.mk
-include src/Hardware/Gripper/subdir.mk
-include src/subdir.mk
-include .metadata/.plugins/org.eclipse.cdt.make.core/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(CC_DEPS)),)
-include $(CC_DEPS)
endif
ifneq ($(strip $(C++_DEPS)),)
-include $(C++_DEPS)
endif
ifneq ($(strip $(C_UPPER_DEPS)),)
-include $(C_UPPER_DEPS)
endif
ifneq ($(strip $(CXX_DEPS)),)
-include $(CXX_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(CPP_DEPS)),)
-include $(CPP_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: pick-robot

# Tool invocations
pick-robot: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: Cross G++ Linker'
	armv8-rpi3-linux-gnueabihf-g++ -L"/Users/masonuren/GitHub/pick-robot/pick-robot/lib" -o "pick-robot" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(CC_DEPS)$(C++_DEPS)$(EXECUTABLES)$(OBJS)$(C_UPPER_DEPS)$(CXX_DEPS)$(C_DEPS)$(CPP_DEPS) pick-robot
	-@echo ' '

.PHONY: all clean dependents

-include ../makefile.targets
