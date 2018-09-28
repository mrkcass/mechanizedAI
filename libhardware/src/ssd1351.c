#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <mraa/gpio.h>
#include <mraa/spi.h>
#include <sys/time.h>

// Display Size
#define SSD1351WIDTH 128
#define SSD1351HEIGHT 128  // Set this to 96 for 1.27"

// SSD1351 Commands
#define SSD1351_CMD_SETCOLUMN       0x15
#define SSD1351_CMD_SETROW          0x75
#define SSD1351_CMD_WRITERAM        0x5C
#define SSD1351_CMD_READRAM         0x5D
#define SSD1351_CMD_SETREMAP        0xA0
#define SSD1351_CMD_STARTLINE       0xA1
#define SSD1351_CMD_DISPLAYOFFSET   0xA2
#define SSD1351_CMD_DISPLAYALLOFF   0xA4
#define SSD1351_CMD_DISPLAYALLON    0xA5
#define SSD1351_CMD_NORMALDISPLAY   0xA6
#define SSD1351_CMD_INVERTDISPLAY   0xA7
#define SSD1351_CMD_FUNCTIONSELECT  0xAB
#define SSD1351_CMD_DISPLAYOFF      0xAE
#define SSD1351_CMD_DISPLAYON       0xAF
#define SSD1351_CMD_PRECHARGE       0xB1
#define SSD1351_CMD_DISPLAYENHANCE  0xB2
#define SSD1351_CMD_CLOCKDIV        0xB3
#define SSD1351_CMD_SETVSL          0xB4
#define SSD1351_CMD_SETGPIO         0xB5
#define SSD1351_CMD_PRECHARGE2      0xB6
#define SSD1351_CMD_SETGRAY         0xB8
#define SSD1351_CMD_USELUT          0xB9
#define SSD1351_CMD_PRECHARGELEVEL  0xBB
#define SSD1351_CMD_VCOMH           0xBE
#define SSD1351_CMD_CONTRASTABC     0xC1
#define SSD1351_CMD_CONTRASTMASTER  0xC7
#define SSD1351_CMD_MUXRATIO        0xCA
#define SSD1351_CMD_COMMANDLOCK     0xFD
#define SSD1351_CMD_HORIZSCROLL     0x96
#define SSD1351_CMD_STOPSCROLL      0x9E
#define SSD1351_CMD_STARTSCROLL     0x9F

#define HIGH                1
#define LOW                 0

#define SPI_FREQUENCY_MHZ 25

// Number of blocks for SPI transfer of buffer
#define BLOCKS              8

struct Ssd1351Gpio
{
    mraa_gpio_context reset;
    mraa_gpio_context data_cmd;
    mraa_gpio_context slave_select;
};

mraa_spi_context init_spi_bus();
bool init_gpio(Ssd1351Gpio * ssd1351_gpio);
bool init_ssd1351(mraa_spi_context spi, Ssd1351Gpio ssd1351_gpio);
void send_spi_cmd(mraa_spi_context spi, struct Ssd1351Gpio gpio, uint8_t value);
void send_spi_data(mraa_spi_context spi, struct Ssd1351Gpio gpio,uint8_t value);
int set_gpio(struct Ssd1351Gpio gpio, const char * pin_name, int state);
char * decode_error(mraa_result_t code);
void refresh(mraa_spi_context spi, struct Ssd1351Gpio gpio, uint8_t * framebuffer);
void draw_pixel(mraa_spi_context spi, struct Ssd1351Gpio gpio, uint8_t * framebuffer, int16_t x, int16_t y, uint16_t color);
void draw_hline(mraa_spi_context spi, struct Ssd1351Gpio gpio, int16_t x, int16_t y, uint16_t color, int16_t length);
void fill_rect(mraa_spi_context spi, struct Ssd1351Gpio gpio, uint8_t * framebuffer, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color);
void fill_screen(mraa_spi_context spi, struct Ssd1351Gpio gpio, uint8_t * framebuffer, uint16_t color);

int main(int argc, char ** argv)
{
    printf("\nStarting Driver: SSD1351\n");

    struct Ssd1351Gpio ssd1351_gpio;
    uint8_t framebuffer[SSD1351HEIGHT * SSD1351WIDTH * 2];

    printf ("\nInitalizing SPI..\n");
    mraa_spi_context spi = init_spi_bus();
    if (!spi)
        return -1;

    bool init_result;
    init_result = init_gpio(&ssd1351_gpio);
    if (!init_result)
        return -1;

    printf ("\nReseting controller..\n");
    set_gpio(ssd1351_gpio, "rst", 1);
    usleep(500000);
    set_gpio(ssd1351_gpio, "rst", 0);
    usleep(500000);
    set_gpio(ssd1351_gpio, "rst", 1);
    usleep(500000);

    init_result = init_ssd1351(spi, ssd1351_gpio);
    if (!init_result)
        return -1;

    int fillidx;
    for (fillidx = 0; fillidx < SSD1351WIDTH * (SSD1351HEIGHT / 2); fillidx++)
        framebuffer[fillidx] = 0xff;
    printf("\nEntering refresh loop\n");
    int color = 100;
    int fcreset = 5;
    int fc = fcreset;
    uint64_t totaltime = 0;
    while (1)
    {
        struct timespec start, end;
        clock_gettime(CLOCK_MONOTONIC_RAW, &start);
        fill_screen(spi, ssd1351_gpio, framebuffer, color);
        clock_gettime(CLOCK_MONOTONIC_RAW, &end);
        uint64_t delta_us = (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_nsec - start.tv_nsec) / 1000;
        totaltime += delta_us;
        fc--;
        if (!fc)
        {
            float upf = (float)totaltime / (float)fcreset;
            float fps = 1000000.0 / upf;
            float bps = fps * (float)(SSD1351WIDTH*SSD1351HEIGHT*2);
            printf("Frame Rate: %3.2f fps\t %5.2f KBps\n", fps, bps / 1000.0);
            totaltime = 0;
            fc = fcreset;
        }
        color+=100;
        usleep(300000);
    }

    return 0;
}

mraa_spi_context init_spi_bus()
{
    mraa_result_t result;

    printf ("\nInitalizing SPI..\n");
    mraa_spi_context spi = mraa_spi_init(1);
    printf("  SPI: MRAA context = %p\n", spi);
    int freq = SPI_FREQUENCY_MHZ * 1000000;
    freq /= 1;
    result = mraa_spi_frequency(spi, freq);
    if (result == MRAA_SUCCESS)
        printf("  SPI: frequency = %d\n", freq);
    else
        printf("  SPI ERROR: Could not set frequency = %d. [%d]%s\n", freq, result, decode_error(result));
    result = mraa_spi_mode(spi, MRAA_SPI_MODE0 );
    if (result == MRAA_SUCCESS)
        printf("  SPI: mode = 0\n");
    else
        printf("  SPI ERROR: Could not set mode = 0. [%d]%s\n", result, decode_error(result));
    //initialize the bus
    mraa_spi_write(spi, 0x00);

    result = mraa_spi_lsbmode(spi, 0);
    if (result == MRAA_SUCCESS)
        printf("  SPI: lsbmode = 0\n");
    else
        printf("  SPI ERROR: Could not set lsbmode = 0. [%d]%s\n", result, decode_error(result));
    result = mraa_spi_bit_per_word(spi,8);
    if (result == MRAA_SUCCESS)
        printf("  SPI: bits per word = 8\n");
    else
        printf("  SPI ERROR: Could not set bits per word = 8. [%d]%s\n", result, decode_error(result));

    return spi;
}

bool init_gpio(Ssd1351Gpio * ssd1351_gpio)
{
    mraa_result_t result;
    printf ("\nInitalizing GPIO..\n");
    ssd1351_gpio->reset = mraa_gpio_init_raw(130);
    printf ("  SPI[RST]: %p\n", ssd1351_gpio->reset);
    mraa_gpio_use_mmaped(ssd1351_gpio->reset, 1);
    result = mraa_gpio_dir(ssd1351_gpio->reset, MRAA_GPIO_OUT);
    if (result == MRAA_SUCCESS)
        result = mraa_gpio_mode(ssd1351_gpio->reset, MRAA_GPIO_PULLDOWN);
    if (result == MRAA_SUCCESS)
        printf("  SPI[RST]: set direction out\n");
    else
    {
        printf("  SPI[RST] ERROR: Could not set direction = out. [%d]%s\n", result, decode_error(result));
        return false;
    }

    ssd1351_gpio->data_cmd = mraa_gpio_init_raw(128);
    printf ("  SPI[DC]: %p\n", ssd1351_gpio->data_cmd);
    mraa_gpio_use_mmaped(ssd1351_gpio->data_cmd, 1);
    result = mraa_gpio_dir(ssd1351_gpio->data_cmd, MRAA_GPIO_OUT);
    if (result == MRAA_SUCCESS)
        result = mraa_gpio_mode(ssd1351_gpio->data_cmd, MRAA_GPIO_PULLDOWN);
    if (result == MRAA_SUCCESS)
        printf("  SPI[DC]: set direction out\n");
    else
    {
        printf("  SPI[DC] ERROR: Could not set direction = out. [%d]%s\n", result, decode_error(result));
        return false;
    }

    return true;
}

bool init_ssd1351(mraa_spi_context spi, Ssd1351Gpio ssd1351_gpio)
{
    printf ("\nConfiguring display: %d x %d\n", SSD1351WIDTH, SSD1351HEIGHT);

    // Configure and init display
    send_spi_cmd(spi, ssd1351_gpio, SSD1351_CMD_COMMANDLOCK);
    send_spi_data(spi, ssd1351_gpio, 0x12);

    send_spi_cmd(spi, ssd1351_gpio, SSD1351_CMD_COMMANDLOCK);
    send_spi_data(spi, ssd1351_gpio, 0xB1);

    send_spi_cmd(spi, ssd1351_gpio, SSD1351_CMD_DISPLAYOFF);

    send_spi_cmd(spi, ssd1351_gpio, SSD1351_CMD_SETCOLUMN);
    send_spi_data(spi, ssd1351_gpio, 0x00);
    send_spi_data(spi, ssd1351_gpio, 0x7F);

    send_spi_cmd(spi, ssd1351_gpio, SSD1351_CMD_SETROW);
    send_spi_data(spi, ssd1351_gpio, 0x00);
    send_spi_data(spi, ssd1351_gpio, 0x7F);

    send_spi_cmd(spi, ssd1351_gpio, SSD1351_CMD_CLOCKDIV);
    send_spi_cmd(spi, ssd1351_gpio, 0xF1);

    send_spi_cmd(spi, ssd1351_gpio, SSD1351_CMD_MUXRATIO);
    send_spi_data(spi, ssd1351_gpio, 0x7F);

    send_spi_cmd(spi, ssd1351_gpio, SSD1351_CMD_SETREMAP);
    send_spi_data(spi, ssd1351_gpio, 0x74);

    send_spi_cmd(spi, ssd1351_gpio, SSD1351_CMD_STARTLINE);
    if (SSD1351HEIGHT == 96) {
        send_spi_data(spi, ssd1351_gpio, 96);
    } else {
        send_spi_data(spi, ssd1351_gpio, 0);
    }

    send_spi_cmd(spi, ssd1351_gpio, SSD1351_CMD_DISPLAYOFFSET);
    send_spi_data(spi, ssd1351_gpio, 0x0);

    send_spi_cmd(spi, ssd1351_gpio, SSD1351_CMD_SETGPIO);
    send_spi_data(spi, ssd1351_gpio, 0x00);

    send_spi_cmd(spi, ssd1351_gpio, SSD1351_CMD_FUNCTIONSELECT);
    send_spi_data(spi, ssd1351_gpio, 0x01);

    send_spi_cmd(spi, ssd1351_gpio, SSD1351_CMD_SETVSL );
    send_spi_data(spi, ssd1351_gpio, 0xA0);
    send_spi_data(spi, ssd1351_gpio, 0xB5);
    send_spi_data(spi, ssd1351_gpio, 0x55);

    send_spi_cmd(spi, ssd1351_gpio, SSD1351_CMD_CONTRASTABC);
    send_spi_data(spi, ssd1351_gpio, 0xC8);
    send_spi_data(spi, ssd1351_gpio, 0x80);
    send_spi_data(spi, ssd1351_gpio, 0xC8);

    send_spi_cmd(spi, ssd1351_gpio, SSD1351_CMD_CONTRASTMASTER);
    send_spi_data(spi, ssd1351_gpio, 0x0F);

    send_spi_cmd(spi, ssd1351_gpio, SSD1351_CMD_PRECHARGE);
    send_spi_cmd(spi, ssd1351_gpio, 0x32);

    send_spi_cmd(spi, ssd1351_gpio, SSD1351_CMD_DISPLAYENHANCE);
    send_spi_data(spi, ssd1351_gpio, 0xA4);
    send_spi_data(spi, ssd1351_gpio, 0x00);
    send_spi_data(spi, ssd1351_gpio, 0x00);

    send_spi_cmd(spi, ssd1351_gpio, SSD1351_CMD_PRECHARGE2);
    send_spi_cmd(spi, ssd1351_gpio, 0x01);

    send_spi_cmd(spi, ssd1351_gpio, SSD1351_CMD_VCOMH);
    send_spi_cmd(spi, ssd1351_gpio, 0x05);

    send_spi_cmd(spi, ssd1351_gpio, SSD1351_CMD_NORMALDISPLAY);

    send_spi_cmd(spi, ssd1351_gpio, SSD1351_CMD_DISPLAYON);

    return true;
}

void send_spi_cmd(mraa_spi_context spi, struct Ssd1351Gpio gpio, uint8_t value)
{
    set_gpio(gpio, "dc", 0);
    int status = mraa_spi_write(spi, value);
    if (status < 0)
        printf("  SB1351 Error: Send command failed [%x]\n", value);
    set_gpio(gpio, "dc", 1);
}

void send_spi_data(mraa_spi_context spi, struct Ssd1351Gpio gpio, uint8_t value)
{
    //set_gpio(gpio, "cs", 0);
    set_gpio(gpio, "dc", 1);
    int status = mraa_spi_write(spi, value);
    if (status < 0)
        printf("  SB1351 Error: Send data failed [%x]\n", value);
    set_gpio(gpio, "dc", 0);
}

void refresh(mraa_spi_context spi, struct Ssd1351Gpio gpio, uint8_t * framebuffer)
{
    send_spi_cmd(spi, gpio, SSD1351_CMD_WRITERAM);
    int blockSize = SSD1351HEIGHT * SSD1351WIDTH * 2 / BLOCKS;
    set_gpio(gpio, "cs", 0);
    set_gpio(gpio, "dc", 1);
    int block;
    for (block = 0; block < BLOCKS; block++)
    {
        mraa_spi_write_buf(spi, &framebuffer[block * blockSize], blockSize);
    }
    set_gpio(gpio, "cs", 1);
}

void draw_pixel(mraa_spi_context spi, struct Ssd1351Gpio gpio, uint8_t * framebuffer, int16_t x, int16_t y, uint16_t color)
{
    bool m_usemap = false;

    if ((x < 0) || (y < 0) || (x >= SSD1351WIDTH) || (y >= SSD1351HEIGHT))
        return;

    if(m_usemap)
    {
        int index = (y * SSD1351WIDTH + x) * 2;
        framebuffer[index] = color >> 8;
        framebuffer[index + 1] = color;
    } else
    {
        send_spi_cmd(spi, gpio, SSD1351_CMD_SETCOLUMN);
        send_spi_data(spi, gpio, x);
        send_spi_data(spi, gpio, x);

        send_spi_cmd(spi, gpio, SSD1351_CMD_SETROW);
        send_spi_data(spi, gpio, y);
        send_spi_data(spi, gpio, y);

        send_spi_cmd(spi, gpio, SSD1351_CMD_WRITERAM);
        send_spi_data(spi, gpio, color >> 8);
        send_spi_data(spi, gpio, color & 0xFF);
    }
}

void draw_hline(mraa_spi_context spi, struct Ssd1351Gpio gpio, int16_t x, int16_t y, uint16_t color, int16_t length)
{
    // Bounds check
    if ((x >= SSD1351WIDTH) || (y >= SSD1351HEIGHT))
        return;

    // X bounds check
    if (x+length > SSD1351WIDTH)
    {
        length = SSD1351WIDTH - x - 1;
    }

    if (length < 0)
        return;

    uint16_t linebuf[SSD1351WIDTH];
    for (uint16_t i=0; i < length; i++)
        linebuf[i] = color;

    // set location
    send_spi_cmd(spi, gpio, SSD1351_CMD_SETCOLUMN);
    send_spi_data(spi, gpio, x);
    send_spi_data(spi, gpio, x+length-1);
    send_spi_cmd(spi, gpio, SSD1351_CMD_SETROW);
    send_spi_data(spi, gpio, y);
    send_spi_data(spi, gpio, y);
    // fill!
    send_spi_cmd(spi, gpio, SSD1351_CMD_WRITERAM);

    mraa_spi_transfer_buf(spi, (uint8_t*)linebuf, 0, length*2);
}

void fill_screen(mraa_spi_context spi, struct Ssd1351Gpio gpio, uint8_t * framebuffer, uint16_t color)
{
     for (uint16_t i=0; i < SSD1351WIDTH*SSD1351HEIGHT; i++)
        ((uint16_t*)framebuffer)[i] = color;
    // set location
    send_spi_cmd(spi, gpio, SSD1351_CMD_SETCOLUMN);
    send_spi_data(spi, gpio, 0);
    send_spi_data(spi, gpio, SSD1351WIDTH-1);
    send_spi_cmd(spi, gpio, SSD1351_CMD_SETROW);
    send_spi_data(spi, gpio, 0);
    send_spi_data(spi, gpio, SSD1351HEIGHT-1);
    // fill!
    send_spi_cmd(spi, gpio, SSD1351_CMD_WRITERAM);

    int max_tx_lines = 4;
    for (uint16_t i=0; i < SSD1351HEIGHT / max_tx_lines; i++)
        mraa_spi_transfer_buf(spi, &framebuffer[SSD1351WIDTH*i*max_tx_lines*2], 0, SSD1351WIDTH*max_tx_lines*2);
}

void fill_rect(mraa_spi_context spi, struct Ssd1351Gpio gpio, uint8_t * framebuffer, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color)
{
    uint16_t i,j;

    for(j = y1; j <= y2; j++)
        for(i = x1; i <= x2; i++)
            ((uint16_t*)framebuffer)[(j*SSD1351WIDTH)+i] = color;

    send_spi_cmd(spi, gpio, SSD1351_CMD_SETCOLUMN);
    send_spi_data(spi, gpio, x1);
    send_spi_data(spi, gpio, x2);

    send_spi_cmd(spi, gpio, SSD1351_CMD_SETROW);
    send_spi_data(spi, gpio, y1);
    send_spi_data(spi, gpio, y2);
    send_spi_cmd(spi, gpio, SSD1351_CMD_WRITERAM);

    for (uint16_t i=y1; i < y2; i++)
        mraa_spi_transfer_buf(spi, &framebuffer[x1*i*2], 0, (x2-x1)*2);
}

int set_gpio(struct Ssd1351Gpio gpio, const char * pin_name, int state)
{
    mraa_gpio_context pin = 0;
    if (!strcmp(pin_name, "dc"))
        pin = gpio.data_cmd;
    else if (!strcmp(pin_name, "rst"))
        pin = gpio.reset;
    // else if (!strcmp(pin_name, "cs"))
    //     pin = gpio.slave_select;
    else
        printf("  SB351 Error: Unknown set_gpio [%s]", pin_name);

    mraa_result_t wstatus;
    wstatus = mraa_gpio_write (pin, state);
    if (wstatus != MRAA_SUCCESS)
    {
        printf("  SB1351 Error: gpio '%s' write failed with code [%d]%s\n", pin_name, wstatus, decode_error(wstatus));
        return -1;
    }
    return 0;
}

char * decode_error(mraa_result_t code)
{
    switch (code)
    {
        case MRAA_SUCCESS:
        {
            static char decode01[] = "Expected response";
            return decode01;
        }
        case MRAA_ERROR_FEATURE_NOT_IMPLEMENTED:
        {
            static char decode02[] = "Feature TODO";
            return decode02;
        }
        case MRAA_ERROR_FEATURE_NOT_SUPPORTED:
        {
            static char decode03[] = "Feature not";
            return decode03;
        }
        case MRAA_ERROR_INVALID_VERBOSITY_LEVEL:
        {
            static char decode04[] = "Verbosity level";
            return decode04;
        }
        case MRAA_ERROR_INVALID_PARAMETER:
        {
            static char decode05[] = "Parameter invalid";
            return decode05;
        }
        case MRAA_ERROR_INVALID_HANDLE:
        {
            static char decode06[] = "Handle invalid";
            return decode06;
        }
        case MRAA_ERROR_NO_RESOURCES:
        {
            static char decode07[] = "No resource";
            return decode07;
        }
        case MRAA_ERROR_INVALID_RESOURCE:
        {
            static char decode08[] = "Resource invalid";
            return decode08;
        }
        case MRAA_ERROR_INVALID_QUEUE_TYPE:
        {
            static char decode09[] = "Queue type";
            return decode09;
        }
        case MRAA_ERROR_NO_DATA_AVAILABLE:
        {
            static char decode10[] = "No data";
            return decode10;
        }
        case MRAA_ERROR_INVALID_PLATFORM:
        {
            static char decode11[] = "Platform not";
            return decode11;
        }
        case MRAA_ERROR_PLATFORM_NOT_INITIALISED:
        {
            static char decode12[] = "Board information";
            return decode12;
        }
        case MRAA_ERROR_UART_OW_SHORTED:
        {
            static char decode12[] = "UART OW";
            return decode12;
        }
        case MRAA_ERROR_UART_OW_NO_DEVICES:
        {
            static char decode13[] = "UART OW";
            return decode13;
        }
        case MRAA_ERROR_UART_OW_DATA_ERROR:
        {
            static char decode14[] = "UART OW";
            return decode14;
        }
        case MRAA_ERROR_UNSPECIFIED:
        {
            static char decode15[] = "Uknown Error";
            return decode15;
        }
    }
    return 0;
}
