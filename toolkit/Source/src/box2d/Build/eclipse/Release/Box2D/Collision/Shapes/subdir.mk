################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
/media/pablo/HD1/Programacion/ethanon/toolkit/Source/src/box2d/Box2D/Collision/Shapes/b2ChainShape.cpp \
/media/pablo/HD1/Programacion/ethanon/toolkit/Source/src/box2d/Box2D/Collision/Shapes/b2CircleShape.cpp \
/media/pablo/HD1/Programacion/ethanon/toolkit/Source/src/box2d/Box2D/Collision/Shapes/b2EdgeShape.cpp \
/media/pablo/HD1/Programacion/ethanon/toolkit/Source/src/box2d/Box2D/Collision/Shapes/b2PolygonShape.cpp 

OBJS += \
./Box2D/Collision/Shapes/b2ChainShape.o \
./Box2D/Collision/Shapes/b2CircleShape.o \
./Box2D/Collision/Shapes/b2EdgeShape.o \
./Box2D/Collision/Shapes/b2PolygonShape.o 

CPP_DEPS += \
./Box2D/Collision/Shapes/b2ChainShape.d \
./Box2D/Collision/Shapes/b2CircleShape.d \
./Box2D/Collision/Shapes/b2EdgeShape.d \
./Box2D/Collision/Shapes/b2PolygonShape.d 


# Each subdirectory must supply rules for building sources it contributes
Box2D/Collision/Shapes/b2ChainShape.o: /media/pablo/HD1/Programacion/ethanon/toolkit/Source/src/box2d/Box2D/Collision/Shapes/b2ChainShape.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	g++ -I../../../ -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Box2D/Collision/Shapes/b2CircleShape.o: /media/pablo/HD1/Programacion/ethanon/toolkit/Source/src/box2d/Box2D/Collision/Shapes/b2CircleShape.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	g++ -I../../../ -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Box2D/Collision/Shapes/b2EdgeShape.o: /media/pablo/HD1/Programacion/ethanon/toolkit/Source/src/box2d/Box2D/Collision/Shapes/b2EdgeShape.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	g++ -I../../../ -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Box2D/Collision/Shapes/b2PolygonShape.o: /media/pablo/HD1/Programacion/ethanon/toolkit/Source/src/box2d/Box2D/Collision/Shapes/b2PolygonShape.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	g++ -I../../../ -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


