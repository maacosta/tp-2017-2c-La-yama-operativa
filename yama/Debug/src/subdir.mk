################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/configuration.c \
../src/detalles_nodo.c \
../src/estados_master.c \
../src/filesystem.c \
../src/log_planificador.c \
../src/main.c \
../src/op_almacenamiento.c \
../src/op_jobs_estados_master.c \
../src/op_reduccion.c \
../src/op_transformacion.c \
../src/server.c 

OBJS += \
./src/configuration.o \
./src/detalles_nodo.o \
./src/estados_master.o \
./src/filesystem.o \
./src/log_planificador.o \
./src/main.o \
./src/op_almacenamiento.o \
./src/op_jobs_estados_master.o \
./src/op_reduccion.o \
./src/op_transformacion.o \
./src/server.o 

C_DEPS += \
./src/configuration.d \
./src/detalles_nodo.d \
./src/estados_master.d \
./src/filesystem.d \
./src/log_planificador.d \
./src/main.d \
./src/op_almacenamiento.d \
./src/op_jobs_estados_master.d \
./src/op_reduccion.d \
./src/op_transformacion.d \
./src/server.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


