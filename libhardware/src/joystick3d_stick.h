#ifndef __joystick3d_h__
#define __joystick3d_h__

#include "wire.h"

#define MAX_AXIS 4
#define JOY3D_AXIS_X       0
#define JOY3D_AXIS_Y       1
#define JOY3D_AXIS_Z       2
#define JOY3D_AXIS_W       3
#define JOY3D_BUTTON       0

#define JOY3D_RANGE 128

#define JOY3D_BTN_ANLOG_THRESHOLD 650

typedef void (*JOY3D_CALLBACK)(int axis, char pos);
typedef int (*STICK_SAMPLER)(int axis);
typedef int (*BUTTON_SAMPLER)();

#define JOY3D_NUM_AXIS     3
#define JOY3D_NUM_BTN      1

#define CALIBRATION_FILE "joystick3d.calibration"

struct AxisData;

struct Joystick
{

   Joystick();
   ~Joystick();
   int add_axis(int joy3d_axis_id, const char * axis_name, int invert);
   int connect(STICK_SAMPLER joy_sampler, BUTTON_SAMPLER btn_sampler,
               JOY3D_CALLBACK callback, int minmax_tolerance_factor,
               int num_power_levels);
   int run();
   int calibrate();
   int testsampler();

private:
   bool calibrate_dead_center(int axis);
   bool calibrate_minmax(int axis);
   void update_live_centers(int live_axis);
   void write_calibration();
   int load_calibration();

   bool wait_sample_threshold(int axis, int threshold, bool live_center = false);
   int sample_and_average(int axis, int num_samples, int duration_ms, int *min_buff, int *max_buff);

   int calc_power(int axis);
   int calc_linear_power(int val);

   JOY3D_CALLBACK changed_cb;
   STICK_SAMPLER stick_sampler;
   BUTTON_SAMPLER button_sampler;

   AxisData * axis_data;
   int num_axis;
   int minmax_tolerance_factor;
   int num_power_levels;
};

#endif
