#!/vendor/bin/sh
#
# edge_sense_init.sh [<power chip name>] [<power gpio number>]
#
# Initialize Edge Sense.  If needed, power it up using the power controller chip
# specified by [<power chip name>] and power GPIO specified by
# [<power gpio number>].
#
#   [<power chip name>]     Name of chip (e.g., "pm8998") controlling Edge Sense
#                           power.
#   [<power gpio number>]   GPIO number controlling Edge Sens power (e.g., 2).
#
# [<power chip name>] and [<power gpio number>] default to values appropriate
# for the type and version of device.
#
# TODO: b/67205273
#       The Edge Sense should only be powered up when it's in use.
#       Ideally, this would be done in the Edge Sense SLPI driver, but it
#       doesn't have direct access to the PM8998 GPIOs.
#       As an alternative, the Elmyra Edge Sense sensor HAL driver could power
#       up the Edge Sense or act as a GPIO proxy for the SLPI driver.
#

# Check for default values.
if [ "${#}" -eq 0 ]; then
  use_defaults=1
else
  use_defaults=0
fi

# Get the program name.
prog_name=$(basename ${0})

# Read the power chip name.
chip_name=${1}

# Read the power gpio number.
gpio_num=${2}

# Get the hardware platform and platform version.
hw_platform=`cat /sys/devices/soc0/hw_platform`
platform_version=`cat /sys/devices/soc0/platform_version`

# If using default values, check if platform needs Edge Sense to be initialized.
if [ ${use_defaults} -ne 0 ]; then
  if [ "${hw_platform}" != "OEM" ] || \
     [ "${platform_version}" == "65538" ]; then
    log -t "${prog_name}" "Platform \"${hw_platform}\" version" \
        "${platform_version} does not need Edge Sense to be initialized."
    exit
  fi
fi

# Set default values if using them.
if [ ${use_defaults} -ne 0 ]; then
  chip_name=pm8998
  gpio_num=2
fi

# Validate chip name and gpio number.
if [ -z ${chip_name} ]; then
  log -t "${prog_name}" "Chip name not specified."
  exit 1
fi
if [ -z ${gpio_num} ]; then
  log -t "${prog_name}" "GPIO number not specified."
  exit 1
fi

# Find the GPIO pin control device node for the power chip.
pinctrl=`find /sys/devices -name "*${chip_name}*pinctrl*"`
if [ -z ${pinctrl} ]; then
  log -t "${prog_name}" "Power chip \"${chip_name}\" not found."
  exit 1
fi

# Find the GPIO index within the chip GPIO interrupt name list.  This will be
# the GPIO index offset from the chip GPIO index base.
found=0
gpio_name=gpio${gpio_num}
gpio_index_off=0
while IFS= read -d '' name; do
  # Check for a match.
  if [ "${name%${gpio_name}}" != "${name}" ]; then
    found=1
    break
  fi

  # Check next GPIO index.
  gpio_index_off=$((${gpio_index_off} + 1))
done < ${pinctrl}/of_node/interrupt-names
if [ ${found} -eq 0 ]; then
  log -t "${prog_name}" "GPIO ${gpio_num} on chip \"${chip_name}\" not found."
  exit 1
fi

# Find the chip GPIO base index.
base_file=`find ${pinctrl} -name base`
gpio_index_base=`cat ${base_file}`

# Get the GPIO index.
gpio_index=$((${gpio_index_base} + ${gpio_index_off}))

# Export the GPIO.
echo ${gpio_index} > /sys/class/gpio/export

# Set the GPIO direction to out.
echo out > /sys/class/gpio/gpio${gpio_index}/direction

# Set the GPIO high.
echo 1 > /sys/class/gpio/gpio${gpio_index}/value


