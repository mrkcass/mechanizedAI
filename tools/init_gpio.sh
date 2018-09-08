#!/bin/bash

echo "INITGPIO" >> /root/startup.log

# i2c-6
echo "Init I2C-6"
if [[ ! -e /sys/class/gpio/gpio27 ]]; then
   echo 27 > /sys/class/gpio/export
fi
echo "mode1" > /sys/kernel/debug/gpio_debug/gpio27/current_pinmux
if [[ ! -e /sys/class/gpio/gpio28 ]]; then
   echo 28 > /sys/class/gpio/export
fi
echo "mode1" > /sys/kernel/debug/gpio_debug/gpio28/current_pinmux
echo "  GPIO27 pinmux: $(cat /sys/kernel/debug/gpio_debug/gpio27/current_pinmux)"
echo "  GPIO28 pinmux: $(cat /sys/kernel/debug/gpio_debug/gpio28/current_pinmux)"


# i2c-1
echo "Init I2C-1"
if [[ ! -e /sys/class/gpio/gpio19 ]]; then
   echo 19 > /sys/class/gpio/export
fi
echo "mode1" > /sys/kernel/debug/gpio_debug/gpio19/current_pinmux
if [[ ! -e /sys/class/gpio/gpio20 ]]; then
   echo 20 > /sys/class/gpio/export
fi
echo "mode1" > /sys/kernel/debug/gpio_debug/gpio20/current_pinmux
echo "  GPIO19 pinmux: $(cat /sys/kernel/debug/gpio_debug/gpio19/current_pinmux)"
echo "  GPIO20 pinmux: $(cat /sys/kernel/debug/gpio_debug/gpio20/current_pinmux)"


echo "Init SPI-5.1"
if [[ ! -e /sys/class/gpio/gpio130 ]]; then
   echo 130 > /sys/class/gpio/export
fi
echo "mode0" > /sys/kernel/debug/gpio_debug/gpio130/current_pinmux
echo "out" > /sys/kernel/debug/gpio_debug/gpio130/current_direction
echo "pulldown" > /sys/kernel/debug/gpio_debug/gpio130/current_pullmode
echo "  GPIO130 pinmux: $(cat /sys/kernel/debug/gpio_debug/gpio130/current_pinmux)"
echo "  GPIO130 direction: $(cat /sys/kernel/debug/gpio_debug/gpio130/current_direction)"
echo "  GPIO130 pullmode: $(cat /sys/kernel/debug/gpio_debug/gpio130/current_pullmode)"

if [[ ! -e /sys/class/gpio/gpio128 ]]; then
   echo 128 > /sys/class/gpio/export
fi
echo "mode0" > /sys/kernel/debug/gpio_debug/gpio128/current_pinmux
echo "out" > /sys/kernel/debug/gpio_debug/gpio128/current_direction
echo "pulldown" > /sys/kernel/debug/gpio_debug/gpio128/current_pullmode
echo "  GPIO128 pinmux: $(cat /sys/kernel/debug/gpio_debug/gpio128/current_pinmux)"
echo "  GPIO128 direction: $(cat /sys/kernel/debug/gpio_debug/gpio128/current_direction)"
echo "  GPIO128 pullmode: $(cat /sys/kernel/debug/gpio_debug/gpio128/current_pullmode)"

if [[ ! -e /sys/class/gpio/gpio109 ]]; then
   echo 109 > /sys/class/gpio/export
fi
echo "mode1" > /sys/kernel/debug/gpio_debug/gpio109/current_pinmux
echo "pulldown" > /sys/kernel/debug/gpio_debug/gpio109/current_pullmode
echo "  GPIO109 pinmux: $(cat /sys/kernel/debug/gpio_debug/gpio109/current_pinmux)"
echo "  GPIO109 direction: $(cat /sys/kernel/debug/gpio_debug/gpio109/current_direction)"
echo "  GPIO109 pullmode: $(cat /sys/kernel/debug/gpio_debug/gpio109/current_pullmode)"

if [[ ! -e /sys/class/gpio/gpio111 ]]; then
   echo 111 > /sys/class/gpio/export
fi
echo "mode1" > /sys/kernel/debug/gpio_debug/gpio111/current_pinmux
echo "pulldown" > /sys/kernel/debug/gpio_debug/gpio111/current_pullmode
echo "  GPIO111 pinmux: $(cat /sys/kernel/debug/gpio_debug/gpio111/current_pinmux)"
echo "  GPIO111 direction: $(cat /sys/kernel/debug/gpio_debug/gpio111/current_direction)"
echo "  GPIO111 pullmode: $(cat /sys/kernel/debug/gpio_debug/gpio111/current_pullmode)"

if [[ ! -e /sys/class/gpio/gpio115 ]]; then
   echo 115 > /sys/class/gpio/export
fi
echo "mode1" > /sys/kernel/debug/gpio_debug/gpio115/current_pinmux
echo "pulldown" > /sys/kernel/debug/gpio_debug/gpio115/current_pullmode
echo "  GPIO115 pinmux: $(cat /sys/kernel/debug/gpio_debug/gpio115/current_pinmux)"
echo "  GPIO115 direction: $(cat /sys/kernel/debug/gpio_debug/gpio115/current_direction)"
echo "  GPIO115 pullmode: $(cat /sys/kernel/debug/gpio_debug/gpio115/current_pullmode)"

echo "Init LEDSTRIP"
if [[ ! -e /sys/class/gpio/gpio15 ]]; then
   echo 15 > /sys/class/gpio/export
fi
echo "mode0" > /sys/kernel/debug/gpio_debug/gpio15/current_pinmux
echo "out" > /sys/kernel/debug/gpio_debug/gpio15/current_direction
echo "pullup" > /sys/kernel/debug/gpio_debug/gpio15/current_pullmode
echo "  GPIO15 pinmux: $(cat /sys/kernel/debug/gpio_debug/gpio15/current_pinmux)"
echo "  GPIO15 direction: $(cat /sys/kernel/debug/gpio_debug/gpio15/current_direction)"
echo "  GPIO15 pullmode: $(cat /sys/kernel/debug/gpio_debug/gpio15/current_pullmode)"

echo "Init MICARRAY"
if [[ ! -e /sys/class/gpio/gpio40 ]]; then
   echo 40 > /sys/class/gpio/export
fi
echo "mode1" > /sys/kernel/debug/gpio_debug/gpio40/current_pinmux
echo "pulldown" > /sys/kernel/debug/gpio_debug/gpio40/current_pullmode
echo "  GPIO40 pinmux: $(cat /sys/kernel/debug/gpio_debug/gpio40/current_pinmux)"
echo "  GPIO40 direction: $(cat /sys/kernel/debug/gpio_debug/gpio40/current_direction)"
echo "  GPIO40 pullmode: $(cat /sys/kernel/debug/gpio_debug/gpio40/current_pullmode)"

if [[ ! -e /sys/class/gpio/gpio41 ]]; then
   echo 41 > /sys/class/gpio/export
fi
echo "mode1" > /sys/kernel/debug/gpio_debug/gpio41/current_pinmux
echo "pulldown" > /sys/kernel/debug/gpio_debug/gpio41/current_pullmode
echo "  GPIO41 pinmux: $(cat /sys/kernel/debug/gpio_debug/gpio41/current_pinmux)"
echo "  GPIO41 direction: $(cat /sys/kernel/debug/gpio_debug/gpio41/current_direction)"
echo "  GPIO41 pullmode: $(cat /sys/kernel/debug/gpio_debug/gpio41/current_pullmode)"

if [[ ! -e /sys/class/gpio/gpio42 ]]; then
   echo 42 > /sys/class/gpio/export
fi
echo "mode1" > /sys/kernel/debug/gpio_debug/gpio42/current_pinmux
echo "pulldown" > /sys/kernel/debug/gpio_debug/gpio42/current_pullmode
echo "  GPIO42 pinmux: $(cat /sys/kernel/debug/gpio_debug/gpio42/current_pinmux)"
echo "  GPIO42 direction: $(cat /sys/kernel/debug/gpio_debug/gpio42/current_direction)"
echo "  GPIO42 pullmode: $(cat /sys/kernel/debug/gpio_debug/gpio42/current_pullmode)"


motors="MTRPAN MTRTILT MTRROTATE MTRFOLD"
motor_wires="IN1 IN2 IN3 IN4"
declare -A motor_gpio_pin=(
   [MTRFOLD-IN1]="182"
   [MTRFOLD-IN2]="129"
   [MTRFOLD-IN3]="131"
   [MTRFOLD-IN4]="15"
   [MTRPAN-IN1]="13"
   [MTRPAN-IN2]="165"
   [MTRPAN-IN3]="12"
   [MTRPAN-IN4]="183"
   [MTRTILT-IN1]="44"
   [MTRTILT-IN2]="46"
   [MTRTILT-IN3]="48"
   [MTRTILT-IN4]="14"
   [MTRROTATE-IN1]="134"
   [MTRROTATE-IN2]="45"
   [MTRROTATE-IN3]="47"
   [MTRROTATE-IN4]="49"
)
declare -A motor_gpio_direction=(
   [MTRFOLD-IN1]="out"
   [MTRFOLD-IN2]="out"
   [MTRFOLD-IN3]="out"
   [MTRFOLD-IN4]="out"
   [MTRPAN-IN1]="out"
   [MTRPAN-IN2]="out"
   [MTRPAN-IN3]="out"
   [MTRPAN-IN4]="out"
   [MTRTILT-IN1]="out"
   [MTRTILT-IN2]="out"
   [MTRTILT-IN3]="out"
   [MTRTILT-IN4]="out"
   [MTRROTATE-IN1]="out"
   [MTRROTATE-IN2]="out"
   [MTRROTATE-IN3]="out"
   [MTRROTATE-IN4]="out"
)
declare -A motor_gpio_mode=(
   [MTRFOLD-IN1]="0"
   [MTRFOLD-IN2]="0"
   [MTRFOLD-IN3]="0"
   [MTRFOLD-IN4]="0"
   [MTRPAN-IN1]="0"
   [MTRPAN-IN2]="0"
   [MTRPAN-IN3]="0"
   [MTRPAN-IN4]="0"
   [MTRTILT-IN1]="0"
   [MTRTILT-IN2]="0"
   [MTRTILT-IN3]="0"
   [MTRTILT-IN4]="0"
   [MTRROTATE-IN1]="0"
   [MTRROTATE-IN2]="0"
   [MTRROTATE-IN3]="0"
   [MTRROTATE-IN4]="0"
)
declare -A motor_gpio_pullmode=(
   [MTRFOLD-IN1]="pullup"
   [MTRFOLD-IN2]="pullup"
   [MTRFOLD-IN3]="pullup"
   [MTRFOLD-IN4]="pullup"
   [MTRPAN-IN1]="pullup"
   [MTRPAN-IN2]="pullup"
   [MTRPAN-IN3]="pullup"
   [MTRPAN-IN4]="pullup"
   [MTRTILT-IN1]="pullup"
   [MTRTILT-IN2]="pullup"
   [MTRTILT-IN3]="pullup"
   [MTRTILT-IN4]="pullup"
   [MTRROTATE-IN1]="pullup"
   [MTRROTATE-IN2]="pullup"
   [MTRROTATE-IN3]="pullup"
   [MTRROTATE-IN4]="pullup"
)

declare -A motor_gpio_pullstrength=(
   [MTRFOLD-IN1]="20k"
   [MTRFOLD-IN2]="20k"
   [MTRFOLD-IN3]="20k"
   [MTRFOLD-IN4]="20k"
   [MTRPAN-IN1]="20k"
   [MTRPAN-IN2]="20k"
   [MTRPAN-IN3]="20k"
   [MTRPAN-IN4]="20k"
   [MTRTILT-IN1]="20k"
   [MTRTILT-IN2]="20k"
   [MTRTILT-IN3]="20k"
   [MTRTILT-IN4]="20k"
   [MTRROTATE-IN1]="20k"
   [MTRROTATE-IN2]="20k"
   [MTRROTATE-IN3]="20k"
   [MTRROTATE-IN4]="20k"
)

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

function init_motors()
{
   set -xv
   for motor in $motors; do
      echo "Init $motor"
      for wire in $motor_wires; do
         init_gpio ${motor_gpio_pin[$motor-$wire]} ${motor_gpio_mode[$motor-$wire]} ${motor_gpio_direction[$motor-$wire]} ${motor_gpio_pullmode[$motor-$wire]} ${motor_gpio_pullstrength[$motor-$wire]}
      done
   done
}

init_motors

echo "Init JOYB"
init_gpio 135 "0" "in" "none" "20k"
