#include <pthread.h>
#include <stdio.h>
#include "unistd.h"
#include "stdlib.h"
#include "memory.h"


#include "joystick3d_stick.h"

enum
{
   RUNSTATE_RUN,
   RUNSTATE_RUNNING,
   RUNSTATE_STOP,
   RUNSTATE_STOPPING,
   RUNSTATE_STOPPPED,
};

#define JOY3D_NAME_PAN           0
#define JOY3D_NAME_TILT          1
#define JOY3D_NAME_ROTATE        2
#define JOY3D_NAME_FRAME_ROTATE  3

#define JOY3D_AXIS_NAME_PAN "AXIS-PAN"
#define JOY3D_AXIS_NAME_TILT "AXIS-TILT"
#define JOY3D_AXIS_NAME_ROTATE "AXIS-ROTATE"
#define JOY3D_AXIS_NAME_FRAME_ROTATE "AXIS-FRAME-ROTATE"

#define JOY3D_INIT_AXIS_MIN 0
#define JOY3D_INIT_AXIS_MAX 1000000

//left: looking down on an upward facing stick and with your left hand closest to stick minimum
//right: looking down on the stick and with your right hand closest to stick maximum
struct AxisData
{
   int axis;
   char * name;

   int tolerance;
   int invert;

   int center;
   int dead_center_min, dead_center_max;
   int live_center_min, live_center_max;

   int left;
   int left_min, left_max;

   int right;
   int right_min, right_max;
};


//todo: this should be provided by the main function and not defined here.
char joy3d_axis_names[][32] =
{
   JOY3D_AXIS_NAME_PAN,
   JOY3D_AXIS_NAME_TILT,
   JOY3D_AXIS_NAME_ROTATE,
   JOY3D_AXIS_NAME_FRAME_ROTATE
};

Joystick::Joystick()
{
   axis_data = NULL;
   num_power_levels = 1;
}

Joystick::~Joystick()
{
   if (axis_data)
      for (int i=0; i < num_axis; i++)
         free(axis_data[i].name);
      delete [] axis_data;
}

//add an axis
//the value returned and used in messaging will be multiplied to by invert
//before use in these cases. causes right values (positive) to return as left
//values (negative). min/max messages will flip in calibration. handy when the axis
//is wired opposite of intuition. might be handy for implenting a simply headless mode.
int Joystick::add_axis(int joy3d_axis_id, const char * axis_name, int invert)
{
   if (this->axis_data == NULL)
      this->axis_data = new AxisData[MAX_AXIS];
   axis_data[num_axis].axis = joy3d_axis_id;
   axis_data[num_axis].name = strdup(axis_name);
   axis_data[num_axis].tolerance = minmax_tolerance_factor;
   axis_data[num_axis].invert = invert;
   //center
   axis_data[num_axis].center = JOY3D_INIT_AXIS_MIN;
   axis_data[num_axis].dead_center_min = JOY3D_INIT_AXIS_MAX;
   axis_data[num_axis].dead_center_max = JOY3D_INIT_AXIS_MIN;
   axis_data[num_axis].live_center_min = JOY3D_INIT_AXIS_MAX;
   axis_data[num_axis].live_center_max = JOY3D_INIT_AXIS_MIN;
   //left
   axis_data[num_axis].right = JOY3D_INIT_AXIS_MIN;
   axis_data[num_axis].right_min = JOY3D_INIT_AXIS_MAX;
   axis_data[num_axis].right_max = JOY3D_INIT_AXIS_MIN;
   //right
   axis_data[num_axis].left = JOY3D_INIT_AXIS_MIN;
   axis_data[num_axis].left_min = JOY3D_INIT_AXIS_MAX;
   axis_data[num_axis].left_max = JOY3D_INIT_AXIS_MIN;
   num_axis++;
   return 0;
}

//provide the information needed to sample data and should be passed ready to use.
//provide callback functions for button and stick axis data.
int Joystick::connect(STICK_SAMPLER stick_sampler, BUTTON_SAMPLER button_sampler,
                      JOY3D_CALLBACK callback, int minmax_tolerance_factor, int power_steps)
{
   this->minmax_tolerance_factor = minmax_tolerance_factor;
   this->num_power_levels = power_steps;
   this->stick_sampler = stick_sampler;
   this->button_sampler = button_sampler;
   this->changed_cb = callback;
   return 0;
}

int Joystick::run()
{
   if (load_calibration())
      return 1;
   int samples_per_sec = 180;
   int * last = new int[num_axis+1];
   int current;
   int *debounce = new int[num_axis + 1];
   int debounce_threshold = 1;
   while (1)
   {
      for (int axis = 0; axis <= num_axis; axis++)
      {
         if (axis == num_axis)
            current = this->button_sampler();
         else
            current = calc_power(axis);
         if (last[axis] == current)
         {
            if (debounce[axis] == debounce_threshold)
            {
               debounce[axis]++;
               changed_cb(axis, current);
            }
            else
               debounce[axis]++;
         }
         else
            debounce[axis] = 0;

         last[axis] = current;
      }
      usleep(1000000 / samples_per_sec);
   }
   return 0;
}

int Joystick::calc_power(int axis)
{
   return calc_linear_power(axis);
}

int Joystick::calc_linear_power(int axis)
{
   int range_power_levels = (num_power_levels / 2) - 1;
   int max_power = range_power_levels + 1;
   AxisData *adat = &axis_data[axis];

   int sample = this->stick_sampler(axis);

   if (sample > adat->dead_center_min && sample < adat->dead_center_max)
      return 0;

   if (sample < adat->dead_center_min)
   {
      if (sample < adat->left_max)
         return -max_power;
      float sample_range = adat->dead_center_min - adat->left_max;
      float sample_norm = sample - adat->left_max;
      float neg_power_ratio = sample_norm / -sample_range;
      float power_level = (float)range_power_levels * neg_power_ratio;
      return -range_power_levels - (int)power_level;
   }

   if (sample > adat->right_min)
      return max_power;
   float sample_range = adat->right_min - adat->dead_center_max;
   float sample_norm = sample - adat->dead_center_max;
   float power_ratio = sample_norm / sample_range;
   float power_level = (float)range_power_levels * power_ratio;
   return (int)power_level + 1;
}

//till control-c sample from the stick sampler to verify that the callback
//is passing data. for development not production.
int Joystick::testsampler()
{
   printf("Testing JOY3D Sampler:\n");
   while (1)
   {
      int a0 = stick_sampler(JOY3D_AXIS_X);
      int a1 = stick_sampler(JOY3D_AXIS_Y);
      int a2 = stick_sampler(JOY3D_AXIS_Z);

      printf("   JOY3D_AXIS_X[0]: %6d   JOY3D_AXIS_Y[2]: %6d  JOY3D_AXIS_Z[1]: %6d\r", a0, a1, a2);
      fflush(stdout);
      usleep(1000000 / 20);
   }
   return 0;
}

int Joystick::calibrate()
{
   bool button_pressed;
   int x_val, y_val, new_button;
   int calibration_mode = 0;

   int ONE_MILLISECOND = 1000;
   int FIFTY_MILLISECONDS = ONE_MILLISECOND * 50;

   button_pressed = false;

   printf("\n\nEntering stick calibration mode\n");

   while (calibration_mode != 100)
   {
      x_val = this->stick_sampler(JOY3D_AXIS_X);
      y_val = this->stick_sampler(JOY3D_AXIS_Y);
      button_pressed = this->button_sampler();
      new_button = 0;

      printf("x: %03d y: %03d b: % 03d\n", x_val, y_val, new_button);

      if (button_pressed && new_button == 0)
      {
         printf("button released\n");
         button_pressed = false;
         if (calibration_mode == 0)
            calibration_mode = 2;
      }
      else if (!button_pressed && new_button == 1)
      {
         button_pressed = true;
         calibration_mode++;
      }
      else if (calibration_mode == 1)
      {
         calibrate_dead_center(JOY3D_AXIS_X);
         calibrate_dead_center(JOY3D_AXIS_Y);
         calibrate_dead_center(JOY3D_AXIS_Z);
         calibrate_minmax(JOY3D_AXIS_X);
         calibrate_minmax(JOY3D_AXIS_Y);
         calibrate_minmax(JOY3D_AXIS_Z);
         calibration_mode = 3;
      }
      else if (calibration_mode == 3)
      {
         calibration_mode = 100;
         write_calibration();
         printf("exiting calibartion.\n");
      }
      usleep(FIFTY_MILLISECONDS);
   }
   return false;
}

//read and calibrate the band cuttoff for center stick on axis
bool Joystick::calibrate_dead_center(int axis)
{
   int ONE_SECOND = 1000;

   printf("\nStarting %s center dead stick calibration in 1 second. don't move the stick\n", axis_data[axis].name);
   usleep(ONE_SECOND);
   printf("  sampling.");
   axis_data[axis].center = sample_and_average(axis, 50, ONE_SECOND, &axis_data[axis].dead_center_min, &axis_data[axis].dead_center_max);
   axis_data[axis].dead_center_max += (axis_data[axis].dead_center_max - axis_data[axis].center) * minmax_tolerance_factor;
   axis_data[axis].dead_center_min += (axis_data[axis].dead_center_min - axis_data[axis].center) * minmax_tolerance_factor;

   printf("\n   CALIBRATION[%s] center = %3d, min_center = %3d , max_center = %3d\n",
          axis_data[axis].name,
          axis_data[axis].center,
          axis_data[axis].dead_center_min,
          axis_data[axis].dead_center_max);
   printf("dead stick calibration complete. Passed\n");

   return true;
}

//find and calibrate the minimum and maximum cutoffs for axis
bool Joystick::calibrate_minmax(int axis)
{
   int ONE_SECOND = 1000;
   printf("\n\nSTARTING: %s min / max power calibration\n", axis_data[axis].name);

   wait_sample_threshold(axis, 0);
   printf("\n");
   wait_sample_threshold(axis, (int)(axis_data[axis].center * -.2));
   printf("\n  %s at 0 power. sampling.", axis_data[axis].name);
   axis_data[axis].left_min = this->stick_sampler(axis_data[axis].axis) * 2;
   axis_data[axis].left_max = this->stick_sampler(axis_data[axis].axis) / 2;
   axis_data[axis].left = sample_and_average(axis, 50, 1000, &axis_data[axis].left_min, &axis_data[axis].left_max);
   axis_data[axis].left_max += (axis_data[axis].left_max - axis_data[axis].left_min) * minmax_tolerance_factor;
   printf("\n");
   printf("   CALIBRATION[%s] left = %6d, min_left = %6d, max_left = %6d\n",
          axis_data[axis].name,
          axis_data[axis].left,
          axis_data[axis].left_min,
          axis_data[axis].left_max);

   wait_sample_threshold(axis, 0);
   printf("\n");
   wait_sample_threshold(axis, (int)(axis_data[axis].center * 1.8));
   printf("\n  %s at full power. sampling.", axis_data[axis].name);
   axis_data[axis].right_min = this->stick_sampler(axis_data[axis].axis) * 2;
   axis_data[axis].right_max = this->stick_sampler(axis_data[axis].axis) / 2;
   axis_data[axis].right = sample_and_average(axis, 50, ONE_SECOND, &axis_data[axis].right_min, &axis_data[axis].right_max);
   axis_data[axis].right_min -= (axis_data[axis].right_max - axis_data[axis].right_min) * minmax_tolerance_factor;
   printf("\n   CALIBRATION[%s] right = %6d, min_right = %6d,  max_right = %6d\n",
          axis_data[axis].name,
          axis_data[axis].right,
          axis_data[axis].right_min,
          axis_data[axis].right_max);
   printf("\n");
   wait_sample_threshold(axis, 0);


   printf("\nCOMPLETE: %s min/max power calibration\n", axis_data[axis].name);

   return true;
}

//*wait until samples are being passed by a hi/lo/band pass filter.
//*if threshold is negative then invert threshold and use as low pass cutoff
//otherwise. use threshold unmodified as hi pass cutoff.
//*if threshold is 0 then use dead_center min/max a band pass cutoff.
//*for all values of threshold sample until the filter passes samples.
//*if live_center then sample and calibrate the min and max extent of all axis' other
//than axis.
bool Joystick::wait_sample_threshold(int axis, int threshold, bool live_center)
{
   int ONE_SECOND = 1000 * 1000;

   if (threshold == 0)
      printf("  <-->CENTER STICK %s \n", axis_data[axis].name);
   else if ((axis_data[axis].invert == false && threshold < 0) || (axis_data[axis].invert == true && threshold > 0))
      printf("  <-->HOLD   STICK %s at minimum\n", axis_data[axis].name);
   else
      printf("  <-->HOLD   STICK %s maximum\n", axis_data[axis].name);

   int sample = this->stick_sampler(axis_data[axis].axis);

   while (1)
   {
      if (threshold == 0 && sample > axis_data[axis].dead_center_min && sample < axis_data[axis].dead_center_max)
         break;
      if ((threshold < 0 && sample < -threshold) || (threshold > 0 && sample > threshold))
         break;
      sample = this->stick_sampler(axis_data[axis].axis);
      if (live_center)
      {
         update_live_centers(axis);
      }
      printf("  %s center = %6d current = %6d\r", axis_data[axis].name, axis_data[axis].center, sample);
      usleep(ONE_SECOND / 16);
   }
   usleep(ONE_SECOND / 2);

   return true;
}

//sample an axis for duration_ms milliseconds and return  average
//after collecting num_samples
int Joystick::sample_and_average(int axis, int num_samples, int duration_ms, int *min_buff, int *max_buff)
{
   int MICRO_PER_MILLI = 1000;
   int local_min;
   int local_max;
   int * min = &local_min;
   int * max = &local_max;

   if (min_buff)
      min = min_buff;
   if (max_buff)
      max = max_buff;
   //*min = this->stick_sampler(axis) * 10;
   //*max = 0;

   int sample_time = duration_ms * MICRO_PER_MILLI;
   int sample_rate = sample_time / num_samples;

   int sample_sum = 0;
   for (int counter = 0; counter < num_samples; counter++)
   {
      int sample = this->stick_sampler(axis);
      sample_sum += sample;
      if (sample < *min)
         *min = sample;
      else if (sample > *max)
         *max = sample;
      usleep(sample_rate);
      printf(".");
      fflush(stdout);
   }

   return sample_sum / num_samples;
}

//for axis' other than live_axis update the live center min / max
//band cutoff accordingly.
void Joystick::update_live_centers(int live_axis)
{
   for (int axis = 0; axis < num_axis; axis++)
   {
      if (live_axis == axis)
         continue;
      int sample = this->stick_sampler(axis);
      if (sample > axis_data[axis].live_center_max)
         axis_data[axis].live_center_max = sample;
      else if (sample < axis_data[axis].live_center_min)
         axis_data[axis].live_center_min = sample;
   }
}

void Joystick::write_calibration()
{
   FILE * calib_out;

   calib_out = fopen(CALIBRATION_FILE, "rw+");

   if (calib_out == NULL)
   {
      printf("\n\nAn Error occured while writing the calibration file\n\n");
      return;
   }

   fprintf(calib_out, "# Calibration data for SOMAX 3D Joystick\n");
   fprintf(calib_out, "# lines starting with # are ignored\n");
   fprintf(calib_out, "# name : axis id : tolerance : invert : center : dead_center_min : dead_center_max : live_center_min : live_center_max : left, left_min : left_max : right : right_min : right_max\n");

   for (int axis = 0; axis < num_axis; axis++)
   {

      fprintf(calib_out, "%20s : %1d : %3d : %1d : %5d : %5d : %5d : %5d : %5d : %4d : %4d : %4d : %5d : %5d : %5d\n",
              axis_data[axis].name,
              axis_data[axis].axis,
              axis_data[axis].tolerance,
              axis_data[axis].invert,
              axis_data[axis].center,
              axis_data[axis].dead_center_min,
              axis_data[axis].dead_center_max,
              axis_data[axis].live_center_min,
              axis_data[axis].live_center_max,
              axis_data[axis].left,
              axis_data[axis].left_min,
              axis_data[axis].left_max,
              axis_data[axis].right,
              axis_data[axis].right_min,
              axis_data[axis].right_max);
   }

   fclose(calib_out);
   printf("Wrote Calibration to: %s\n", CALIBRATION_FILE);
}

int Joystick::load_calibration()
{
   FILE *calib_in;

   calib_in = fopen(CALIBRATION_FILE, "r");

   if (calib_in == NULL)
   {
      printf("\n\nAn Error occured while loading the joystick3d calibration file\n\n");
      return -1;
   }
   else
      printf("Loading Calibration from: %s\n", CALIBRATION_FILE);

   char buff[1024];
   int bufflen = 0;
   int axis = 0;
   int c = 'a';
   bool is_comment = false;
   while (c != EOF)
   {
      c = fgetc(calib_in);
      if ((bufflen == 0 && c == '#'))
      {
         is_comment = true;
      }
      else if (c == '\n' || (c == EOF && bufflen > 0))
      {
         buff[bufflen] = '\0';
         bufflen++;
         if (is_comment == true || bufflen == 0 || !strstr(buff, "AXIS-"))
         {
            is_comment = false;
            bufflen = 0;
            printf("%s\n", buff);
            continue;
         }

         sscanf(buff, "%20s : %d : %d : %d : %d : %d : %d : %d : %d : %d : %d : %d : %d : %d : %d",
                  axis_data[axis].name,
                  &axis_data[axis].axis,
                  &axis_data[axis].tolerance,
                  &axis_data[axis].invert,
                  &axis_data[axis].center,
                  &axis_data[axis].dead_center_min,
                  &axis_data[axis].dead_center_max,
                  &axis_data[axis].live_center_min,
                  &axis_data[axis].live_center_max,
                  &axis_data[axis].left,
                  &axis_data[axis].left_min,
                  &axis_data[axis].left_max,
                  &axis_data[axis].right,
                  &axis_data[axis].right_min,
                  &axis_data[axis].right_max);
         printf("  CALIBRATION[%20s] : %1d : %3d : %1d : %5d : %5d : %5d : %5d : %5d : %4d : %4d : %4d : %5d : %5d : %5d\n",
                  axis_data[axis].name,
                  axis_data[axis].axis,
                  axis_data[axis].tolerance,
                  axis_data[axis].invert,
                  axis_data[axis].center,
                  axis_data[axis].dead_center_min,
                  axis_data[axis].dead_center_max,
                  axis_data[axis].live_center_min,
                  axis_data[axis].live_center_max,
                  axis_data[axis].left,
                  axis_data[axis].left_min,
                  axis_data[axis].left_max,
                  axis_data[axis].right,
                  axis_data[axis].right_min,
                  axis_data[axis].right_max);
         bufflen = 0;
         axis++;
      }

      buff[bufflen] = (char)c;
      bufflen++;
   }

   fclose(calib_in);
   printf("Loaded Calibration from: %s\n", CALIBRATION_FILE);
   return 0;
}
