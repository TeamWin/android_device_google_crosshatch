#!/vendor/bin/sh

# Init starts this script in the factory, to trigger shutdown after
# setupwizard has started, and USB is removed.

# wait for system to settle down after sys.boot_complete
sleep 5

# Wait for USB to be removed.
type=`cat /sys/class/power_supply/usb/typec_mode`
while [ "x$type" != "xNothing attached" ]; do
    echo Wait for \"$type\" == \"Nothing attached\"
    sleep 1;
    type=`cat /sys/class/power_supply/usb/typec_mode`
done;

# Shutdown
setprop sys.powerctl shutdown
