#!/bin/bash

#------------------------------------------------------------------------------
#------------------------------------------------------------------------------
# author: mark cass
# project: somax personal AI
# project url: https://mechanizedai.com
# license: open source and free for all uses without encumbrance.
#
# FILE: inputsource.h
# DESCRIPTION: Initalize tcpip communication.
#------------------------------------------------------------------------------
#------------------------------------------------------------------------------

wifi_networks=""

wpa_cli -i wlan0 scan
sleep 1
wifi_networks="$(wpa_cli -i wlan0 scan_results)"

if [[ "$1" == "wifi-networks" ]]; then
   echo "$wifi_networks" | grep ""
fi


