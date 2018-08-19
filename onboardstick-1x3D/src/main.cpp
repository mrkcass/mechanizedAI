#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/select.h>
#include <termios.h>
#include <string.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "joystick.h"
#include "adc50.h"

//#define KEYPRESS_ENABLED 1

struct termios orig_termios;

#define JSTICK1_ANALOG_IN_X 1
#define JSTICK1_ANALOG_IN_Y 0
#define JSTICK1_DIG_IN_BTN  4
#define JSTICK2_ANALOG_IN_X 2
#define JSTICK2_ANALOG_IN_Y 3
#define JSTICK2_DIG_IN_BTN  5

#define NUM_POWER_LEVELS 5

static char RUN_STR[11] =  "run   \n";
static char STOP_STR[11] = "stop  \n";

char xpowers[][11] = {"pwrxn5\n", "pwrxn4\n",  "pwrxn3\n", "pwrxn2\n", "pwrxn1\n",
					  "pwrx00\n",
					  "pwrxp1\n", "pwrxp2\n", "pwrxp3\n", "pwrxp4\n", "pwrxp5\n" };

char ypowers[][11] = {"pwryp5\n", "pwryn4\n", "pwryn3\n", "pwryn2\n", "pwryn1\n",
					 "pwry00\n",
					 "pwryp1\n", "pwryp2\n", "pwryp3\n", "pwryp4\n", "pwryp5\n" };

char zpowers[][11] = {"pwrzn5\n", "pwrzn4\n", "pwrzn3\n", "pwrzn2\n", "pwrzn1\n",
					 "pwrz00\n",
					 "pwrzp1\n", "pwrzp2\n", "pwrzp3\n", "pwrzp4\n", "pwrzp5\n" };

#ifdef KEYPRESS_ENABLED
void reset_terminal_mode();
void set_conio_terminal_mode();
int kbhit();
int getch();
#endif
int send_to_mcu(char * msg);
int run(Joystick * jstick_left, Joystick * jstick_right);
int joy3d_testsample(mraa_i2c_context i2c1);

int main(int argc, char * argv[])
{
	Joystick jstick_left, jstick_right;

	mraa_init();

	mraa_i2c_context i2c = NULL;

	i2c = i2c_open(ADC50_I2C_BUS_NUM);
	adc50_init(i2c, ADC50_INPUT_JOY3D);
	joy3d_testsample(i2c);

	// jstick_left.connect(JSTICK1_ANALOG_IN_X, JSTICK1_ANALOG_IN_Y, JSTICK1_DIG_IN_BTN, false);
	// jstick_right.connect(JSTICK2_ANALOG_IN_X, JSTICK2_ANALOG_IN_Y, JSTICK2_DIG_IN_BTN, false);
	// usleep(100 * 1000);
	// printf("calibrate left stick................\n");
	// jstick_left.calibrate(NUM_POWER_LEVELS);
	// printf("calibrate right stick................\n");
	// jstick_right.calibrate(NUM_POWER_LEVELS);
	// run(&jstick_left, &jstick_right);


	int had_error = -1;
	if (i2c)
	{
		had_error = 0;
		i2c_close(i2c);
	}
	mraa_deinit();
	return had_error;
}

int joy3d_testsample(mraa_i2c_context i2c1)
{
	printf("Sampling JOY3D:\n");
	while (1)
	{
		int a0 = adc50_sample_single_end(i2c1, 0);
		int a1 = adc50_sample_single_end(i2c1, 1);
		int a2 = adc50_sample_single_end(i2c1, 2);

		printf("   JOYPAN[0]: %6d   JOYTILT[2]: %6d  JOYROTATE[1]: %6d\r", a0, a2, a1);
		fflush(stdout);
		usleep(1000000 / 20);
	}
	return 0;
}

int run(Joystick * jstick_left, Joystick * jstick_right)
{
	int x_power, y_power, z_power;
	int x_last_power, y_last_power, z_last_power;
	int MILLISECONDS_DELAY = 50 * 1000;

	send_to_mcu(RUN_STR);

	x_last_power = y_last_power = 0;
	while (1)
	{
		x_power = jstick_left->read(JOYSTICK_SRC_AXIS_X);
		y_power = jstick_left->read(JOYSTICK_SRC_AXIS_Y);
		z_power = jstick_right->read(JOYSTICK_SRC_AXIS_X);

		if (x_power != x_last_power)
		{
			printf("send x %d\n", x_power);
			send_to_mcu(xpowers[x_power + NUM_POWER_LEVELS]);
		}
		if (y_power != y_last_power)
		{
			printf("send y %d\n", y_power);
			send_to_mcu(ypowers[y_power + NUM_POWER_LEVELS]);
		}
		if (z_power != z_last_power)
		{
			printf("send z %d\n", z_power);
			send_to_mcu(zpowers[z_power + NUM_POWER_LEVELS]);
		}
		x_last_power = x_power;
		y_last_power = y_power;
		z_last_power = z_power;
		usleep(MILLISECONDS_DELAY);
	}

	send_to_mcu(STOP_STR);
}

int send_to_mcu(char * msg)
{
	int msg_len = strlen(msg);
	int mcu_fd = open("/dev/ttymcu0", O_RDWR | O_NOCTTY);
	if (mcu_fd == -1)
	{
		printf("open ttymcu1 failed!\n");
		return 1;
	}
	else
	{
		write(mcu_fd, msg, msg_len);
		close(mcu_fd);
	}
	return 0;
}

#ifdef KEYPRESS_ENABLED
void reset_terminal_mode()
{
	tcsetattr(0, TCSANOW, &orig_termios);
}

void set_conio_terminal_mode()
{
	struct termios new_termios;

	/* take two copies - one for now, one for later */
	tcgetattr(0, &orig_termios);
	memcpy(&new_termios, &orig_termios, sizeof(new_termios));

	/* register cleanup handler, and set the new terminal mode */
	atexit(reset_terminal_mode);
	cfmakeraw(&new_termios);
	tcsetattr(0, TCSANOW, &new_termios);
}

int kbhit()
{
	struct timeval tv = { 0L, 0L };
	fd_set fds;
	FD_ZERO(&fds);
	FD_SET(0, &fds);
	return select(1, &fds, NULL, NULL, &tv);
}

int getch()
{
	int r;
	unsigned char c;
	if ((r = read(0, &c, sizeof(c))) < 0) {
		return r;
	}
	else {
		return c;
	}
}
#endif