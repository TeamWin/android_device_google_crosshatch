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
#ifndef __UAPI_CAM_LRME_H__
#define __UAPI_CAM_LRME_H__
#include "cam_defs.h"
enum CAM_LRME_IO_TYPE {
  CAM_LRME_IO_TYPE_TAR,
  CAM_LRME_IO_TYPE_REF,
  CAM_LRME_IO_TYPE_RES,
  CAM_LRME_IO_TYPE_DS2,
};
#define CAM_LRME_INPUT_PORT_TYPE_TAR (1 << 0)
#define CAM_LRME_INPUT_PORT_TYPE_REF (1 << 1)
#define CAM_LRME_OUTPUT_PORT_TYPE_DS2 (1 << 0)
#define CAM_LRME_OUTPUT_PORT_TYPE_RES (1 << 1)
#define CAM_LRME_DEV_MAX 1
struct cam_lrme_hw_version {
  uint32_t gen;
  uint32_t rev;
  uint32_t step;
};
struct cam_lrme_dev_cap {
  struct cam_lrme_hw_version clc_hw_version;
  struct cam_lrme_hw_version bus_rd_hw_version;
  struct cam_lrme_hw_version bus_wr_hw_version;
  struct cam_lrme_hw_version top_hw_version;
  struct cam_lrme_hw_version top_titan_version;
};
struct cam_lrme_query_cap_cmd {
  struct cam_iommu_handle device_iommu;
  struct cam_iommu_handle cdm_iommu;
  uint32_t num_devices;
  struct cam_lrme_dev_cap dev_caps[CAM_LRME_DEV_MAX];
};
struct cam_lrme_soc_info {
  uint64_t clock_rate;
  uint64_t bandwidth;
  uint64_t reserved[4];
};
struct cam_lrme_acquire_args {
  struct cam_lrme_soc_info lrme_soc_info;
};
#endif

