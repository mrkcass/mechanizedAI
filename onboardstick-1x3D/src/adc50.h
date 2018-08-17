
//ADC50 is a 16 bit analog to digital converter with 4 5 volt inputs. ADC50
//uses the I2C protocol for reading or writing of data and configuration.
//ADC50 inputs A0 - A3 are accessible through the on board stick jack when
//onboard joysticks are not in use.
//based on the Texas Instruments
//ADS 1115.
//


#define ADC50_I2C_BUS_NUM 1
#define ADC50_I2C_ADDRESS 48

//Sample rate - (sps = samples per second)
#define   ADC50_SPS_8    0x0000
#define   ADC50_SPS_16   0x0020
#define   ADC50_SPS_32   0x0040
#define   ADC50_SPS_64   0x0060
#define   ADC50_SPS_128  0x0080
#define   ADC50_SPS_250  0x00A0
#define   ADC50_SPS_475  0x00C0
#define   ADC50_SPS_860  0x00E0

//taken and renamed from https://github.com/intel-iot-devkit/upm/blob/master/src/ads1x15/ads1x15.hpp
/*=========================================================================
    POINTER REGISTER
    -----------------------------------------------------------------------*/
#define ADC50_REG_POINTER_MASK (0x03)
#define ADC50_REG_POINTER_CONVERT (0x00)
#define ADC50_REG_POINTER_CONFIG (0x01)
#define ADC50_REG_POINTER_LOWTHRESH (0x02)
#define ADC50_REG_POINTER_HITHRESH (0x03)
/*=========================================================================*/

/*=========================================================================
    CONFIG REGISTER
    -----------------------------------------------------------------------*/
#define ADC50_OS_MASK (0x8000)
#define ADC50_OS_SINGLE (0x8000)  // Write: Set to start a single-conversion
#define ADC50_OS_BUSY (0x0000)    // Read: Bit = 0 when conversion is in progress
#define ADC50_OS_NOTBUSY (0x8000) // Read: Bit = 1 when device is not performing a conversion

#define ADC50_MUX_MASK (0x7000)
#define ADC50_MUX_DIFF_0_1 (0x0000) // Differential P = AIN0, N = AIN1 (default)
#define ADC50_MUX_DIFF_0_3 (0x1000) // Differential P = AIN0, N = AIN3
#define ADC50_MUX_DIFF_1_3 (0x2000) // Differential P = AIN1, N = AIN3
#define ADC50_MUX_DIFF_2_3 (0x3000) // Differential P = AIN2, N = AIN3
#define ADC50_MUX_SINGLE_0 (0x4000) // Single-ended AIN0
#define ADC50_MUX_SINGLE_1 (0x5000) // Single-ended AIN1
#define ADC50_MUX_SINGLE_2 (0x6000) // Single-ended AIN2
#define ADC50_MUX_SINGLE_3 (0x7000) // Single-ended AIN3

#define ADC50_PGA_MASK (0x0E00)
#define ADC50_PGA_6_144V (0x0000) // +/-6.144V range = Gain 2/3
#define ADC50_PGA_4_096V (0x0200) // +/-4.096V range = Gain 1
#define ADC50_PGA_2_048V (0x0400) // +/-2.048V range = Gain 2 (default)
#define ADC50_PGA_1_024V (0x0600) // +/-1.024V range = Gain 4
#define ADC50_PGA_0_512V (0x0800) // +/-0.512V range = Gain 8
#define ADC50_PGA_0_256V (0x0A00) // +/-0.256V range = Gain 16

#define ADC50_MODE_MASK (0x0100)
#define ADC50_MODE_CONTIN (0x0000) // Continuous conversion mode
#define ADC50_MODE_SINGLE (0x0100) // Power-down single-shot mode (default)

#define ADC50_DR_MASK (0x00E0)

#define ADC50_CMODE_MASK (0x0010)
#define ADC50_CMODE_TRAD (0x0000)   // Traditional comparator with hysteresis (default)
#define ADC50_CMODE_WINDOW (0x0010) // Window comparator
