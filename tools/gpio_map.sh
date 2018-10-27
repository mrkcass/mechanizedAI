#!/bin/bash

somax_pin_map_p3="
J17-1   GP182         PWM2           MTRROTATE-IN1
J17-2   NC
J17-3   NC
J17-4   VIN           -              PSU12V-VOUT
J17-5   UART2-TX      -
J17-6   RCVR-MODE     -
J17-7   GP27          I2C6-SCL       LVL33B-7 IMUCAMD THERMCAM
J17-8   GP20          I2C1-SDA       LVL33A-8 IMUFRAME LVL50A-2 ADC50 JOY3D-BXYZ LIDARCAM
J17-9   GP28          I2C6-SDA       LVL33B-6 IMUCAMD THERMCAM
J17-10  GP111         SPI5-CS1       LVL33B-5 OLEDFRAME
J17-11  GP109         SPI5-CLK       LVL33B-2 OLEDFRAME OLEDCAMD
J17-12  GP115         SPI5-TX        LVL33B-1 OLEDFRAME OLEDCAMD
J17-13  OSCCLK-OUT    -
J17-14  GP128         UART1-CTS      LVL33B-4 SPI-DC OLEDFRAME OLEDCAMD

J18-1   GP13          PWM1           MTRPAN-IN1
J18-2   GP165         -              MTRPAN-IN2
J18-3   PWRBTN        -
J18-4   MSIC-SLPCLK   -
J18-5   VBAT-BKUP     -
J18-6   GP19          I2C1-SCL       LVL33A-7 IMUFRAME LVL50A-1 ADC50 JOY3D-BXYZ LIDARCAM
J18-7   GP12          PWM0           MTRPAN-IN3
J18-8   GP183         PWM3           MTRPAN-IN4
J18-9   NC            -
J18-10  GP110         SPI5-FS0       OLEDCAMD
J18-11  GP114         SPI5-RXD       MTRROTATE-IN2
J18-12  GP129         UART1-RTS      MTRROTATE-IN3
J18-13  GP130         UART1-RX       LVL33A-3 OLEDFRAME-RST
J18-14  FW-RCVR       -

J19-1   NC            -
J19-2   V1P80         -              LVL33A LVL33B LVL50
J19-3   GND           -              PSU12V-GND
J19-4   GP44          -              MTRTILTU-IN1
J19-5   GP46          -              MTRTILTU-IN2
J19-6   GP48          -              MTRTILTU-IN3
J19-7   RESET         -
J19-8   GP131         UART1-TX       MTRROTATE-IN4
J19-9   GP14          -              MTRTILTU-IN4
J19-10  GP40          I2S2-CLK       MICARRAY-SCK
J19-11  GP43          I2S2-TXD       MICARRAY-TXD
J19-12  GP77          SD-CDN         LIDARCAM-XSHUT
J19-13  GP82          SD-DAT2        LIDARCAM-INT
J19-14  GP83          SD-DAT3

J20-1   VSYS          -
J20-2   V3P30         -
J20-3   UART2-RX      -
J20-4   GP45          -              MTRTILTL-IN2
J20-5   GP47          -              MTRTILTL-IN3
J20-6   GP49          -              MTRTILTL-IN4
J20-7   GP15          -
J20-8   GP84          SD-CLK-FB      MTRTILTL-IN1
J20-9   GP42          I2S2-RXD       MICARRAY-SD
J20-10  GP41          I2S2-FS        MICARRAY-WS
J20-11  GP78          SD-CLK
J20-12  GP79          SD-CMD
J20-13  GP80          SD-DAT0
J20-14  GP81          SD-DAT1
"

get_gpio_state()
{
   local gpionum=$1; local mode0label=$2; local mode1label=$3; local somaxlabels="$4"

   pinmode="$(cat /sys/kernel/debug/gpio_debug/gpio${gpionum}/current_pinmux)"
   if [[ "$pinmode" == "mode1" ]]; then
      #echo -n "  mode: $mode1label"
      printf "  %12s" $mode1label
   elif [[ "$pinmode" == "mode0" ]]; then
      #echo -n "  mode: $mode0label"
      printf "  %12s" $mode0label
   else
      echo -n " Unknown <$pinmode>"
      printf "%12s" "unknown"
   fi
   printf "  %3s" "$(cat /sys/kernel/debug/gpio_debug/gpio${gpionum}/current_direction)"
   printf "  %7s" "$(cat /sys/kernel/debug/gpio_debug/gpio${gpionum}/current_opendrain)"
   printf "  %8s" "$(cat /sys/kernel/debug/gpio_debug/gpio${gpionum}/current_pullmode)"
   printf "  %7s" "$(cat /sys/kernel/debug/gpio_debug/gpio${gpionum}/current_pullstrength)"
   printf "  %4s" "$(cat /sys/kernel/debug/gpio_debug/gpio${gpionum}/current_value)"
   printf "  %s\n" "$somaxlabels"
}

echo " "
echo "SOMAX PROTO 3 GPIO MAPPING"
echo " "
#cho "   J17-1         GP182  out   disable   pulldown  910ohms  low
echo " PIN   GPIO      Direction  Opne-drain  pullmode  pullstrength state"
echo "--------------------------------------------------------------------------"
while read -r line; do
   if [[ "$line" == "" ]]; then
      echo " "
      continue
   fi
   pin="$(echo "$line" | awk '{print $1;}')"
   mode0="$(echo "$line" | awk '{print $2;}')"
   gpnum="$(echo "$line" | grep -oE "GP[0-9]{1,3}" | grep -oE "[0-9]+")"
   mode1="$(echo "$line" | awk '{print $3;}')"
   connections="$(echo "$line" | awk '{$1=$2=$3=""; print $0}' | grep -oE '[^ ].*')"

   gpiolabel="$(printf "GPIO%d" $gpnum)"
   printf "%6s %7s" $pin $gpiolabel
   if [[ "$gpnum" != "" ]]; then
      get_gpio_state $gpnum "$mode0" "$mode1" "$connections"
   else
      printf "  %12s\n" $mode0
   fi
done <<< "$somax_pin_map_p3"
