

PROJ_NAME=ledstrip-ws2812b

OBJECT_FILES = ledstrip-ws2812b.o

SRC_DIR = src
OBJ_DIR = build

CC = g++
INLUDE_PATHS = -I. -I/usr/include
CFLAGS = -Wall -O2

LIB_PATHS = -L/usr/lib
LIBS=-lpthread -lmraa

OBJ = $(patsubst %,$(OBJ_DIR)/%,$(OBJECT_FILES))

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) -c -o $@ $< $(CFLAGS) $(INCLUDE_PATHS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CC) -c -o $@ $< $(CFLAGS) $(INCLUDE_PATHS)

$(PROJ_NAME): $(OBJ)
	 $(CC) -o $@ $^ $(CFLAGS) $(LIB_PATHS) $(LIBS)

.PHONY: clean

clean:
	rm -f $(OBJ_DIR)/*.o $(PROJ_NAME)