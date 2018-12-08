#!/sbin/sh
SLOT=`getprop ro.boot.slot_suffix`
mount /dev/block/by-name/vendor$SLOT /vendor -o ro
insmod /vendor/lib/modules/ftm5.ko
insmod /vendor/lib/modules/sec_touch.ko
umount /vendor
