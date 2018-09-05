#include <stdio.h>

#include "mraa.h"
#include "mraa/i2c.h"

#include "i2c_interface.h"

#define TEST_REG_ERROR -1

static char frequency_str[][32] = {
    "FREQUENCY_100KHZ",
    "FREQUENCY_400KHZ",
    "FREQUENCY_3400KHZ"
};

static int open_bus_number = 0;

static void i2c_detect_devices(mraa_i2c_context i2c, int bus);
static mraa_result_t i2c_get(int bus, uint8_t device_address, uint8_t register_address, uint8_t* data);

mraa_i2c_context i2c_open(int i2c_bus_num)
{
    mraa_i2c_context i2c;

    i2c = mraa_i2c_init(i2c_bus_num);

    if (i2c == NULL)
    {
        printf("\n Error: bus [%d] did not initialize\n", i2c_bus_num);
        return NULL;
    }

    open_bus_number = i2c_bus_num;
    i2c_detect_devices(i2c, i2c_bus_num);
    return i2c;
}

void i2c_close(mraa_i2c_context i2c)
{
    open_bus_number = 0;
    mraa_i2c_stop(i2c);
}

int i2c_set_frequency(mraa_i2c_context i2c, int i2c_frequency)
{
    if (mraa_i2c_frequency(i2c, (mraa_i2c_mode_t)i2c_frequency) != MRAA_SUCCESS)
    {
        printf("Warning: i2c_interface set_frequency failed: %s\n", frequency_str[i2c_frequency - I2C_FREQUENCY_100KHZ]);
        return I2C_FREQUENCY_100KHZ;
    }
    printf("i2c_interface[I2C-%d]: frequency set to %s\n", open_bus_number, frequency_str[i2c_frequency - I2C_FREQUENCY_100KHZ]);
    return i2c_frequency;
}

int i2c_latch_device(mraa_i2c_context i2c, uint8_t device_id)
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

int i2c_dev_read_byte(mraa_i2c_context i2c)
{
    int byte_read = mraa_i2c_read_byte(i2c);
    if (byte_read == -1)
    {
        printf("Error: i2c couldn't read device byte\n");
    }

    return byte_read;
}

int i2c_dev_read_word(mraa_i2c_context i2c)
{
    uint8_t read_buff[2];
    int num_bytes_read;

    num_bytes_read = mraa_i2c_read(i2c, read_buff, 2);
    if (num_bytes_read == -1)
    {
        printf("Error: i2c couldn't read device word\n");
    }

    return num_bytes_read;
}

int i2c_dev_read_bytes(mraa_i2c_context i2c, uint8_t *buffer, int num_bytes)
{
    int num_bytes_read;

    num_bytes_read = mraa_i2c_read(i2c, buffer, num_bytes);
    if (num_bytes_read == -1)
    {
        printf("Error: i2c couldn't read device bytes\n");
    }

    return num_bytes_read;
}

int i2c_dev_write_byte(mraa_i2c_context i2c, uint8_t byte)
{
    int error_occurred = 0;
    int write_status;

    write_status = mraa_i2c_write_byte(i2c, byte);
    if ( write_status != MRAA_SUCCESS)
    {
        printf("Error: i2c couldn't write device byte\n");
    }

    return error_occurred;
}

int i2c_dev_write_bytes(mraa_i2c_context i2c, uint8_t *byte, uint8_t *buffer, int num_bytes)
{
    int write_status;

    write_status = mraa_i2c_write(i2c, buffer, num_bytes);
    if (write_status != MRAA_SUCCESS)
    {
        printf("Error: i2c couldn't write device bytes\n");
    }

    return write_status;
}














int i2c_reg_read_byte(mraa_i2c_context i2c, uint8_t register_id)
{
    int num_bytes_read;

    num_bytes_read = mraa_i2c_read_byte_data(i2c, register_id);
    if (num_bytes_read == -1)
    {
        printf("Error: i2c couldn't read register byte\n");
    }

    return num_bytes_read;
}

int i2c_reg_read_word(mraa_i2c_context i2c, uint8_t register_id)
{
    int num_bytes_read;

    num_bytes_read = mraa_i2c_read_word_data(i2c, register_id);
    if (num_bytes_read == -1)
    {
        printf("Error: i2c couldn't read register word\n");
    }

    return num_bytes_read;
}

int i2c_reg_read_many(mraa_i2c_context i2c, uint8_t first_register_id, uint8_t *buffer, int num_registers)
{
    int error_occurred = 0;
    int num_bytes_read;

    num_bytes_read = mraa_i2c_read_bytes_data(i2c, first_register_id, buffer, num_registers);
    if (num_bytes_read == -1)
    {
        printf("Error: i2c couldn't read bulk registers\n");
        error_occurred = 1;
    }

    return error_occurred;
}

int i2c_reg_write_byte(mraa_i2c_context i2c, uint8_t register_id, uint8_t value)
{
	mraa_result_t status = MRAA_SUCCESS;
	int error_occurred = 0;

    status = mraa_i2c_write_byte_data(i2c, value, register_id);
    if (status != MRAA_SUCCESS)
    {
    	printf("Error: i2c couldn't write register byte (id=%x) value: %x\n", register_id, value);
        error_occurred = 1;
    }

    return error_occurred;
}

int i2c_reg_write_orbyte(mraa_i2c_context i2c, uint8_t register_id, uint8_t value)
{
    mraa_result_t status = MRAA_SUCCESS;
    int error_occurred = 0;
    uint8_t reg_val = 0;

    reg_val = mraa_i2c_read_byte_data(i2c, register_id);
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
        printf("Error: i2c couldn't write register orbyte (id=%x) value: %x\n", register_id, reg_val);
        error_occurred = 1;
    }

    return error_occurred;
}

int i2c_reg_write_word(mraa_i2c_context i2c, uint8_t register_id, uint16_t value)
{
    mraa_result_t status = MRAA_SUCCESS;
    int error_occurred = 0;

    status = mraa_i2c_write_word_data(i2c, value, register_id);
    if (status != MRAA_SUCCESS)
    {
        printf("Error: i2c couldn't write register word (id=%x) value: %x\n", register_id, value);
        error_occurred = 1;
    }

    return error_occurred;
}

void debug(int argc, char **argv)
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

