#!/vendor/bin/sh

# Decrypt the keys and write them to the kernel
ramoops -D

if [ $? -eq 0 ]; then
    # Pivot (and decrypt)
    echo 1 > /sys/devices/virtual/ramoops/pstore/use_alt
else
    setprop vendor.ramoops.decryption.error $?
fi

# Trigger remount of pstore regardless of decryption state
setprop vendor.ramoops.decrypted true

# Generate keys (if none exist), and load the keys to carveout
ramoops -g -l -c

