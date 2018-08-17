#include <stdio.h>

#include "mraa.h"
#include "mraa/i2c.h"

#define TEST_REG_ERROR -1

static void i2c_detect_devices(mraa_i2c_context i2c, int bus);
static mraa_result_t i2c_get(int bus, uint8_t device_address, uint8_t register_address, uint8_t* data);

int latch_device(mraa_i2c_context i2c, uint8_t device_id)
{
	mraa_result_t status = MRAA_SUCCESS;
	int error_occurred = 0;

	status = mraa_i2c_address(i2c, device_id);
	if (status != MRAA_SUCCESS)
    {
    	printf("Error: i2c couldn't latch address:%x\n", device_id);
        error_occurred = 1;
    }

    return error_occurred;
}

int write_byte(mraa_i2c_context i2c, uint8_t byte)
{
	mraa_result_t status = MRAA_SUCCESS;
	int error_occurred = 0;

    status = mraa_i2c_write_byte(i2c, byte);
    if (status != MRAA_SUCCESS)
    {
    	printf("Error: i2c couldn't write byte:%x\n", byte);
        error_occurred = 1;
    }

    return error_occurred;
}

int write_register_byte(mraa_i2c_context i2c, uint8_t register_id, uint8_t value)
{
	mraa_result_t status = MRAA_SUCCESS;
	int error_occurred = 0;

    status = mraa_i2c_write_byte_data(i2c, value, register_id);
    if (status != MRAA_SUCCESS)
    {
    	printf("Error: i2c couldn't write register (id=%x) value: %x\n", register_id, value);
        error_occurred = 1;
    }

    return error_occurred;
}

int write_or_register_byte(mraa_i2c_context i2c, uint8_t register_id, uint8_t value)
{
    mraa_result_t status = MRAA_SUCCESS;
    int error_occurred = 0;
    uint8_t reg_val = 0;

    reg_val = mraa_i2c_read_bytes_data(i2c, register_id, &reg_val, 1);
    if (reg_val == -1)
    {
        printf("Error: i2c couldn't read byte");
        error_occurred = 1;
    }
    reg_val |= value;
    if (!error_occurred)
        status = mraa_i2c_write_byte_data(i2c, reg_val, register_id);
    if (!error_occurred && status != MRAA_SUCCESS)
    {
        printf("Error: i2c couldn't write register (id=%x) value: %x\n", register_id, reg_val);
        error_occurred = 1;
    }

    return error_occurred;
}

int read_byte(mraa_i2c_context i2c, uint8_t * byte)
{
	int error_occurred = 0;
	int read_val;

    read_val = mraa_i2c_read_byte(i2c);
    if (read_val == -1)
    {
    	printf("Error: i2c couldn't read byte");
        error_occurred = 1;
    }
    else
    	*byte = read_val;

    return error_occurred;
}

int read_register_bytes(mraa_i2c_context i2c, uint8_t register_id, uint8_t * buffer, int num_bytes)
{
	int error_occurred = 0;
	int read_val;

    read_val = mraa_i2c_read_bytes_data(i2c, register_id, buffer, num_bytes);
    if (read_val == -1)
    {
    	printf("Error: i2c couldn't read byte");
        error_occurred = 1;
    }

    return error_occurred;
}


void debug(int argc, char ** argv)
{
	int bus;

	if (argc == 2)
		bus = strtol(argv[1], NULL, 0);
	else
	{
		printf("\n error no bus number provided\n");
		exit(1);
	}

	mraa_init();

	mraa_i2c_context i2c;
	i2c = mraa_i2c_init(bus);

	i2c_detect_devices(i2c, bus);

	if (i2c == NULL)
	{
		printf("\n Error: bus [%d] did not initialize\n", bus);
	}

	exit(0);
}

static void i2c_detect_devices(mraa_i2c_context i2c, int bus)
{
    //mraa_i2c_context i2c = mraa_i2c_init(bus);
    if (i2c == NULL) {
        return;
    }
    int addr;
    for (addr = 0x0; addr < 0x80; ++addr) {
        uint8_t value;
        if ((addr) % 16 == 0)
            printf("%02x: ", addr);
        if (i2c_get(bus, addr, 0, &value) == MRAA_SUCCESS)
            printf("%02x ", addr);
        else
            printf("-- ");
        if ((addr + 1) % 16 == 0)
            printf("\n");
    }
}

static mraa_result_t i2c_get(int bus, uint8_t device_address, uint8_t register_address, uint8_t* data)
{
    mraa_result_t status = MRAA_SUCCESS;
    mraa_i2c_context i2c = mraa_i2c_init(bus);
    if (i2c == NULL) {
        return MRAA_ERROR_NO_RESOURCES;
    }
    status = mraa_i2c_address(i2c, device_address);
    if (status != MRAA_SUCCESS) {
        goto i2c_get_exit;
    }
    status = mraa_i2c_write_byte(i2c, register_address);
    if (status != MRAA_SUCCESS) {
        goto i2c_get_exit;
    }
    status = mraa_i2c_read(i2c, data, 1) == 1 ? MRAA_SUCCESS : MRAA_ERROR_UNSPECIFIED;
    if (status != MRAA_SUCCESS) {
        goto i2c_get_exit;
    }
i2c_get_exit:
    mraa_i2c_stop(i2c);
    return status;
}

