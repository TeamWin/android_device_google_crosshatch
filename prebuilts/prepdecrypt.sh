#!/system/bin/sh

relink()
{
	fname=$(basename "$1")
	target="/sbin/$fname"
	sed 's|/system/bin/linker64|///////sbin/linker64|' "$1" > "$target"
	chmod 755 $target
}

finish()
{
	#umount /v
	#umount /s
	#rmdir /v
	#rmdir /s
	#mount /v /vendor
	#mount /s/system /system
	setprop prep.decrypt 1
	exit 0
}

#rm -rf /odm
#mv /odmrename /odm
suffix=$(getprop ro.boot.slot_suffix)
if [ -z "$suffix" ]; then
	suf=$(getprop ro.boot.slot)
	suffix="_$suf"
fi
compat_xml_path="/system_root/system/etc/vintf/"
twrp_vintf_path="/sbin/vintf/manifest/"
mkdir -p $twrp_vintf_path
cp $compat_xml_path/compatibility_matrix*xml $twrp_vintf_path
device_codename=$(getprop ro.boot.hardware)
is_fastboot_twrp=$(getprop ro.boot.fastboot)
if [ ! -z "$is_fastboot_twrp" ]; then
	# Note, this method only works on walleye as taimen still fetches the OS / patch information from the active boot slot even when fastboot booting
	# Be sure to increase the PLATFORM_VERSION in build/core/version_defaults.mk to override Google's anti-rollback features to something rather insane
	osver=$(getprop ro.build.version.release_orig)
	patchlevel=$(getprop ro.build.version.security_patch_orig)
	setprop ro.build.version.release "$osver"
	setprop ro.build.version.security_patch "$patchlevel"
	finish
fi

if [ -f /system_root/system/build.prop ]; then
	# TODO: It may be better to try to read these from the boot image than from /system
	osver=$(grep -i 'ro.build.version.release' /system_root/system/build.prop  | cut -f2 -d'=')
	patchlevel=$(grep -i 'ro.build.version.security_patch' /system_root/system/build.prop  | cut -f2 -d'=')
	setprop ro.build.version.release "$osver"
	setprop ro.build.version.security_patch "$patchlevel"
	finish
else
	# Be sure to increase the PLATFORM_VERSION in build/core/version_defaults.mk to override Google's anti-rollback features to something rather insane
    osver=$(getprop ro.build.version_orig)
    patchlevel=$(getprop ro.build.version.security_patch_orig)
	setprop ro.build.version.release "$osver"
	setprop ro.build.version.security_patch "$patchlevel"
	finish
fi

finish
exit 0
