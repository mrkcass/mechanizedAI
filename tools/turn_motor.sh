#!/bin/bash

function init_gpio()
{
   local pin_num="$1"; local gpio_mode="$2"; local gpio_direction="$3"; local gpio_pull_mode="$4"; local gpio_pull_strength="$5"

   if [[ ! -e /sys/class/gpio/gpio$pin_num ]]; then
      echo "$pin_num" > /sys/class/gpio/export
   fi
   echo "mode$gpio_mode" > /sys/kernel/debug/gpio_debug/gpio$pin_num/current_pinmux
   echo "$gpio_pull_mode" > /sys/kernel/debug/gpio_debug/gpio$pin_num/current_pullmode
   echo "$gpio_direction" > /sys/kernel/debug/gpio_debug/gpio$pin_num/current_direction
   echo "$gpio_pull_strength" > /sys/kernel/debug/gpio_debug/gpio$pin_num/current_pullstrength
   if [[ $gpio_direction == "out" ]]; then
      echo "low" > /sys/kernel/debug/gpio_debug/gpio$pin_num/current_value
   fi

   echo "  GP$pin_num pinmux: $(cat /sys/kernel/debug/gpio_debug/gpio$pin_num/current_pinmux)"
   echo "  GP$pin_num direction: $(cat /sys/kernel/debug/gpio_debug/gpio$pin_num/current_direction)"
   echo "  GP$pin_num pullmode: $(cat /sys/kernel/debug/gpio_debug/gpio$pin_num/current_pullmode)"
   echo "  GP$pin_num pullstrength: $(cat /sys/kernel/debug/gpio_debug/gpio$pin_num/current_pullstrength)"
}

turn_motor()
{
   local a_pos="$1"; a_neg="$2"; b_pos="$3"; b_neg="$4"; phase="$5";

   if [[ "$phase" == "1" ]]; then
      echo "high" > /sys/kernel/debug/gpio_debug/gpio$a_pos/current_value
      echo "low"  > /sys/kernel/debug/gpio_debug/gpio$a_neg/current_value
      echo "low"  > /sys/kernel/debug/gpio_debug/gpio$b_pos/current_value
      echo "low" > /sys/kernel/debug/gpio_debug/gpio$b_neg/current_value
   fi

   if [[ "$phase" == "2" ]]; then
      echo "low"  > /sys/kernel/debug/gpio_debug/gpio$a_pos/current_value
      echo "low"  > /sys/kernel/debug/gpio_debug/gpio$a_neg/current_value
      echo "high" > /sys/kernel/debug/gpio_debug/gpio$b_pos/current_value
      echo "low"  > /sys/kernel/debug/gpio_debug/gpio$b_neg/current_value
   fi

   if [[ "$phase" == "3" ]]; then
      echo "low" > /sys/kernel/debug/gpio_debug/gpio$a_pos/current_value
      echo "high" > /sys/kernel/debug/gpio_debug/gpio$a_neg/current_value
      echo "low"  > /sys/kernel/debug/gpio_debug/gpio$b_pos/current_value
      echo "low"  > /sys/kernel/debug/gpio_debug/gpio$b_neg/current_value
   fi

   if [[ "$phase" == "4" ]]; then
      echo "low"  > /sys/kernel/debug/gpio_debug/gpio$a_pos/current_value
      echo "low"  > /sys/kernel/debug/gpio_debug/gpio$a_neg/current_value
      echo "low"  > /sys/kernel/debug/gpio_debug/gpio$b_pos/current_value
      echo "high" > /sys/kernel/debug/gpio_debug/gpio$b_neg/current_value
   fi

   if [[ "$phase" == "off" ]]; then
      echo "low"  > /sys/kernel/debug/gpio_debug/gpio$a_pos/current_value
      echo "low"  > /sys/kernel/debug/gpio_debug/gpio$a_neg/current_value
      echo "low"  > /sys/kernel/debug/gpio_debug/gpio$b_pos/current_value
      echo "low" > /sys/kernel/debug/gpio_debug/gpio$b_neg/current_value
   fi
}

motor_p()
{
   local phase="$1"

   local in1="13"
   local in2="165"
   local in3="12"
   local in4="183"

   turn_motor $in1 $in2 $in3 $in4 $phase
}

motor_t()
{
   local phase="$1"

   local in1="44"
   local in2="46"
   local in3="48"
   local in4="14"

   turn_motor $in1 $in2 $in3 $in4 $phase
}

motor_r()
{
   local phase="$1"

   local in1="182"
   local in2="114"
   local in3="129"
   local in4="131"


   turn_motor $in1 $in2 $in3 $in4 $phase
}

if [[ "$1" == "" || "$1" == "h" || "$1" == "help" || "$1" == "--help" ]]; then
   echo " "
   echo "   turn_motor.sh [p|t|r|off] [f|r] NNN DDD delay"
   echo " "
   echo "   turn a CAMD-stepper motor along a given axis."
   echo " "
   echo "   [p&r&t]|off"
   echo "      motor to move pan, tilt, or rotate."
   echo "       off will power down all motors."
   echo "   [f|r|p]"
   echo "      direction of step,forward of reverse. change to a phase"
   echo "   NNN"
   echo "      steps, phase to move (200 per revolution). phase = 0 - 3."
   echo "   DDD"
   echo "      step delay - microseconds between phase changes. default=500"
   echo " "
   echo " "
   exit
fi

phase_delay=$4
if [[ "$phase_delay" == "" ]]; then
   phase_delay="500"
fi
axis=$1

if [[ "$1" == "off" ]]; then
   motor_p off
   motor_t off
   motor_r off
fi
if [[ "$2" == "p" ]]; then
   motor_$axis $3
fi
if [[ "$2" == "f" ]]; then
   steps_taken=0
   while (( $steps_taken < $3 )); do
      usleep $phase_delay
      motor_$axis 1
      ((steps_taken++))
      if (( $steps_taken == $3 )); then
         break
      fi
      usleep $phase_delay
      motor_$axis 2
      ((steps_taken++))
      if (( $steps_taken == $3 )); then
         break
      fi
      usleep $phase_delay
      motor_$axis 3
      ((steps_taken++))
      if (( $steps_taken == $3 )); then
         break
      fi
      usleep $phase_delay
      motor_$axis 4
      ((steps_taken++))
   done
fi
if [[ "$2" == "r" ]]; then
   steps_taken=0
   while (( $steps_taken < $3 )); do
      usleep $phase_delay
      motor_$axis 4
      ((steps_taken++))
      if (( $steps_taken == $3 )); then
         break
      fi
      usleep $phase_delay
      motor_$axis 3
      ((steps_taken++))
      if (( $steps_taken == $3 )); then
         break
      fi
      usleep $phase_delay
      motor_$axis 2
      ((steps_taken++))
      if (( $steps_taken == $3 )); then
         break
      fi
      usleep $phase_delay
      motor_$axis 1
      ((steps_taken++))
   done
fi