################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/apareo.c \
../src/configuration.c \
../src/main.c \
../src/operaciones.c \
../src/operaciones_local.c \
../src/server.c 

OBJS += \
./src/apareo.o \
./src/configuration.o \
./src/main.o \
./src/operaciones.o \
./src/operaciones_local.o \
./src/server.o 

C_DEPS += \
./src/apareo.d \
./src/configuration.d \
./src/main.d \
./src/operaciones.d \
./src/operaciones_local.d \
./src/server.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


