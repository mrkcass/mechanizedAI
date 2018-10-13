

PROJ_NAME=somax_ui

OBJECT_FILES = somaxui_main.o

SRC_DIR = src
OBJ_DIR = build

SERVICE_COMMAND = systemctl
SERVICE_CONFIG_DIR = /etc/systemd/system

CC = g++
INCLUDE_PATHS = -I. -I/usr/include -I../libhardware/src
CFLAGS = -Wall -O2

LIB_PATHS = -L/usr/lib -L../libhardware
LIBS = -lpthread -lmraa -lhardware

OBJ = $(patsubst %,$(OBJ_DIR)/%,$(OBJECT_FILES))

all: $(OBJ_DIR) start_build_message $(PROJ_NAME)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CC) -c -o $@ $< $(CFLAGS) $(INCLUDE_PATHS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) -c -o $@ $< $(CFLAGS) $(INCLUDE_PATHS)

$(PROJ_NAME): $(OBJ)
	 $(CC) -o $@ $^ $(CFLAGS) $(LIB_PATHS) $(LIBS)

service-install: $(PROJ_NAME) $(PROJ_NAME).service start_install_message service-copy service-enable
	$(info )

service-enable: $(PROJ_NAME) $(PROJ_NAME).service
	@$(SERVICE_COMMAND) enable $(PROJ_NAME).service
	$(info enabled $(PROJ_NAME).service)

service-disable: $(PROJ_NAME) $(PROJ_NAME).service
	@$(SERVICE_COMMAND) disable $(PROJ_NAME).service
	$(info disabled $(PROJ_NAME).service)

service-copy: $(PROJ_NAME) $(PROJ_NAME).service
	@cp $(PROJ_NAME).service $(SERVICE_CONFIG_DIR)
	$(info copied $(PROJ_NAME).service to $(SERVICE_CONFIG_DIR))

list:
	@$(MAKE) -pRrq -f $(lastword $(MAKEFILE_LIST)) : 2>/dev/null | awk -v RS= -F: '/^# File/,/^# Finished Make data base/ {if ($$1 !~ "^[#.]") {print $$1}}' | sort | egrep -v -e '^[^[:alnum:]]' -e '^$@$$' | xargs

start_install_message:
	$(info #)
	$(info #)
	$(info #)
	$(info #)
	$(info #)
	$(info Installing: $(PROJ_NAME).service)
	$(info )


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