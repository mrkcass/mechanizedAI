#include <math.h>
#include <unistd.h>
#include <stdio.h>

#include "adc50.h"

#define ADC50_REG_POINTER_CONVERSION   0x00
#define ADC50_REG_POINTER_CONFIG       0x01
#define ADC50_REG_POINTER_THRESH_LO    0x02
#define ADC50_REG_POINTER_THRESH_HI    0x03

//time from issue request to data ready
#define ADC50_CONVERSION_DELAY 5000

struct ADC50_CONFIG_REGISTER
{
   int mux;
   int pga_range;
   int conversion_mode;
   int data_rate;
   int compare_mode;
   int compare_polarity;
   int compare_latch;
   int compare_que;
};

int adc50_init_onboardstick1x3D();
int adc50_read_config_register();
void adc50_write_config_register(int config_bits);
int adc50_testsample();
void adc50_uppack_configbits(struct ADC50_CONFIG_REGISTER *config_struct, int config_bits);
int adc50_pack_config(struct ADC50_CONFIG_REGISTER *config_struct);
void adc50_dump_config(int config);

static struct ADC50_CONFIG_REGISTER adc50_config_register;
static i2c_context i2cdevice;

char ADC50_MUX_REG_STR[][32] = {
   "[0x00] Differential 0-1",
   "[0x01] Differential 0-3",
   "[0x02] Differential 1-3",
   "[0x03] Differential 2-3",
   "[0x04] Single Ended channel 0",
   "[0x05] Single Ended channel 1",
   "[0x06] Single Ended channel 2",
   "[0x07] Single Ended channel 3",
};

char ADC50_PGA_REG_STR[][32] = {
   "[0x00] Range +/- 6.144v",
   "[0x01] Range +/- 4.096v",
   "[0x02] Range +/- 2.048v",
   "[0x03] Range +/- 1.024v",
   "[0x04] Range +/- 0.512v",
   "[0x05] Range +/- 0.256v",
   "[0x06] Range +/- 0.256v",
   "[0x07] Range +/- 0.256v",
};

char ADC50_MODE_REG_STR[][64] = {
   "[0x00] Continuous conversion",
   "[0x01] Single-shot or powered down",
};

char ADC50_DATARATE_REG_STR[][64] = {
   "[0x00] 8 Samples Per Second",
   "[0x01] 16 Samples Per Second",
   "[0x02] 32 Samples Per Second",
   "[0x03] 64 Samples Per Second",
   "[0x04] 128 Samples Per Second",
   "[0x05] 250 Samples Per Second",
   "[0x06] 475 Samples Per Second",
   "[0x07] 860 Samples Per Second",
};

char ADC50_COMPAREMODE_REG_STR[][64] = {
   "[0x00] Traditional",
   "[0x01] Window",
};

char ADC50_COMPAREPOLARITY_REG_STR[][64] = {
   "[0x00] Active low",
   "[0x01] Active high",
};

char ADC50_COMPARELATCH_REG_STR[][64] = {
   "[0x00] Non-latching",
   "[0x01] Latching",
};

char ADC50_COMPAREQUE_REG_STR[][64] = {
   "[0x00] Assert After 1st conversion",
   "[0x01] Assert After 2nd conversion",
   "[0x02] Assert After 4th conversion",
   "[0x03] Comparator Disabled",
};




int adc50_init(int adc50_input)
{
   i2cdevice = i2c_open(ADC50_I2C_BUS_NUM, ADC50_I2C_ADDRESS);

   i2c_set_frequency(i2cdevice, I2C_FREQUENCY_400KHZ);

   int configbits = adc50_read_config_register();

   if (configbits != -1)
   {
     adc50_uppack_configbits(&adc50_config_register, configbits);
     if (adc50_input == ADC50_INPUT_JOY3D)
        adc50_init_onboardstick1x3D();
     int configbits_after = adc50_read_config_register();
     adc50_dump_config(configbits_after);
   }

   return 0;
}

int adc50_init_onboardstick1x3D()
{
   printf("\n\nJOY3D <-- ADC50 Configuration -----------\n");
   adc50_config_register.mux = ADC50_MUX_SINGLE_1;
   adc50_config_register.pga_range = ADC50_PGA_RANGE_6144V;
   adc50_config_register.conversion_mode = ADC50_CONVMODE_CONTINUOUS;
   adc50_config_register.data_rate = ADC50_DATARATE_475;
   adc50_config_register.compare_que = ADC50_COMPARE_OFF;
   adc50_config_register.compare_latch = ADC50_COMPARE_NONLATCHING;
   adc50_config_register.compare_polarity = ADC50_COMPARE_POLARITYACTIVELO;
   adc50_config_register.compare_mode = ADC50_COMPARE_MODETRADITIONAL;
   int configbits = adc50_pack_config(&adc50_config_register);
   adc50_write_config_register(configbits);
   return 0;
}

int adc50_read_config_register()
{
   int regbits = i2c_reg_read_word(i2cdevice, ADC50_REG_POINTER_CONFIG);
   return ((regbits & 0xFF) << 8) | ((regbits >> 8) & 0xFF);
}

void adc50_write_config_register(int config_bits)
{
   uint16_t reg_bits = ((config_bits & 0xFF) << 8) | ((config_bits >> 8) & 0xFF);
   i2c_reg_write_word(i2cdevice, ADC50_REG_POINTER_CONFIG, reg_bits);
}

int adc50_read_conversion_register()
{
   int regbits = i2c_reg_read_word(i2cdevice, ADC50_REG_POINTER_CONVERSION);
   return ((regbits & 0xFF) << 8) | ((regbits >> 8) & 0xFF);
}

int adc50_sample_single_end(int channel)
{
   adc50_config_register.mux = ADC50_MUX_SINGLE_1 + channel;

   int configbits = adc50_pack_config(&adc50_config_register);
   //configbits |= (ADC50_STATUS_START_SINGLE << ADC50_STATUS_BIT0);
   adc50_write_config_register(configbits);
   usleep(ADC50_CONVERSION_DELAY);
   return adc50_read_conversion_register();
}

int adc50_testsample()
{
   printf("Sampling:\n");
   while (1)
   {
     int a0 = adc50_sample_single_end(0);
     int a1 = adc50_sample_single_end(1);
     int a2 = adc50_sample_single_end(2);

     printf("   CHANNEL 0: %6d   CHANNEL 1: %6d  CHANNEL 2: %6d\r", a0, a1, a2);
     fflush(stdout);
     usleep(1000000/16);
   }
   return 0;
}

void adc50_uppack_configbits(struct ADC50_CONFIG_REGISTER * config_struct, int config_bits)
{
   config_struct->mux = (config_bits >> ADC50_MUX_BIT0) & ADC50_MUX_BIT0MASK;
   config_struct->pga_range = (config_bits >> ADC50_PGA_BIT0) & ADC50_PGA_BIT0MASK;
   config_struct->conversion_mode = (config_bits >> ADC50_MODE_BIT0) & ADC50_MODE_BIT0MASK;
   config_struct->data_rate = (config_bits >> ADC50_DATARATE_BIT0) & ADC50_DATARATE_BIT0MASK;
   config_struct->compare_mode = (config_bits >> ADC50_COMPAREMODE_BIT0) & ADC50_COMPAREMODE_BIT0MASK;
   config_struct->compare_polarity = (config_bits >> ADC50_COMPAREPOLARITY_BIT0) & ADC50_COMPAREPOLARITY_BIT0MASK;
   config_struct->compare_latch = (config_bits >> ADC50_COMPARELATCH_BIT0) & ADC50_COMPARELATCH_BIT0MASK;
   config_struct->compare_que = (config_bits >> ADC50_COMPAREQUE_BIT0) & ADC50_COMPAREQUE_BIT0MASK;
}

int adc50_pack_config(struct ADC50_CONFIG_REGISTER *config_struct)
{
   int packed = 0;

   packed |= (config_struct->mux & ADC50_STATUS_BIT0MASK) << ADC50_STATUS_BIT0;
   packed |= (config_struct->mux & ADC50_MUX_BIT0MASK) << ADC50_MUX_BIT0;
   packed |= (config_struct->pga_range & ADC50_PGA_BIT0MASK) << ADC50_PGA_BIT0;
   packed |= (config_struct->conversion_mode & ADC50_MODE_BIT0MASK) << ADC50_MODE_BIT0;
   packed |= (config_struct->data_rate & ADC50_DATARATE_BIT0MASK) << ADC50_DATARATE_BIT0;
   packed |= (config_struct->compare_mode & ADC50_COMPAREMODE_BIT0MASK) << ADC50_COMPAREMODE_BIT0;
   packed |= (config_struct->compare_polarity & ADC50_COMPAREPOLARITY_BIT0MASK) << ADC50_COMPAREPOLARITY_BIT0;
   packed |= (config_struct->compare_latch & ADC50_COMPARELATCH_BIT0MASK) << ADC50_COMPARELATCH_BIT0;
   packed |= (config_struct->compare_latch & ADC50_COMPAREQUE_BIT0MASK) << ADC50_COMPAREQUE_BIT0;

   return packed;
}

void adc50_dump_config(int config)
{
   int bits0 = (config >> ADC50_MUX_BIT0) & ADC50_MUX_BIT0MASK;
   printf("  MUX      : %s\n", ADC50_MUX_REG_STR[bits0]);

   bits0 = (config >> ADC50_PGA_BIT0) & ADC50_PGA_BIT0MASK;
   printf("  PGA      : %s\n", ADC50_PGA_REG_STR[bits0]);

   bits0 = (config >> ADC50_MODE_BIT0) & ADC50_MODE_BIT0MASK;
   printf("  Mode     : %s\n", ADC50_MODE_REG_STR[bits0]);

   bits0 = (config >> ADC50_DATARATE_BIT0) & ADC50_DATARATE_BIT0MASK;
   printf("  Data Rate: %s\n", ADC50_DATARATE_REG_STR[bits0]);

   printf("  Comparator:\n");
   bits0 = (config >> ADC50_COMPAREMODE_BIT0) & ADC50_COMPAREMODE_BIT0MASK;
   printf("    Mode    : %s\n", ADC50_COMPAREMODE_REG_STR[bits0]);
   bits0 = (config >> ADC50_COMPAREPOLARITY_BIT0) & ADC50_COMPAREPOLARITY_BIT0MASK;
   printf("    Polarity: %s\n", ADC50_COMPAREPOLARITY_REG_STR[bits0]);
   bits0 = (config >> ADC50_COMPARELATCH_BIT0) & ADC50_COMPARELATCH_BIT0MASK;
   printf("    Latching: %s\n", ADC50_COMPARELATCH_REG_STR[bits0]);
   bits0 = (config >> ADC50_COMPAREQUE_BIT0) & ADC50_COMPAREQUE_BIT0MASK;
   printf("    Queueing: %s\n", ADC50_COMPAREQUE_REG_STR[bits0]);
}

