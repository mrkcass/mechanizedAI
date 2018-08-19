

PROJ_NAME=onboardstick-1x3D

OBJECT_FILES = main.o joystick.o wire.o i2c_interface.o adc50.o

SRC_DIR = src
OBJ_DIR = build

CC = g++
INLUDE_PATHS = -I. -I/usr/include
CFLAGS = -Wall -O2

LIB_PATHS = -L/usr/lib
LIBS=-lpthread -lmraa

OBJ = $(patsubst %,$(OBJ_DIR)/%,$(OBJECT_FILES))

all: $(OBJ_DIR) start_build_message $(PROJ_NAME)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CC) -c -o $@ $< $(CFLAGS) $(INCLUDE_PATHS)

$(PROJ_NAME): $(OBJ)
	 $(CC) -o $@ $^ $(CFLAGS) $(LIB_PATHS) $(LIBS)

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