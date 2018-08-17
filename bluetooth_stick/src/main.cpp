/*
* Hidraw Userspace Example
*
* Copyright (c) 2010 Alan Ott <alan@signal11.us>
* Copyright (c) 2010 Signal 11 Software
*
* The code may be used by anyone for any purpose,
* and can serve as a starting point for developing
* applications using hidraw.
*/

/* Linux */
#include <linux/types.h>
#include <linux/input.h>
#include <linux/hidraw.h>

/*
* Ugly hack to work around failing compilation on systems that don't
* yet populate new version of hidraw.h to userspace.
*/
#ifndef HIDIOCSFEATURE
#warning Please have your distro update the userspace kernel headers
#define HIDIOCSFEATURE(len)    _IOC(_IOC_WRITE|_IOC_READ, 'H', 0x06, len)
#define HIDIOCGFEATURE(len)    _IOC(_IOC_WRITE|_IOC_READ, 'H', 0x07, len)
#endif

/* Unix */
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

/* C */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

struct report {
	unsigned char id;
	unsigned char junk[3];
	unsigned char key;
	unsigned char effs[5];
	unsigned short press;
};

#define THUMB_LEFT_X			1
#define THUMB_LEFT_Y			2
#define THUMB_RIGHT_X			3
#define THUMB_RIGHT_Y			4
#define DPAD_LEFT				5
#define DPAD_UP					5
#define DPAD_RIGHT				5
#define DPAD_DOWN				5
#define TRIGGER_LEFT			7
#define TRIGGER_RIGHT			8
#define BUTTON_A				5
#define BUTTON_B				5
#define BUTTON_X				5
#define BUTTON_Y				5
#define BUTTON_LEFT_SHOULDER	6
#define BUTTON_RIGHT_SHOULDER	6
#define BUTTON_LEFT_THUMB		6
#define BUTTON_RIGHT_THUMB		6

#define THUMB_X_MIN			0x00
#define THUMB_X_CENTER		0x80
#define THUMB_X_MAX			0xff

#define THUMB_Y_MIN			0x00
#define THUMB_Y_CENTER		0x7f
#define THUMB_Y_MAX			0xff

#define DPAD_ACTIVE_NONE	0x0f
#define DPAD_ACTIVE_LEFT	0x06
#define DPAD_ACTIVE_UP		0x00
#define DPAD_ACTIVE_RIGHT	0x02
#define DPAD_ACTIVE_DOWN	0x04

#define BUTTON_ACTIVE_A		0x10
#define BUTTON_ACTIVE_B		0x20
#define BUTTON_ACTIVE_X		0x40
#define BUTTON_ACTIVE_Y		0x80

#define TRIGGER_MIN			0x00

#define TRIGGER_MAX			0xff

#define BUTTON_ACTIVE_LEFT_SHOULDER		0x01
#define BUTTON_ACTIVE_RIGHT_SHOULDER	0x02

#define BUTTON_ACTIVE_LEFT_THUMB		0x10
#define BUTTON_ACTIVE_RIGHT_THUMB		0x20

#define NUM_POWER_LEVELS 5

static char RUN_STR[11] =  "run   \n";
static char STOP_STR[11] = "stop  \n";

char xpowers[][11] = {"pwrxn5\n", "pwrxn4\n",  "pwrxn3\n", "pwrxn2\n", "pwrxn1\n",
					  "pwrx00\n",
					  "pwrxp1\n", "pwrxp2\n", "pwrxp3\n", "pwrxp4\n", "pwrxp5\n" };

char ypowers[][11] = {"pwryn5\n", "pwryn4\n", "pwryn3\n", "pwryn2\n", "pwryn1\n",
					 "pwry00\n",
					 "pwryp1\n", "pwryp2\n", "pwryp3\n", "pwryp4\n", "pwryp5\n" };

char zpowers[][11] = {"pwrzn5\n", "pwrzn4\n", "pwrzn3\n", "pwrzn2\n", "pwrzn1\n",
					 "pwrz00\n",
					 "pwrzp1\n", "pwrzp2\n", "pwrzp3\n", "pwrzp4\n", "pwrzp5\n" };

static int ONE_MILLISECOND = 1000;
static int FIFTY_MILLISECONDS = ONE_MILLISECOND * 50;


static const char *bus_str(int bus);
static int wait_for_connect();
static int init(int fd);
static void read_loop(int fd);
static int send_to_mcu(char * msg);
static void input_repsonse(unsigned char * input_buf, unsigned char * last_input_buff, int buflen);
static void thumbstick_response(int stickid, unsigned char current_val, unsigned char last_val);


int main(int argc, char **argv)
{
	int fd;

	while (1)
	{
		fd = wait_for_connect();

		init(fd);
		read_loop(fd);

		printf("connection reset\n");
	}

	close(fd);
	return 0;
}

static int wait_for_connect()
{
	char device [] = "/dev/hidraw0";
	int fd = 0;
	char test;

	while (1)
	{
		fd = open(device, O_RDONLY);
		if (fd > 0)
		{
			if (read(fd, &test, 1) >=0 || errno != 11)
				return fd;
			else
			{
				printf("couldn't read..trying again\n");
				close(fd);
			}
		}
		else
			printf("couldn't open..trying again\n");

		usleep(FIFTY_MILLISECONDS);
	}

	return 0;
}

static void read_loop(int fd)
{
	printf("\n\nstarting read loop function\n");
	#define BUFF_LEN 32
	#define BUFF_PRINT_LEN 9
	unsigned char last[BUFF_LEN];
	int counter = 1;
	char buf[256];
	memset(last, 0xff, sizeof(unsigned char) * BUFF_LEN);
	while (1)
	{
		//struct report usage;
		//memset(&usage, 0, sizeof usage);
		unsigned char buff[BUFF_LEN];
		memcpy(buff, last, sizeof(unsigned char) * BUFF_LEN);
		if (read(fd, buff, sizeof(unsigned char) * BUFF_LEN) >= 0)
		{
			if (memcmp(buff, last, sizeof(unsigned char) * 9))
			{
				printf("message%d:", counter);
				for (int i = 0; i < BUFF_LEN; i++)
				{
					if (i < BUFF_PRINT_LEN)
						printf(" 0x%02x", buff[i]);
				}
				printf("\n");

				input_repsonse(buff, last, BUFF_PRINT_LEN);

				for (int i = 0; i < BUFF_LEN; i++)
					last[i] = buff[i];

				counter++;
			}
		}
		else if (errno != 11)
		{
			printf("read_loop: reseting connection: %d\n", errno);
			return;
		}
		usleep(ONE_MILLISECOND);
	}
}

static void input_repsonse(unsigned char * input_buf, unsigned char * last_input_buff, int buflen)
{
	unsigned char thumb_stick_axis[] = {THUMB_LEFT_X, THUMB_LEFT_Y, THUMB_RIGHT_X, THUMB_RIGHT_Y};
	int num_thumb_stick_axis = sizeof(thumb_stick_axis) / sizeof(unsigned char);

	for (int tstick_idx=0; tstick_idx < num_thumb_stick_axis; tstick_idx++)
	{
		unsigned char tstick_byte_offset = thumb_stick_axis[tstick_idx];
		if (input_buf[tstick_byte_offset] != last_input_buff[tstick_byte_offset])
		{
			thumbstick_response(thumb_stick_axis[tstick_idx], input_buf[tstick_byte_offset], last_input_buff[tstick_byte_offset]);
		}
	}

}

static void thumbstick_response(int stickid, unsigned char current_val, unsigned char last_val)
{
	unsigned char min_val [] = {THUMB_X_MIN, THUMB_Y_MIN, THUMB_X_MIN, THUMB_Y_MIN};
	unsigned char max_val [] = {THUMB_X_MAX, THUMB_Y_MAX, THUMB_X_MAX, THUMB_Y_MAX};
	unsigned char center_val [] = {THUMB_X_CENTER, THUMB_Y_CENTER, THUMB_X_CENTER, THUMB_Y_CENTER};
	char stickid_names [][15] = {"THUMB_LEFT_X", "THUMB_LEFT_Y", "THUMB_RIGHT_X", "THUMB_RIGHT_Y"};
	int stickid_idx = stickid - 1;
	int power_scaler = (max_val[stickid_idx] - min_val[stickid_idx]) / (NUM_POWER_LEVELS * 2);

	printf ("powerscaler = %d\n", power_scaler);
	int throttle = current_val / power_scaler;
	printf ("throttle = %d\n", throttle);

	if (stickid  == THUMB_LEFT_X)
	{
		printf("thumbstick_response invoked: %s\n", xpowers[throttle]);
		send_to_mcu(xpowers[throttle]);
	}
	else if (stickid  == THUMB_LEFT_Y)
	{
		printf("thumbstick_response invoked: %s\n", ypowers[throttle]);
		send_to_mcu(ypowers[throttle]);
	}
	else if (stickid  == THUMB_RIGHT_X)
	{
		printf("thumbstick_response invoked: %s\n", zpowers[throttle]);
		send_to_mcu(zpowers[throttle]);
	}
}

static int send_to_mcu(char * msg)
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


static int init(int fd)
{
	int i, res, desc_size = 0;
	char buf[256];
	struct hidraw_report_descriptor rpt_desc;
	struct hidraw_devinfo info;

	memset(&rpt_desc, 0x0, sizeof(rpt_desc));
	memset(&info, 0x0, sizeof(info));
	memset(buf, 0x0, sizeof(buf));

	/* Get Report Descriptor Size */
	res = ioctl(fd, HIDIOCGRDESCSIZE, &desc_size);
	if (res < 0)
		return -1;
	else
		printf("Report Descriptor Size: %d\n", desc_size);

	/* Get Report Descriptor */
	rpt_desc.size = desc_size;
	res = ioctl(fd, HIDIOCGRDESC, &rpt_desc);
	if (res < 0) {
		return -1;
	}
	else {
		printf("Report Descriptor:\n");
		for (i = 0; i < rpt_desc.size; i++)
			printf("%hhx ", rpt_desc.value[i]);
		puts("\n");
	}

	/* Get Raw Name */
	res = ioctl(fd, HIDIOCGRAWNAME(256), buf);
	if (res < 0)
		return -1;
	else
		printf("Raw Name: %s\n", buf);

	/* Get Physical Location */
	res = ioctl(fd, HIDIOCGRAWPHYS(256), buf);
	if (res < 0)
		return -1;
	else
		printf("Raw Phys: %s\n", buf);

	/* Get Raw Info */
	res = ioctl(fd, HIDIOCGRAWINFO, &info);
	if (res < 0) {
		return -1;
	}
	else {
		printf("Raw Info:\n");
		printf("\tbustype: %d (%s)\n",	info.bustype, bus_str(info.bustype));
		printf("\tvendor: 0x%04hx\n", info.vendor);
		printf("\tproduct: 0x%04hx\n", info.product);
	}

	return 0;
}

static const char * bus_str(int bus)
{
	switch (bus) {
	case BUS_USB:
		return "USB";
		break;
	case BUS_HIL:
		return "HIL";
		break;
	case BUS_BLUETOOTH:
		return "Bluetooth";
		break;
	case BUS_VIRTUAL:
		return "Virtual";
		break;
	default:
		return "Other";
		break;
	}
}