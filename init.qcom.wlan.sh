#! /vendor/bin/sh

wlan_driver_version=`cat /sys/kernel/wifi/wlan/driver_version`
setprop vendor.wlan.driver.version "$wlan_driver_version"
wlan_fw_version=`cat /sys/kernel/wifi/wlan/fw/0/version`
setprop vendor.wlan.firmware.version "$wlan_fw_version"
