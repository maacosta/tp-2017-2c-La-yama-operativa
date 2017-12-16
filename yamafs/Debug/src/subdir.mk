################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/archivo.c \
../src/bitmap.c \
../src/configuration.c \
../src/consola.c \
../src/directorio.c \
../src/filesystem.c \
../src/main.c \
../src/md5.c \
../src/nodo.c \
../src/op_nodos.c \
../src/server.c 

OBJS += \
./src/archivo.o \
./src/bitmap.o \
./src/configuration.o \
./src/consola.o \
./src/directorio.o \
./src/filesystem.o \
./src/main.o \
./src/md5.o \
./src/nodo.o \
./src/op_nodos.o \
./src/server.o 

C_DEPS += \
./src/archivo.d \
./src/bitmap.d \
./src/configuration.d \
./src/consola.d \
./src/directorio.d \
./src/filesystem.d \
./src/main.d \
./src/md5.d \
./src/nodo.d \
./src/op_nodos.d \
./src/server.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


