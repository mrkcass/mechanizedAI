#!/bin/bash

SOMAX_DIR="/home/mcass/somaxdev"

SOMAX_USB_IP="192.168.2.15"
SOMAX_WIFI_IP="192.168.1.108"
SOMAX_USER="root"
SOMAX_PASS="somax"

#sshfs remote file system mount to local directory
SSHFS_REMOTE="/root"
SSHFS_LOCAL="$SOMAX_DIR/somaxfs"

SOMAX_BACKUP="$SOMAX_DIR/somax_backup"

EXP_CMD="$SOMAX_DIR/somax/tools/somax_cmd.exp"

wifi=""
somax_ip=$SOMAX_USB_IP

if [[ "$1" == "" || "$1" == "help" || "$1" == "--help" || "$1" == "-h" ]]; then
   echo ""
   echo "somaxdev.sh"
   echo ""
   echo "version: 1.0"
   echo ""
   echo "Script to execute common development functions"
   echo " "
   echo "Options"
   echo "  wifi - use the wifi ip address instead of the usb ip address"
   echo ""
   echo ""
   echo "Commands"
   echo "  * setup - setup environment variables to make this command usable"
   echo "       without providing a path. Must be sourced by doing calling "
   echo "       this script as such: source somaxdev.sh setup"
   echo "  * sshfs - mount somax directory $SSHFS_REMOTE for read/write to:"
   echo "       $SSHFS_LOCAL"
   echo "       arguments:"
   echo "         up   - mount the file system"
   echo "         down - unmount the file system"
   echo "         clean - umount if mounted then remove any files stale files"
   echo " "
   echo " "
   $EXP_CMD help
   exit 0
fi

if [[ "$1" == "setup" ]]; then
   PATH="$PATH:$SOMAX_DIR/somax/tools"
fi

if [[ "$1" == "wifi" ]]; then
   wifi="wifi"
   soamx_ip=$SOMAX_WIFI_IP
   shift
fi

if [[ $1 == "sshfs" ]]; then
   if [[ ! -e $SSHFS_LOCAL ]]; then
      mkdir -p $SSHFS_LOCAL
   fi
   if [[ "$2" == "up" ]]; then
      $EXP_CMD sshfs $SOMAX_USER $somax_ip $SSHFS_REMOTE $SSHFS_LOCAL $SOMAX_PASS
   elif [[ "$2" == "down" ]]; then
      fusermount -u $SSHFS_LOCAL
   elif [[ "$2" == "clean" ]]; then
      fusermount -u $SSHFS_LOCAL
      rm -rf $SSHFS_LOCAL/*
   elif [[ "$2" == "backup" ]]; then
      arch_name="somaxfs-$(date +%Y-%m-%d-%H_%M)-$HOSTNAME.tar"
      find $SSHFS_LOCAL -name "*.c"                   \
            -o -name "*.cpp"                          \
            -o -name "*.h"                            \
            -o -name "*.sh"                           \
            -o -name "*.exp"                          \
            -o -name "*.md"                           \
            -o -name "*.service"                      \
            -o -name "Makefile*"                      \
            -o -name "*.calibration"                  \
            | tar --no-recursion -cf $SOMAX_BACKUP/$arch_name -T -

      gzip $SOMAX_BACKUP/$arch_name
      arch_name+=".gz"
      echo " "
      echo "   Created Archive: $arch_name"
      echo " "
   fi
else
   $EXP_CMD $wifi $1 $2 $3 $4 $5 $6
fi
