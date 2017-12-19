#! /vendor/bin/sh

# Initialize Edge Sense.
# TODO: b/67205273
/vendor/bin/init.edge_sense.sh

echo 1 > /sys/kernel/boot_adsp/boot
echo 1 > /sys/kernel/boot_slpi/boot
setprop sys.qcom.devup 1
log SLPI boot started.

version=`grep -ao "OEM_IMAGE_VERSION_STRING[ -~]*" \
              /vendor/firmware/slpi.b04 | \
         sed -e s/OEM_IMAGE_VERSION_STRING=SLPI.version.// -e s/\(.*\).//`
setprop sys.slpi.firmware.version "$version"
