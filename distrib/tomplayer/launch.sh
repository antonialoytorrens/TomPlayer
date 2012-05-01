LOG=/dev/null
#LOG=/media/sdcard/tomplayer/testbt.txt

# refresh for some time to be able to log navcore launch 
/media/sdcard/tomplayer/rwdg.sh &


if [ -f /media/sdcard/tomplayer/boot.txt ] ; then

/media/sdcard/tomplayer/rc.bluetooth
/media/sdcard/tomplayer/tomplayergui.sh
TOM_EXIT=$?
if [ $TOM_EXIT -ne 51 ] ; then
rm -f /media/sdcard/tomplayer/boot.txt
sync
fi
killall -9 refresh_wdg

else

start_ttn
/media/sdcard/tomplayer/waitkey.sh &
/etc/rc.bluetooth 
/media/sdcard/tomplayer/logbt.sh &

fi

killall -9 rwdg.sh
