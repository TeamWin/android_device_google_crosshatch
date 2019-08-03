#! /vendor/bin/sh

# Initialize Edge Sense.
/vendor/bin/init.edge_sense.sh

echo 1 > /sys/kernel/boot_slpi/boot
setprop vendor.qcom.devup 1
log SLPI boot started.

version=`grep -ao "OEM_IMAGE_VERSION_STRING[ -~]*" \
              /vendor/firmware/slpi.b04 | \
         sed -e s/OEM_IMAGE_VERSION_STRING=SLPI.version.// -e s/\(.*\).//`
setprop vendor.sys.slpi.firmware.version "$version"
