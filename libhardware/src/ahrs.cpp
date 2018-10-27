#include <stdio.h>
#include <math.h>

#include "ahrs.h"
#include "bno055.h"

struct AHRS_CONTEXT
{
    unsigned char this_is_not_populated_on_purpose;
};

static const char ahrs_names[][32] =
    {"AHRS-NULL", "AHRS-FRAME[BNO055]", "AHRS-CAMD[BNO055]"};

static AHRS_EULER_CALLBACK outputdata_euler_callbk;
static AHRS_QUATERNION_CALLBACK outputdata_quaternion_callbk;
static AHRS_MAGNETOMETER_CALLBACK outputdata_magnetometer_callbk;
static int ahrs_bno055_id_map[] = {0, BNO_DEVICE_FRAME, BNO_DEVICE_CAMD};

static void ahrs_test_euler_callback(ahrs_context ahrs, float heading, float pitch, float roll);
static void ahrs_test_quaternion_callback(ahrs_context ahrs, float w, float x, float y, float z);
static void ahrs_test_magnetometer_callback(ahrs_context ahrs, int mag_x, int mag_y, int mag_z);


ahrs_context ahrs_open(ahrs_id id)
{
   if (id <= 0 || id > AHRS_NUM_DEVICES)
      return NULL;

   ahrs_context ctx;
   ctx = bno055_open(ahrs_bno055_id_map[id]);

   return ctx;
}

void ahrs_close()
{
   bno055_close();
}

void ahrs_info(ahrs_context ahrs)
{
   printf("AHRS chip info: %s -------------\n", ahrs_names[ahrs_context_to_id(ahrs)]);
   bno055_info(ahrs);
}

int ahrs_cfg_run_mode(ahrs_context ahrs, smx_byte mode)
{
   if (mode == AHRS_RUNMODE_IMU)
      bno055_cfg_run_mode(ahrs, BNO_RUNMODE_ACCMAGGYRO);
   else
      bno055_cfg_run_mode(ahrs, BNO_RUNMODE_PANTILTROT);

   return 0;
}

int ahrs_test(int output_format)
{
   printf("starting test: \n");
   if (output_format == AHRS_OUTPUTFORMAT_EULER)
      ahrs_run(ahrs_test_euler_callback, NULL, NULL);
   else if (output_format == AHRS_OUTPUTFORMAT_QUATERNION)
      ahrs_run(NULL, ahrs_test_quaternion_callback, NULL);
   else if (output_format == AHRS_OUTPUTFORMAT_MAGNETOMETER)
      ahrs_run(NULL, NULL, ahrs_test_magnetometer_callback);
   return 1;
}

int ahrs_run(AHRS_EULER_CALLBACK euler_callbk, AHRS_QUATERNION_CALLBACK quaternion_callbk, AHRS_MAGNETOMETER_CALLBACK magnetometer_callbk)
{
   outputdata_euler_callbk = NULL;
   outputdata_quaternion_callbk = NULL;

   if (euler_callbk != NULL)
   {
      outputdata_euler_callbk = euler_callbk;
      bno055_output_callbk_euler(NULL, outputdata_euler_callbk);
   }
   else if (quaternion_callbk != NULL)
   {
      outputdata_quaternion_callbk = quaternion_callbk;
      bno055_output_callbk_quaternion(NULL, outputdata_quaternion_callbk);
   }
   else if (magnetometer_callbk != NULL)
   {
      printf("debug 100\n");
      outputdata_magnetometer_callbk = magnetometer_callbk;
      bno055_output_callbk_magnetometer(NULL, outputdata_magnetometer_callbk);
   }
   else
      return 0;

   bno055_run();

   return 1;
}

void ahrs_quaternion_to_euler(const float qw, const float qx, const float qy, const float qz, float *ex, float *ey, float *ez)
{
   // heading
   //double siny_cosp = +2.0 * (qw * qz + qx * qy);
   //double cosy_cosp = +1.0 - 2.0 * (qy * qy + qz * qz);
   //*ex = atan2(siny_cosp, cosy_cosp);
   *ex = atan2(2.0f * (qx * qy + qw * qz), qw * qw + qx * qx - qy * qy - qz * qz);

   // pitch
   // double sinr_cosp = +2.0 * (qw * qx + qy * qz);
   // double cosr_cosp = +1.0 - 2.0 * (qx * qx + qy * qy);
   // *ey = atan2(sinr_cosp, cosr_cosp);
   //*ey = asin(2 * qy * qz - 2 * qx * qw);
   *ey = -asin(2.0f * (qx * qz - qw * qy));


   // roll
   // double sinp = +2.0 * (qw * qy - qz * qx);
   // if (fabs(sinp) >= 1)
   //    *ez = copysign(M_PI / 2, sinp); // use 90 degrees if out of range
   // else
   //    *ez = asin(sinp);
   //*ez = -atan2(2 * qx * qz + 2 * qy * qw, 1 - 2 * qx * qx - 2 * qy * qy);
   *ez = atan2(2.0f * (qw * qx + qy * qz), qw * qw - qx * qx - qy * qy + qz * qz);


   *ex = (*ex * 180.0) / M_PI;
   *ey = (*ey * 180.0) / M_PI;
   *ez = (*ez * 180.0) / M_PI;
}

int ahrs_context_to_id(ahrs_context ahrs)
{
   int bno_id = bno055_context_to_id(ahrs);
   int ahrs_id = 0;

   for (int i=1; i < AHRS_NUM_DEVICES+1; i++)
   {
      if (ahrs_bno055_id_map[i] == bno_id)
      {
         ahrs_id = i;
         break;
      }
   }
   return ahrs_id;
}

static void ahrs_test_euler_callback(ahrs_context ahrs, float heading, float pitch, float roll)
{
   static float current[AHRS_NUM_DEVICES + 1][AHRS_NUM_AXIS];
   static int calib[AHRS_NUM_DEVICES + 1][4];

   int device_id = ahrs_context_to_id(ahrs);

   current[device_id][0] = heading;
   current[device_id][1] = pitch;
   current[device_id][2] = roll;

   int calibstat_sys, calibstat_gyro, calibstat_acc, calibstat_mag;

   calibstat_sys = calibstat_gyro = calibstat_acc = calibstat_mag = 0;

   bno055_calibration_status(ahrs, &calibstat_sys, &calibstat_gyro, &calibstat_acc, &calibstat_mag);
   calib[device_id][0] = calibstat_sys;
   calib[device_id][1] = calibstat_gyro;
   calib[device_id][2] = calibstat_acc;
   calib[device_id][3] = calibstat_mag;

   int CAM = AHRS_ID_CAMD;
   int FRAME = AHRS_ID_FRAME;

   float rot_diff = 0;
   float frot = current[FRAME][2];
   float frot_diff = 0;
   if (frot < 0.0)
      frot_diff = 180.0 + current[FRAME][2];
   else
      frot_diff = 180.0 - current[FRAME][2];

   float crot = current[CAM][2];
   float crot_diff = 0;
   if (crot < 0.0)
      crot_diff = 180.0 + current[CAM][2];
   else
      crot_diff = 180.0 - current[CAM][2];

   if ((frot > 0.0 && crot > 0.0) || (frot < 0.0 && crot < 0.0))
      rot_diff = frot - crot;
   else
      rot_diff = frot_diff + crot_diff;
   if (rot_diff < 0)
      rot_diff *= -1;

   printf("FC-PAN[%3.2f|%3.2f|%3.0f] FC-TLT[%3.2f|%3.2f|%3.0f] FC-ROT[%3.2f|%3.2f|%3.0f] FCALSGAM[%1d:%1d:%1d:%1d] CCALSGAM[%1d:%1d:%1d:%1d]   \r",
          current[FRAME][0], current[CAM][0], current[FRAME][0] > current[CAM][0] ? (current[FRAME][0] - current[CAM][0]) : (current[CAM][0] - current[FRAME][0]),
          current[FRAME][1], current[CAM][1], current[FRAME][1] > current[CAM][1] ? (current[FRAME][1] - current[CAM][1]) : (current[CAM][1] - current[FRAME][1]),
          current[FRAME][2], current[CAM][2], rot_diff,

          calib[FRAME][0], calib[FRAME][1], calib[FRAME][2], calib[FRAME][3],
          calib[CAM][0], calib[CAM][1], calib[CAM][2], calib[CAM][3]);
   fflush(stdout);
}

static void ahrs_test_quaternion_callback(ahrs_context ahrs, float w, float x, float y, float z)
{
   float heading, pitch, roll;
   ahrs_quaternion_to_euler(w, x, y, z, &heading, &pitch, &roll);
   ahrs_test_euler_callback(ahrs, heading, pitch, roll);
}

static void ahrs_test_magnetometer_callback(ahrs_context ahrs, int mag_x, int mag_y, int mag_z)
{
   static int current[AHRS_NUM_DEVICES + 1][AHRS_NUM_AXIS];

   int device_id = ahrs_context_to_id(ahrs);

   current[device_id][0] = mag_x;
   current[device_id][1] = mag_y;
   current[device_id][2] = mag_z;

   printf("FC-MAGx[%+5d|%+5d] FC-MAGy[%+5d|%+5d] FC-MAGz[%+5d|%+5d]         \r",
          current[AHRS_ID_FRAME][0], current[AHRS_ID_CAMD][0],
          current[AHRS_ID_FRAME][1], current[AHRS_ID_CAMD][1],
          current[AHRS_ID_FRAME][2], current[AHRS_ID_CAMD][2]);
}
