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
#ifndef __UAPI_CAM_FD_H__
#define __UAPI_CAM_FD_H__
#include "cam_defs.h"
#define CAM_FD_MAX_FACES 35
#define CAM_FD_RAW_RESULT_ENTRIES 512
#define CAM_PACKET_OPCODES_FD_FRAME_UPDATE 0x0
#define CAM_FD_CMD_BUFFER_ID_GENERIC 0x0
#define CAM_FD_CMD_BUFFER_ID_CDM 0x1
#define CAM_FD_CMD_BUFFER_ID_MAX 0x2
#define CAM_FD_BLOB_TYPE_SOC_CLOCK_BW_REQUEST 0x0
#define CAM_FD_BLOB_TYPE_RAW_RESULTS_REQUIRED 0x1
#define CAM_FD_INPUT_PORT_ID_IMAGE 0x0
#define CAM_FD_INPUT_PORT_ID_MAX 0x1
#define CAM_FD_OUTPUT_PORT_ID_RESULTS 0x0
#define CAM_FD_OUTPUT_PORT_ID_RAW_RESULTS 0x1
#define CAM_FD_OUTPUT_PORT_ID_WORK_BUFFER 0x2
#define CAM_FD_OUTPUT_PORT_ID_MAX 0x3
struct cam_fd_soc_clock_bw_request {
  uint64_t clock_rate;
  uint64_t bandwidth;
  uint64_t reserved[4];
};
struct cam_fd_face {
  uint32_t prop1;
  uint32_t prop2;
  uint32_t prop3;
  uint32_t prop4;
};
struct cam_fd_results {
  struct cam_fd_face faces[CAM_FD_MAX_FACES];
  uint32_t face_count;
  uint32_t reserved[3];
};
struct cam_fd_hw_caps {
  struct cam_hw_version core_version;
  struct cam_hw_version wrapper_version;
  uint32_t raw_results_available;
  uint32_t supported_modes;
  uint64_t reserved;
};
struct cam_fd_query_cap_cmd {
  struct cam_iommu_handle device_iommu;
  struct cam_iommu_handle cdm_iommu;
  struct cam_fd_hw_caps hw_caps;
  uint64_t reserved;
};
struct cam_fd_acquire_dev_info {
  struct cam_fd_soc_clock_bw_request clk_bw_request;
  uint32_t priority;
  uint32_t mode;
  uint32_t get_raw_results;
  uint32_t reserved[13];
};
#endif

