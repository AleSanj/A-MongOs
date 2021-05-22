################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/TAD_PATOTA.c \
../src/TAD_TRIPULANTE.c \
../src/conexion.c \
../src/serializacion.c 

OBJS += \
./src/TAD_PATOTA.o \
./src/TAD_TRIPULANTE.o \
./src/conexion.o \
./src/serializacion.o 

C_DEPS += \
./src/TAD_PATOTA.d \
./src/TAD_TRIPULANTE.d \
./src/conexion.d \
./src/serializacion.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -fPIC -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


