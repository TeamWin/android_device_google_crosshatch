#!/vendor/bin/sh

# Init starts this script in the factory, to trigger shutdown after
# setupwizard has started, and USB is removed.

# wait for system to settle down after sys.boot_complete
sleep 5

# Wait for USB to be removed, debounce it three times in case it's enumerating still.
disconnect_count=0
while [ disconnect_count -le 3 ]; do
    type=`cat /sys/class/power_supply/usb/typec_mode`
    if [ "x$type" == "xNothing attached" ]; then
        (( disconnect_count++ ))
        echo disconnect count is $disconnect_count
    else
        # Restart the debounce count
        disconnect_count=0
    fi
    sleep 1
done

# Shutdown
setprop sys.powerctl reboot,packout
