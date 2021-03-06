#!/bin/sh

#Entering tomplayer directory installation
TOMPLAYER_DIR=`echo $0 | sed 's/tomplayergui.sh//'` 
cd $TOMPLAYER_DIR

#To be sure to get absolute path
TOMPLAYER_DIR=`pwd`
export LIB_TOMPLAYER=${TOMPLAYER_DIR}/lib
export CONF_TOMPLAYER=${TOMPLAYER_DIR}/conf

# Remote_inputs is required for Bluetooth remote to work...
# For now disable on non carminat TT as it hangs on exit...
#killall -9 remote_inputs
#./remote_inputs &

#Set timezone : GMT+1 by default as most tomplayer users are in this TZ
# Note that the setting below is daylight saving active...
export TZ=CEST-2


echo "Killing ttn..."
killall -9 mplayer
killall -9 ttn
IS_TTN_NOT_RUNNING=$?
killall -9 mp3d 
killall -9 a2dpd
rm /mnt/sdcard/dirty_fs 
/etc/rc.usbkill 
killall -9 clmapp
killall -9 ttsserver 
killall -9 suntime 


killall -9 refresh_wdg 
./refresh_wdg &

# Call script that creates symlinks for shared librairies
./create_symlinks.sh
ln -sf  $TOMPLAYER_DIR/res/font/decker.ttf /tmp/

#convert in UNIX text format the used configuration file
cp -f conf/tomplaye.ini /tmp/tomplaye.ini
dos2unix /tmp/tomplaye.ini

# Test whether /dev/ts is handled by a driver or not
echo -n "" >> /dev/ts
RES=$?
if [ $RES -ne 0 ] ; then
NO_TS=1
else
NO_TS=0
fi 


# Useful env vars for DirectFB and Tslib
export FRAMEBUFFER=/dev/fb
export TSLIB_CONSOLEDEVICE=none
export TSLIB_FBDEVICE=/dev/fb
export TSLIB_CONFFILE=/etc/ts.conf
export TSLIB_PLUGINDIR=/usr/local/lib/ts
export LD_LIBRARY_PATH=/usr/local/lib


# Adapt to whether /dev/ts or /dev/input/event0 as touchscreen inputs
if [ $NO_TS -eq 0 ] ; then 
  export TSLIB_TSDEVICE=/dev/ts
  # Copy tslib configuration file
  cd /etc
  ln -sf ${CONF_TOMPLAYER}/ts.conf ts.conf
  cd $TOMPLAYER_DIR
else
  # For Linux inputs device we need to calibrate !
  export TSLIB_TSDEVICE=/dev/input/event0
  export TSLIB_CALIBFILE=${CONF_TOMPLAYER}/pointercal
  # Copy tslib configuration file
  cd /etc
  ln -sf ${CONF_TOMPLAYER}/ts_input.conf ts.conf
  cd $TOMPLAYER_DIR
  if [ ! -f ${CONF_TOMPLAYER}/pointercal ] ; then
    ./ts_calibrate
  fi
fi


#sh ./dump_proc.sh > log.txt

# Tomplayer / start_engine loop 
END_ASKED=0
NO_SPLASH=
FIRST_LAUNCH="--first-launch"
while [  $END_ASKED -eq 0 ]; do 
rm /tmp/start_engine.sh
./tomplayer --dfb:no-vt $NO_SPLASH $FIRST_LAUNCH
RES_TOMPLAYER=$?
FIRST_LAUNCH=
sync
./splash_screen res/background/wait
if [ -f /tmp/start_engine.sh ] ; then 
  /bin/sh /tmp/start_engine.sh
  NO_SPLASH="--no-splash"
  killall -9 mplayer
else 
  END_ASKED=1
fi
done


#If power button has been pushed then power off TOMTOM
if [ $RES_TOMPLAYER -eq 51 ] ; then 
if [ $NO_TS -eq 0 ] ; then 
#Does not work on nav>8.3
  ./turn_off
fi
fi

# Launch navigation software if needed
if [ $IS_TTN_NOT_RUNNING -eq 0 ] ; then
if [ $NO_TS -eq 0 ] ; then 
#Directly relaunch nav software
echo "Start TTN"
/etc/rc.restartgps
/bin/ttn
else 
#Reboot to relaunch the nav software
sync
killall refresh_wdg
./force_reboot
fi
fi