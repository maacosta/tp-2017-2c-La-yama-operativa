################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/configuration.c \
../src/main.c \
../src/op_almacenamiento.c \
../src/op_reduccion.c \
../src/op_transformacion.c 

OBJS += \
./src/configuration.o \
./src/main.o \
./src/op_almacenamiento.o \
./src/op_reduccion.o \
./src/op_transformacion.o 

C_DEPS += \
./src/configuration.d \
./src/main.d \
./src/op_almacenamiento.d \
./src/op_reduccion.d \
./src/op_transformacion.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


