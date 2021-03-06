#!/bin/sh
#_rc.bluetooth: Loads Bluetooth modules and starts associated programs.


die() {
  echo "ERROR: $1" 
  exit 1
}

# Defaults
csrinit="y"
btdev="/dev/bt"
btif="hci0"
btmulaw="0"

echo "">/var/run/bt_status

if test "${hw_bluetooth}" = "0"; then
  echo "* NOT starting Bluetooth " 
  echo "not started">/var/run/bt_status
  exit 0
fi

if test "$1" = "resume"; then
  echo "* Resuming Bluetooth";
elif test "$1" = "suspend"; then
  echo "* Suspending Bluetooth"
  if test "${hw_btusb}" = "0"; then
    hciconfig ${btif} down
    pid=$(pidof hciattach|awk '{print $NF}')
    if [ -n "$pid" ]; then
      kill ${pid}
      wait ${pid}
    fi
  fi
  echo "not started">/var/run/bt_status
  exit 0;
else
  echo "* Starting Bluetooth" 

  if ! sed "s/@DEVNAME@/${hw_btname}/;s/@DEVCLASS@/${hw_btclass}/" /etc/bluetooth/hcid.conf.template > /etc/bluetooth/hcid.conf; then
    die "unable to setup hcid config file"
  fi

  if ! /media/sdcard/tomplayer/hcid; then
    die "hcid failed"
  fi

  if ! sdpd; then
    die "sdpd failed"
  fi
fi

if test "${hw_btusb}" != "0"; then
  if ! insmod ohci-hcd.ko; then
    die "insmod ohci-hcd failed" 
  fi
  # let usb stabilise ?
  sleep 1
fi

if test "${csrinit}" = "y"; then
  if test "${hw_btusb}" = "0"; then
    if ! csrinit bcsp ${btdev} ${hw_btspeed} ${hw_btclock}; then
      die "csrinit failed"
    fi
  elif ! csrinit usb ${btdev} ${hw_btspeed} ${hw_btclock}; then
    echo "csrinit usb failed, trying bcsp"
    if ! csrinit bcsp ${btdev} ${hw_btspeed} ${hw_btclock}; then
      die "csrinit bcsp failed too"
    fi
  fi
fi

if test "${hw_btusb}" = "0" && ! hciattach /dev/bt bcsp ${hw_btspeed}; then
  die "hciattach failed"
fi

if ! hciconfig ${btif} up; then
  die "hciconfig failed"
fi

#Launch hidd server to handle Remote controler
/bin/hidd --server
# -n> /media/sdcard/tomplayer/bthid.txt  2>&1 & 

echo "started">/var/run/bt_status

# EOF
