#!/bin/sh
echo "Killing ttn..."

killall -9 mplayer
killall -9 ttn
killall -9 mp3d 
killall -9 a2dpd

rm /mnt/sdcard/dirty_fs 

/etc/rc.usbkill 

killall -9 clmapp
killall -9 ttsserver 
killall -9 suntime 

echo "Running the application"

#Entering tomplayer directory installation
TOMPLAYER_DIR=`dirname $0`
cd $TOMPLAYER_DIR

#To be sure to get absolute path
TOMPLAYER_DIR=`pwd`

#Create a symbolic link to enable generic minigui configuration file
ln -sf ${TOMPLAYER_DIR}/res /etc/res

export FRAMEBUFFER=/dev/fb
./tomplayer

ttn_file=/bin/ttn

ttn_cnt=`ps | grep ttn | wc -l`

if test ${ttn_cnt} -lt 2 ; then 


echo "Start TTN"
/etc/rc.restartgps
${ttn_file}
fi
