#!/bin/bash

cp somax.service /etc/systemd/system
systemctl enable somax.service

if [[ ! -e oled/oled-ssd1351 ]]; then
   cd oled
   make clean
   make
   cd ..
fi
if [[ ! -e /etc/systemd/system/somax-oled.service ]]; then
   cp oled/somax-oled.service /etc/systemd/system
fi
systemctl enable somax-oled.service


if [[ ! -e ledstrip/ledstrip-ws2812b ]]; then
   cd ledstrip
   make clean
   make
   cd ..
fi
if [[ ! -e /etc/systemd/system/somax-ledstrip.service ]]; then
   cp ledstrip/somax-ledstrip.service /etc/systemd/system
fi
systemctl enable somax-ledstrip.service

pkgupdate="src/gz all http://repo.opkg.net/edison/repo/all
src/gz edison http://repo.opkg.net/edison/repo/edison
src/gz core2-32 http://repo.opkg.net/edison/repo/core2-32"

#setup software packge download
#to initialize: opkg update
#to install package: opkg install <package name>
echo $pkgupdate > /etc/opkg/base-feeds.conf

# micarray -------------------
#alsa libraries
opkg install alsa-lib
opkg install alsa-lib-dev
opkg install alsa-lib-dbg