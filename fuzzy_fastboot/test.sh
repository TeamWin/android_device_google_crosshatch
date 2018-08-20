#!/usr/bin/env bash
if [ -z "$ANDROID_BUILD_TOP" ]
then
    echo "You must run lunch to setup the environment"
    exit 1
fi

set -e
BOOTLOADER_IMG="bootloader.img"
RADIO_IMG="radio.img"
BUILD_IMG_PATH="$ANDROID_BUILD_TOP/vendor/google_devices/crosshatch/prebuilts"
BOOTLOADER_PATH="$BUILD_IMG_PATH/$BOOTLOADER_IMG"
RADIO_PATH="$BUILD_IMG_PATH/$RADIO_IMG"
TOOL="$ANDROID_HOST_OUT/testcases/fuzzy_fastboot/x86_64/fuzzy_fastboot"
CONFIG_PATH=$(dirname $0)

# Generate the garbage images
RADIO_SIZE=$(wc -c <"$RADIO_PATH")
BOOTLOADER_SIZE=$(wc -c <"$BOOTLOADER_PATH")
head -c $RADIO_SIZE </dev/urandom>radio_garbage.img
head -c $BOOTLOADER_SIZE </dev/urandom>boorloader_garbage.img

# Symlink the good images
ln -s "$BOOTLOADER_PATH" "$BOOTLOADER_IMG"
ln -s "$RADIO_PATH" "$RADIO_IMG"

# Make the output image directory
mkdir -p "$CONFIG_PATH/output"

# Run fuzzy_fastboot
$TOOL "--search_path=$CONFIG_PATH" "--config=config.xml" "--output_path=output" "$@"

# Clean up
rm radio_garbage.img
rm bootloader_garbage.img
rm "$BOOTLOADER_IMG"
rm "$RADIO_IMG"
