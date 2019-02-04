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

#include <somax.h>
#include "bluetooth_stick.h"

struct report {
   unsigned char id;
   unsigned char junk[3];
   unsigned char key;
   unsigned char effs[5];
   unsigned short press;
};

#define THUMB_LEFT_X			   1
#define THUMB_LEFT_Y			   2
#define THUMB_RIGHT_X			3
#define THUMB_RIGHT_Y			4
#define DPAD_LEFT				   5
#define DPAD_UP					5
#define DPAD_RIGHT				5
#define DPAD_DOWN				   5
#define TRIGGER_LEFT			   7
#define TRIGGER_RIGHT			8
#define BUTTON_A				   5
#define BUTTON_B				   5
#define BUTTON_X				   5
#define BUTTON_Y				   5
#define BUTTON_LEFT_SHOULDER	6
#define BUTTON_RIGHT_SHOULDER	6
#define BUTTON_LEFT_THUMB		6
#define BUTTON_RIGHT_THUMB		6

#define THUMB_MIN			   0x00
#define THUMB_CENTER		   0x80
#define THUMB_MAX			   0xff

#define THUMB_Y_MIN			0x00
#define THUMB_Y_CENTER		0x7f
#define THUMB_Y_MAX			0xff

#define CENTER_ZONE_DELTA  0x10


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

static int ONE_MILLISECOND = 1000;
static int FIFTY_MILLISECONDS = ONE_MILLISECOND * 50;


static const char *bus_str(int bus);
static int wait_for_connect();
static int init(int fd);
static void read_loop(int fd);
static void input_repsonse(unsigned char * input_buf, unsigned char * last_input_buff, int buflen);
static void thumbstick_response(int stickid, unsigned char current_val, unsigned char last_val);

static int bluejoy_num_power_levels;
static bluejoy_input_observer bluejoy_pwrlevel_observer;


int bluetoothjoy_connect(bluejoy_input_observer observer, int num_power_levels)
{
   bluejoy_pwrlevel_observer = observer;
   bluejoy_num_power_levels = num_power_levels;
   return 0;
}

int bluetoothjoy_run()
{
   int fd;

   while (1)
   {
      fd = wait_for_connect();

      init(fd);
      read_loop(fd);
      close(fd);
      printf("connection reset\n");
   }

   return 0;
}

static int wait_for_connect()
{
   char device0 [] = "/dev/hidraw0";
   char device1 [] = "/dev/hidraw1";
   char * device = device0;
   int fd = 0;
   char test;
   int open_error_msg_count = 0;
   int read_error_msg_count = 0;

   while (1)
   {
      fd = open(device0, O_RDONLY);

      if (fd > 0)
      {
         if (read(fd, &test, 1) >=0 || errno != 11)
         {
            open_error_msg_count = 0;
            read_error_msg_count = 0;
            return fd;
         }
         else if (!read_error_msg_count)
         {
            printf("BLUETOOTHSTICK: couldn't read..trying again\n");
            close(fd);
            read_error_msg_count++;
         }
      }
      else
      {
         if (!open_error_msg_count)
         {
            printf("BLUETOOTHSTICK: couldn't open. will poll every 50 milliseconds\n");
            open_error_msg_count++;
         }
         if (device == device0)
            device = device1;
         else
            device = device0;
      }

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

   memset(last, 0xff, sizeof(unsigned char) * BUFF_LEN);
   while (1)
   {
      //struct report usage;
      //memset(&usage, 0, sizeof usage);
      unsigned char buff[BUFF_LEN];
      int bytes_read = read(fd, buff, sizeof(unsigned char) * BUFF_LEN);
      if (bytes_read >= 0)
      {
         if (memcmp(buff, last, sizeof(unsigned char) * 9))
         {
            // printf("message%d:", counter);
            // for (int i = 0; i < BUFF_LEN; i++)
            // {
            // 	if (i < BUFF_PRINT_LEN)
            // 		printf(" 0x%02x", buff[i]);
            // }
            // printf("\n");

            input_repsonse(buff, last, BUFF_PRINT_LEN);

            memcpy(buff, last, sizeof(unsigned char) * BUFF_LEN);
         }
      }
      else if (errno != 11)
      {
         printf("read_loop: reseting connection: %d\n", errno);
         return;
      }
      usleep(ONE_MILLISECOND * 5);
   }
}

// message100: 0x03 0x80 0x7f 0x80 0x7f 0x0f 0x01 0x00 0x00
// message101: 0x03 0x80 0x7f 0x80 0x7f 0x0f 0x00 0x00 0x00
// message102: 0x03 0x80 0x7f 0x80 0x7f 0x0f 0x01 0x00 0x00
// message103: 0x03 0x80 0x7f 0x80 0x7f 0x0f 0x00 0x00 0x00
// message104: 0x03 0x80 0x7f 0x80 0x7f 0x0f 0x02 0x00 0x00
// message105: 0x03 0x80 0x7f 0x80 0x7f 0x0f 0x00 0x00 0x00
// message106: 0x03 0x80 0x7f 0x80 0x7f 0x0f 0x02 0x00 0x00
// message107: 0x03 0x80 0x7f 0x80 0x7f 0x0f 0x00 0x00 0x00

//message29: 0x03 0x80 0x7f 0x80 0x7f 0x0f 0x00 0x00 0x00

static void input_repsonse(unsigned char * input_buf, unsigned char * last_input_buff, int buflen)
{
   unsigned char thumb_stick_axis[] = {THUMB_LEFT_X, THUMB_LEFT_Y, THUMB_RIGHT_X, THUMB_RIGHT_Y, BUTTON_LEFT_SHOULDER, BUTTON_RIGHT_SHOULDER};
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
   static int last_throttle[] = {0,0,0,0};
   if (stickid == BUTTON_LEFT_SHOULDER || stickid == BUTTON_RIGHT_SHOULDER)
   {
      static int last_button = 0;
      if (current_val != last_button)
      {
         int VALUE_LEFT_SHOULDER_PRESSED = 0x01;
         int VALUE_RIGHT_SHOULDER_PRESSED = 0x02;
         if (current_val == VALUE_LEFT_SHOULDER_PRESSED || current_val == VALUE_RIGHT_SHOULDER_PRESSED)
            bluejoy_pwrlevel_observer(BLUEJOY_BUTTON, 0);
         else
            bluejoy_pwrlevel_observer(BLUEJOY_BUTTON, 1);
         last_button = current_val;
      }

      return;
   }


   int stickid_idx = stickid - 1;
   int throttle;

   if (current_val >= THUMB_CENTER - CENTER_ZONE_DELTA && current_val <= THUMB_CENTER + CENTER_ZONE_DELTA)
      throttle = 0;
   else
   {
      int power_range = (bluejoy_num_power_levels) / 2;
      float stick_range = THUMB_MAX - (THUMB_CENTER + CENTER_ZONE_DELTA);
      if (current_val > THUMB_CENTER)
      {
         power_range++;
         float stick_norm_to_range = current_val - (THUMB_CENTER + CENTER_ZONE_DELTA);
         float stick_percent = stick_norm_to_range / stick_range;
         throttle = (float)-power_range * stick_percent;
      }
      else
      {
         float stick_norm_to_range = current_val;
         float stick_percent = stick_norm_to_range / stick_range;
         throttle = power_range - (int)((float)power_range * stick_percent);
      }
   }


   if (throttle == last_throttle[stickid_idx])
      return;

   last_throttle[stickid_idx] = throttle;

   if (stickid  == THUMB_LEFT_X)
   {
      bluejoy_pwrlevel_observer(BLUEJOY_AXIS_X, throttle);
   }
   else if (stickid  == THUMB_LEFT_Y)
   {
      bluejoy_pwrlevel_observer(BLUEJOY_AXIS_Y, throttle * -1);
   }
   else if (stickid  == THUMB_RIGHT_X)
   {
      bluejoy_pwrlevel_observer(BLUEJOY_AXIS_Z, throttle * -1);
   }
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
      for (i = 0; i < (int)rpt_desc.size; i++)
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