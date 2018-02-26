#! /vendor/bin/sh

wlan_driver_version=`cat /sys/kernel/wifi/wlan/driver_version`
setprop sys.wlan.driver.version "$wlan_driver_version"
wlan_fw_version=`cat /sys/kernel/wifi/wlan/fw/0/version`
setprop sys.wlan.firmware.version "$wlan_fw_version"
