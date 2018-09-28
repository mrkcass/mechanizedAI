

PROJ_NAME=libhardware.a

OBJECT_FILES = ahrs.o						\
					adc50.o						\
					bno055.o						\
					joystick3d_stick.o 		\
					mcu_motor_controller.o 	\
					atom_motor_controller.o \
					ledstrip-ws2812b.o		\
					micarray-ics52000.o		\
					wire.o 						\
					i2c_interface.o 			\
					somax.o

SRC_DIR = src
OBJ_DIR = build

AR = ar
CC = g++
INCLUDE_PATHS = -I. -I/usr/include
CFLAGS = -Wall -O2

LIB_PATHS = -L/usr/lib
LIBS=-lpthread -lmraa

OBJ = $(patsubst %,$(OBJ_DIR)/%,$(OBJECT_FILES))

all: $(OBJ_DIR) start_build_message $(PROJ_NAME)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CC) -c -o $@ $< $(CFLAGS) $(INCLUDE_PATHS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) -c -o $@ $< $(CFLAGS) $(INCLUDE_PATHS)

$(PROJ_NAME): $(OBJ)
	 $(AR) rcs $@ $^

start_build_message:
	$(info #)
	$(info #)
	$(info #)
	$(info #)
	$(info #)
	$(info Building: $(PROJ_NAME))
	$(info #)

start_clean_message:
	$(info #)
	$(info #)
	$(info #)
	$(info #)
	$(info #)
	$(info Cleaning: $(PROJ_NAME))
	$(info #)


.PHONY: clean

clean: start_build_message
	rm -f $(OBJ_DIR)/*.o $(PROJ_NAME)