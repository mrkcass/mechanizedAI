
PROJ_NAME=Stepper_L298N_MCU
PROJ_C_FILES = main.c steppermotor.c wire.c
PROJ_EXTRA_INLCUDE_PATHS =
PROJ_EXTRA_LIB_PATHS =
PROJ_EXTRA_LIBS =

MCU_SDK_DIR=~/edison/tools/mcusdk-1.0.10

PROJ_SRC_DIR = src
PROJ_EXTRA_INLCUDE_PATHS += $(PROJ_SRC_DIR)
PROJ_OBJ_DIR = obj
PROJ_BIN_DIR = bin
PROJ_LIB_DIR = lib

GCC_DIR=$(MCU_SDK_DIR)/toolchain/linux-x86_64/i686-elf-gcc
GCC_BIN_DIR=$(GCC_DIR)/bin
GCC_INCLUDE_DIR=$(GCC_DIR)/i686-elf/include $(GCC_DIR)/i686-elf/usr/include
GCC_INCLUDE_DIR+=$(GCC_DIR)/lib/gcc/i686-elf/4.6.1/include $(GCC_DIR)/lib/gcc/i686-elf/4.6.1/include-fixed
GCC_LIB_DIR=$(GCC_DIR)/lib

MCU_SDK_FIRWARE_DIR=$(MCU_SDK_DIR)/src/firmware
MCU_SDK_FIRWARE_INCLUDE_DIR=$(MCU_SDK_DIR)/src/firmware/include
MCU_SDK_FIRWARE_TOOLS_DIR=$(MCU_SDK_FIRWARE_DIR)/internal_tools

GCC_TOOL_PREFIX=i686-elf-

CC=$(GCC_BIN_DIR)/$(GCC_TOOL_PREFIX)gcc
AR = $(GCC_BIN_DIR)/$(GCC_TOOL_PREFIX)ar
RANLIB = $(GCC_BIN_DIR)/$(GCC_TOOL_PREFIX)ranlib
LD = $(GCC_BIN_DIR)/$(GCC_TOOL_PREFIX)ld
NM = $(GCC_BIN_DIR)/$(GCC_TOOL_PREFIX)nm
STRIP = $(GCC_BIN_DIR)/$(GCC_TOOL_PREFIX)strip
INSTALL_STRIP =

LINUX_PATH = /usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin
INCLUDE_PATHS =  $(addprefix -I , $(GCC_INCLUDE_DIR))
INCLUDE_PATHS += $(addprefix -I , $(PROJ_EXTRA_INLCUDE_PATHS))
INCLUDE_PATHS += $(addprefix -I , $(MCU_SDK_FIRWARE_INCLUDE_DIR))
CFLAGS = -Wall -O2

LIB_PATHS =-L $(GCC_LIB_DIR) $(addprefix -L , $(PROJ_EXTRA_LIB_PATHS))
LIBS = $(addprefix -l , $(PROJ_EXTRA_LIBS))
LDFLAGS = -Wall

PROJ_TARGET = $(PROJ_BIN_DIR)/$(PROJ_NAME).a

OBJS = $(patsubst %.c,$(PROJ_OBJ_DIR)/%.o,$(PROJ_C_FILES))

SSH_TARGET = root@192.168.2.15
SSH_IP_ADDR = 192.168.2.15


#$(PROJ_TARGET): $(PROJ_OBJ_DIR) $(PROJ_BIN_DIR) $(OBJS)
#	@echo "linking...."
#	$(CC) $(LDFLAGS) $(LIB_PATHS) $(LIBS) -o $(PROJ_TARGET) $(OBJS)

$(PROJ_TARGET): $(PROJ_OBJ_DIR) $(PROJ_BIN_DIR) $(OBJS)
	@echo "libbing...."
	@$(AR) -r $(PROJ_TARGET) $(OBJS)
	MCUSDK_PATH=$(MCU_SDK_DIR) $(MAKE) --no-print-directory generate_mcu_bin

$(PROJ_OBJ_DIR)/%.o: $(PROJ_SRC_DIR)/%.c
	@echo "compiling...."
	$(CC) $(CFLAGS) $(INCLUDE_PATHS) -c $< -o $@

$(PROJ_OBJ_DIR):
	@echo "creating object directory...."
	@mkdir -p $(PROJ_OBJ_DIR)

$(PROJ_BIN_DIR):
	@echo "creating bin directory...."
	@mkdir -p $(PROJ_BIN_DIR)

generate_mcu_bin:
	@echo 'bining....'
	@cd $(PROJ_BIN_DIR); \
	cp -rf $(MCU_SDK_FIRWARE_DIR)/lib ..; \
	cp -rf $(MCU_SDK_FIRWARE_DIR)/include include; \
	PATH=$(LINUX_PATH):$(GCC_BIN_DIR); \
	MCUSDK_PATH=$(MCU_SDK_DIR); \
	MCUSDK_OS="linux-x86_64"; \
	$(MCU_SDK_FIRWARE_TOOLS_DIR)/generate_mcu_bin.sh  ../$(PROJ_TARGET); \
	cd -

install:
	@echo 'installing....'
	@cd $(PROJ_BIN_DIR); \
	cp -rf $(MCU_SDK_FIRWARE_DIR)/internal_tools/download.sh  . ;\
	SSH_USER="root" SSH_IP_ADDR="192.168.2.15" ./download.sh install . ;\
	cd -

reboot:
	@echo 'rebooting....'
	@ssh -o StrictHostKeyChecking=no -o UserKnownHostsFile=/dev/null $(SSH_TARGET) reboot || true

clean:
	@echo "cleaning...."
	rm -rf $(PROJ_OBJ_DIR)
	rm -rf $(PROJ_BIN_DIR)