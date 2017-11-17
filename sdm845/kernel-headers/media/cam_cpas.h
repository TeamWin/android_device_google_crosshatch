/****************************************************************************
 ****************************************************************************
 ***
 ***   This header was automatically generated from a Linux kernel header
 ***   of the same name, to make information necessary for userspace to
 ***   call into the kernel available to libc.  It contains only constants,
 ***   structures, and macros generated from the original header, and thus,
 ***   contains no copyrightable information.
 ***
 ***   To edit the content of this header, modify the corresponding
 ***   source file (e.g. under external/kernel-headers/original/) then
 ***   run bionic/libc/kernel/tools/update_all.py
 ***
 ***   Any manual change here will be lost the next time this script will
 ***   be run. You've been warned!
 ***
 ****************************************************************************
 ****************************************************************************/
#ifndef __UAPI_CAM_CPAS_H__
#define __UAPI_CAM_CPAS_H__
#include "cam_defs.h"
#define CAM_FAMILY_CAMERA_SS 1
#define CAM_FAMILY_CPAS_SS 2
struct cam_cpas_query_cap {
  uint32_t camera_family;
  uint32_t reserved;
  struct cam_hw_version camera_version;
  struct cam_hw_version cpas_version;
};
#endif

