################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
/media/pablo/HD1/Programacion/ethanon/toolkit/Source/src/box2d/Box2D/Rope/b2Rope.cpp 

OBJS += \
./Box2D/Rope/b2Rope.o 

CPP_DEPS += \
./Box2D/Rope/b2Rope.d 


# Each subdirectory must supply rules for building sources it contributes
Box2D/Rope/b2Rope.o: /media/pablo/HD1/Programacion/ethanon/toolkit/Source/src/box2d/Box2D/Rope/b2Rope.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	g++ -I../../../ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


