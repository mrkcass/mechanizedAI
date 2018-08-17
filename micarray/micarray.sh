#!/bin/bash

if [[ "$1" == "help" || "$1" == "" ]]; then
   echo " "
fi

if [[ "$1" == "pulse" && "$2" == "cards" ]]; then
   # pulse audio change the alsa profile. card
   pactl list cards
elif [[  "$1" == "help" || "$1" == "" ]]; then
   echo "   pulse cards"
   echo "      list pulse audio sound cards"
fi

if [[ "$1" == "pulse" && "$2" == "inputs" ]]; then
   # pulse audio change the alsa profile. card
   pactl list sources
elif [[  "$1" == "help" || "$1" == "" ]]; then
   echo "   pulse inputs"
   echo "      list pulse audio inputs"
fi

if [[ "$1" == "pulse" && "$2" == "input" && "$3" != "" ]]; then
   # pulse audio change the alsa profile. card
   if [[ "$3" == "stereo" ]]; then
      pactl set-card-profile 0 input:analog-stereo
   elif [[ "$3" == "mono" ]]; then
      pactl set-card-profile 0 input:analog-mono
   else
      echo " "
      echo "  Unknown input: $3"
      echo " "
   fi
elif [[  "$1" == "help" || "$1" == "" ]]; then
   echo "   pulse input [stereo mono]"
   echo "      set pulse input profile to micarray"
fi

if [[ "$1" == "pulse" && "$2" == "record" && "$3" != "" ]]; then
   #pulse audio record
   pacat -r -v --volume=65536 --rate=48000  --channels=$3 --format=s24-32le -d alsa_input.platform-merr_dpcm_dummy.0.analog-mono
elif [[  "$1" == "help" || "$1" == "" ]]; then
   echo "   pulse record [num channels]"
   echo "      record mic array audio channels"
fi

if [[ "$1" == "sst" && "$2" == "enabled" ]]; then
   #pulse audio record
   cat /sys/kernel/debug/asoc/dummy-audio/sst-platform/dapm/* | grep ": On" | column
elif [[  "$1" == "help" || "$1" == "" ]]; then
   echo "   sst   enabled"
   echo "      check sysfs for active sst dapm devices"
fi

if [[ "$1" == "sst" && "$2" == "disabled" ]]; then
   #pulse audio record
   cat /sys/kernel/debug/asoc/dummy-audio/sst-platform/dapm/* | grep ": Off" | column
elif [[  "$1" == "help" || "$1" == "" ]]; then
   echo "   sst   disabled"
   echo "      check sysfs for inactive sst dapm devices"
fi

if [[ "$1" == "dummy" && "$2" == "enabled" ]]; then
   #pulse audio record
   cat /sys/kernel/debug/asoc/dummy-audio/snd-soc-dummy/dapm/* | grep ": On" | column
elif [[  "$1" == "help" || "$1" == "" ]]; then
   echo "   dummy   enabled"
   echo "      check sysfs for active soc-dummy dapm devices"
fi

if [[ "$1" == "dummy" && "$2" == "disabled" ]]; then
   #pulse audio record
   cat /sys/kernel/debug/asoc/dummy-audio/snd-soc-dummy/dapm/* | grep ": Off" | column
elif [[  "$1" == "help" || "$1" == "" ]]; then
   echo "   dummy   disabled"
   echo "      check sysfs for inactive soc-dummy dapm devices"
fi

if [[ "$1" == "sst" && "$2" == "config" ]]; then
   #pulse audio record
   cat /sys/bus/pci/devices/0000\:00\:0d.0/config | od -x
elif [[  "$1" == "help" || "$1" == "" ]]; then
   echo "   sst   config"
   echo "      check sysfs for sst config"
fi

if [[ "$1" == "sst" && "$2" == "dais" ]]; then
   #pulse audio record
   cat /sys/kernel/debug/asoc/dais
elif [[  "$1" == "help" || "$1" == "" ]]; then
   echo "   sst   dais"
   echo "      check sysfs for sst digital audio interfaces"
fi

if [[ "$1" == "alsa" && "$2" == "scontrols" ]]; then
   amixer -c 1 scontrols
elif [[  "$1" == "help" || "$1" == "" ]]; then
   echo "   alsa  scontrols"
   echo "      alsa mixer mic array basic configuration"
fi

if [[ "$1" == "alsa" && "$2" == "controls" ]]; then
   amixer -c 1 controls
elif [[  "$1" == "help" || "$1" == "" ]]; then
   echo "   alsa  controls"
   echo "      alsa mixer mic array configuration"
fi

if [[ "$1" == "alsa" && "$2" == "input" && "$3" != "" ]]; then
   # pulse audio change the alsa profile. card
   if [[ "$3" == "0" ]]; then
      amixer -c 1 cget name='codec_in deinterleaver codec_in0_0' 'slot 0'
      amixer -c 1 cget name='codec_in deinterleaver codec_in0_1' 'none'
      amixer -c 1 cget name='codec_in deinterleaver codec_in1_0' 'none'
      amixer -c 1 cget name='codec_in deinterleaver codec_in1_1' 'none'
   elif [[ "$3" == "1" ]]; then
      amixer -q -c 1 cset name='codec_in deinterleaver codec_in0_0' 'slot 0'
      amixer -q -c 1 cset name='codec_in deinterleaver codec_in0_1' 'none'
      amixer -q -c 1 cset name='codec_in deinterleaver codec_in1_0' 'none'
      amixer -q -c 1 cset name='codec_in deinterleaver codec_in1_1' 'none'
   elif [[ "$3" == "2" ]]; then
      amixer -q -c 1 cset name='codec_in deinterleaver codec_in0_0' 'slot 0'
      amixer -q -c 1 cset name='codec_in deinterleaver codec_in0_1' 'slot 1'
      amixer -q -c 1 cset name='codec_in deinterleaver codec_in1_0' 'none'
      amixer -q -c 1 cset name='codec_in deinterleaver codec_in1_1' 'none'
   elif [[ "$3" == "3" ]]; then
      amixer -q -c 1 cset name='codec_in deinterleaver codec_in0_0' 'slot 0'
      amixer -q -c 1 cset name='codec_in deinterleaver codec_in0_1' 'slot 1'
      amixer -q -c 1 cset name='codec_in deinterleaver codec_in1_0' 'slot 2'
      amixer -q -c 1 cset name='codec_in deinterleaver codec_in1_1' 'none'
   elif [[ "$3" == "4" ]]; then
      amixer -q -c 1 cset name='codec_in deinterleaver codec_in0_0' 'slot 0'
      amixer -q -c 1 cset name='codec_in deinterleaver codec_in0_1' 'slot 1'
      amixer -q -c 1 cset name='codec_in deinterleaver codec_in1_0' 'slot 2'
      amixer -q -c 1 cset name='codec_in deinterleaver codec_in1_1' 'slot 3'
   else
      echo " "
      echo "  Invalid num channels: $3"
      echo " "
   fi
elif [[  "$1" == "help" || "$1" == "" ]]; then
   echo "   alsa input [1 2 3 4]"
   echo "      configure alsa mixer for micarray TDM input on 1,2,3 or 4 channels"
fi

if [[ "$1" == "alsa" && "$2" == "volume" && "$3" != "" ]]; then
   # pulse audio change the alsa profile. card
   if [[ "$3" == "all" ]]; then
      amixer -c 1 cget name='codec_in0 dcr 0 params'
      amixer -c 1 cget name='codec_in0 gain 0 mute'
      amixer -c 1 cget name='codec_in0 gain 0 rampduration'
      amixer -c 1 cget name='codec_in0 gain 0 volume'
      amixer -c 1 cget name='codec_in1 dcr 0 params'
      amixer -c 1 cget name='codec_in1 gain 0 mute'
      amixer -c 1 cget name='codec_in1 gain 0 rampduration'
      amixer -c 1 cget name='codec_in1 gain 0 volume'

      amixer -c 1 cget name='pcm1_out mix 0 codec_in0'
      amixer -c 1 cget name='pcm1_out gain 0 volume'
      amixer -c 1 cget name='pcm1_out gain 0 rampduration'
      amixer -c 1 cget name='pcm1_out gain 0 mute'

   else
      echo " "
      echo "  Invalid num channels: $3"
      echo " "
   fi
elif [[  "$1" == "help" || "$1" == "" ]]; then
   echo "   alsa volume"
   echo "      configure alsa mixer volumes for micarray"
fi

if [[ "$1" == "alsa" && "$2" == "record" && "$3" != "" ]]; then
   #pulse audio record
   amixer -q -c 1 cset name='codec_in0 gain 0 volume' 300,300
   amixer -q -c 1 cset name='codec_in0 gain 0 rampduration' 50
   amixer -q -c 1 cset name='codec_in0 gain 0 mute' 0
   amixer -q -c 1 cset name='codec_in1 gain 0 volume' 300,300
   amixer -q -c 1 cset name='codec_in1 gain 0 rampduration' 50
   amixer -q -c 1 cset name='codec_in1 gain 0 mute' 0

   amixer -c 1 cset name='pcm0_in gain 0 volume' 0,0
   amixer -c 1 cset name='pcm0_in gain 0 rampduration' 0
   amixer -c 1 cset name='pcm0_in gain 0 mute' 1
   amixer -c 1 cset name='pcm0_out mix 0 codec_in0' 0
   amixer -c 1 cset name='pcm0_out mix 0 codec_in1' 0

   amixer -c 1 cset name='pcm1_in gain 0 volume' 300,300
   amixer -c 1 cset name='pcm1_in gain 0 rampduration' 50
   amixer -c 1 cset name='pcm1_in gain 0 mute' 1
   amixer -c 1 cset name='pcm1_out mix 0 codec_in0' 1
   amixer -c 1 cset name='pcm1_out mix 0 codec_in1' 0
   amixer -c 1 cset name='pcm1_out gain 0 volume' 300,300
   amixer -c 1 cset name='pcm1_out gain 0 rampduration' 50
   amixer -c 1 cset name='pcm1_out gain 0 mute' 0


   arecord -t wav -D hw:1=dummyaudio,0 -r 48000 -c 1 -vvv -M -f S16_LE $3
elif [[  "$1" == "help" || "$1" == "" ]]; then
   echo "   alsa  record [output file name]"
   echo "      record mic audio using alsa"
fi




if [[ "$1" == "help" || "$1" == "" ]]; then
   echo " "
fi


# Recording WAVE 'test.wav' : Signed 16 bit Little Endian, Rate 48000 Hz, Mono
# arecord: set_params:1305: Unable to install hw params:
# ACCESS:  MMAP_INTERLEAVED
# FORMAT:  S16_LE
# SUBFORMAT:  STD
# SAMPLE_BITS: 16
# FRAME_BITS: 16
# CHANNELS: 1
# RATE: 48000
# PERIOD_TIME: 125000
# PERIOD_SIZE: 6000
# PERIOD_BYTES: 12000
# PERIODS: 4
# BUFFER_TIME: 500000
# BUFFER_SIZE: 24000
# BUFFER_BYTES: 48000
# TICK_TIME: 0
