#!/bin/bash

get_gpio_state()
{
   local gpionum=$1; local mode0label=$2 local mode1label=$3

   pinmode="$(cat /sys/kernel/debug/gpio_debug/gpio${gpionum}/current_pinmux)"
   if [[ "$pinmode" == "mode1" ]]; then
      echo "  mode            : $mode1label"
   elif [[ "$pinmode" == "mode0" ]]; then
      echo "  mode            : $mode0label"
   else
      echo "  mode            : Unknown <$pinmode>"
   fi
   echo "  direction       : "$(cat /sys/kernel/debug/gpio_debug/gpio${gpionum}/current_direction)
   echo "  opendrain enable: "$(cat /sys/kernel/debug/gpio_debug/gpio${gpionum}/current_opendrain)
   echo "  pullup mode     : "$(cat /sys/kernel/debug/gpio_debug/gpio${gpionum}/current_pullmode)
   echo "  pullup size     : "$(cat /sys/kernel/debug/gpio_debug/gpio${gpionum}/current_pullstrength)
   echo "  state           : "$(cat /sys/kernel/debug/gpio_debug/gpio${gpionum}/current_value)
}

# i2c-1
echo "BODYIMU-SCL <--  LVL33R-2 <--  I2C1-SCL [J18-6]:"
get_gpio_state 19 GP19 I2C1-SCL
echo "       -SDA <--> LVL33R-1 <--  I2C1-SDA [J17-8]"
get_gpio_state 20 GP20 I2C1-SDA

echo " "
echo "ADC33-SCL <--> LVL33R-2 <--> I2C1-SCL [J18-6]):"
get_gpio_state 19 GP19 I2C1-SCL
echo "     -SDA <--> LVL33R-1 <--> I2C1-SCL [J17-8]:"
get_gpio_state 19 GP19 I2C1-SDA

echo " "
echo "JOY-SCL <--> ADC33-SCL <--> LVL33R-2 <--> I2C1-SCL [J18-6]:"
get_gpio_state 19 GP19 I2C1-SCL
echo "JOYX-SDA -->  ADC33-1 --> ADC33-SDA <--> LVL33R-1 <--> I2C1-SDA [J17-8]:"
echo "JOYY-SDA -->  ADC33-2 --> ADC33-SDA <--> LVL33R-1 <--> I2C1-SDA [J17-8]:"
echo "JOYZ-SDA -->  ADC33-3 --> ADC33-SDA <--> LVL33R-1 <--> I2C1-SDA [J17-8]:"
get_gpio_state 20 GP20 I2C1-SDA

echo " "
# i2c-6
echo "HEADIMU-SCL <--  LVL33L-6 <--  I2C6-SCL [J17-7]:"
get_gpio_state 27 GP27 I2C6-SCL
echo "       -SDA <--> LVL33L-5 <--> I2C6-SDA [J18-9]:"
get_gpio_state 28 GP28 I2C6-SDA

echo " "
# spi51-MOSI_TXD
echo "OLED-DIN <-- LVL33L-1  <-- SSP5-TXD [J17-12]:"
get_gpio_state 115 GP115 SSP5-TXD
echo "    -CLK <-- LVL33L-2 <-- SSP5-CLK [J17-11]:"
get_gpio_state 109 GP109 SSP5-CLK
echo "    -    <--          <-- SSP5-FS0 [J18-10]:"
get_gpio_state 110 GP110 SSP5-FS0
echo "    -CS  <-- LVL33L-5 <-- SSP5-FS1 [J17-10]:"
get_gpio_state 111 GP111 SSP5-FS1
echo "    -DC  <-- LVL33L-3 <-- GP128 [J17-14]:"
get_gpio_state 128 GP128 SSP5-CLK
echo "    -RST <-- LVL33L-3 <-- GP130 [J18-13]:"
get_gpio_state 130 GP130 SSP5-TXD


echo " "
echo "LEDSTRIP --> GP15 [J20-7]:"
get_gpio_state 15 GP15 GP15

echo " "
echo "MICARRAY-SCK <-- I2S2-CLK [J19-10]:"
get_gpio_state 40 GP40 I2S2-CLK
echo "        -SD <-- I2S2-RXD [J20-9]:"
get_gpio_state 42 GP42 I2S2-RXD
echo "        -WS <-- I2S2-FS [J20-10]:"
get_gpio_state 41 GP41 I2S2-FS


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
declare -A motor_gpio_mode1=(
   [MTRFOLD-IN1]="PWM2"
   [MTRFOLD-IN2]="UART1-RTS"
   [MTRFOLD-IN3]="UART1-TX"
   [MTRFOLD-IN4]="GP15"
   [MTRPAN-IN1]="PWM1"
   [MTRPAN-IN2]="GP165"
   [MTRPAN-IN3]="PWM0"
   [MTRPAN-IN4]="PWM3"
   [MTRTILT-IN1]="GP44"
   [MTRTILT-IN2]="GP46"
   [MTRTILT-IN3]="GP48"
   [MTRTILT-IN4]="GP14"
   [MTRROTATE-IN1]="UART2-RX"
   [MTRROTATE-IN2]="GP45"
   [MTRROTATE-IN3]="GP47"
   [MTRROTATE-IN4]="GP49"
)

function check_motors() {
   local motor_names=($motors)
   local names_index=0
   local print_full_header="false"
   local prefix_len
   for motor in $motors; do
      echo " "
      print_full_header="true"
      for wire in $motor_wires; do
         if [[ "$print_full_header" == "false" ]]; then
            prefix_len=${#motor_names[$names_index]}
            echo "-$wire --> GP${motor_gpio_pin[$motor-$wire]}" | pr -T -o $prefix_len
         else
            echo "$motor-$wire --> GP${motor_gpio_pin[$motor-$wire]}"
            print_full_header="false"
         fi
         get_gpio_state ${motor_gpio_pin[$motor-$wire]} GP${motor_gpio_pin[$motor-$wire]} ${motor_gpio_mode1[$motor-$wire]}
      done
      names_index=$((names_index+1))
   done
}
check_motors

echo " "
echo "JOYB <-- GP135 [J17-5]:"
get_gpio_state 135 GP135 UART2-TX



