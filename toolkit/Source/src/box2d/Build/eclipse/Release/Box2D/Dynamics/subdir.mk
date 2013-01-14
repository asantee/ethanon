################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
/media/pablo/HD1/Programacion/ethanon/toolkit/Source/src/box2d/Box2D/Dynamics/b2Body.cpp \
/media/pablo/HD1/Programacion/ethanon/toolkit/Source/src/box2d/Box2D/Dynamics/b2ContactManager.cpp \
/media/pablo/HD1/Programacion/ethanon/toolkit/Source/src/box2d/Box2D/Dynamics/b2Fixture.cpp \
/media/pablo/HD1/Programacion/ethanon/toolkit/Source/src/box2d/Box2D/Dynamics/b2Island.cpp \
/media/pablo/HD1/Programacion/ethanon/toolkit/Source/src/box2d/Box2D/Dynamics/b2World.cpp \
/media/pablo/HD1/Programacion/ethanon/toolkit/Source/src/box2d/Box2D/Dynamics/b2WorldCallbacks.cpp 

OBJS += \
./Box2D/Dynamics/b2Body.o \
./Box2D/Dynamics/b2ContactManager.o \
./Box2D/Dynamics/b2Fixture.o \
./Box2D/Dynamics/b2Island.o \
./Box2D/Dynamics/b2World.o \
./Box2D/Dynamics/b2WorldCallbacks.o 

CPP_DEPS += \
./Box2D/Dynamics/b2Body.d \
./Box2D/Dynamics/b2ContactManager.d \
./Box2D/Dynamics/b2Fixture.d \
./Box2D/Dynamics/b2Island.d \
./Box2D/Dynamics/b2World.d \
./Box2D/Dynamics/b2WorldCallbacks.d 


# Each subdirectory must supply rules for building sources it contributes
Box2D/Dynamics/b2Body.o: /media/pablo/HD1/Programacion/ethanon/toolkit/Source/src/box2d/Box2D/Dynamics/b2Body.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	g++ -I../../../ -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Box2D/Dynamics/b2ContactManager.o: /media/pablo/HD1/Programacion/ethanon/toolkit/Source/src/box2d/Box2D/Dynamics/b2ContactManager.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	g++ -I../../../ -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Box2D/Dynamics/b2Fixture.o: /media/pablo/HD1/Programacion/ethanon/toolkit/Source/src/box2d/Box2D/Dynamics/b2Fixture.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	g++ -I../../../ -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Box2D/Dynamics/b2Island.o: /media/pablo/HD1/Programacion/ethanon/toolkit/Source/src/box2d/Box2D/Dynamics/b2Island.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	g++ -I../../../ -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Box2D/Dynamics/b2World.o: /media/pablo/HD1/Programacion/ethanon/toolkit/Source/src/box2d/Box2D/Dynamics/b2World.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	g++ -I../../../ -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Box2D/Dynamics/b2WorldCallbacks.o: /media/pablo/HD1/Programacion/ethanon/toolkit/Source/src/box2d/Box2D/Dynamics/b2WorldCallbacks.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	g++ -I../../../ -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


