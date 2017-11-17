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
#ifndef __UAPI_CAM_JPEG_H__
#define __UAPI_CAM_JPEG_H__
#include "cam_defs.h"
#define CAM_JPEG_DEV_TYPE_ENC 0
#define CAM_JPEG_DEV_TYPE_DMA 1
#define CAM_JPEG_DEV_TYPE_MAX 2
#define CAM_JPEG_NUM_DEV_PER_RES_MAX 1
#define CAM_JPEG_RES_TYPE_ENC 0
#define CAM_JPEG_RES_TYPE_DMA 1
#define CAM_JPEG_RES_TYPE_MAX 2
#define CAM_JPEG_OPCODE_ENC_UPDATE 0
#define CAM_JPEG_OPCODE_DMA_UPDATE 1
#define CAM_JPEG_ENC_INPUT_IMAGE 0x0
#define CAM_JPEG_ENC_OUTPUT_IMAGE 0x1
#define CAM_JPEG_ENC_IO_IMAGES_MAX 0x2
#define CAM_JPEG_DMA_INPUT_IMAGE 0x0
#define CAM_JPEG_DMA_OUTPUT_IMAGE 0x1
#define CAM_JPEG_DMA_IO_IMAGES_MAX 0x2
#define CAM_JPEG_IMAGE_MAX 0x2
struct cam_jpeg_dev_ver {
  uint32_t size;
  uint32_t dev_type;
  struct cam_hw_version hw_ver;
};
struct cam_jpeg_query_cap_cmd {
  struct cam_iommu_handle dev_iommu_handle;
  struct cam_iommu_handle cdm_iommu_handle;
  uint32_t num_enc;
  uint32_t num_dma;
  struct cam_jpeg_dev_ver dev_ver[CAM_JPEG_DEV_TYPE_MAX];
};
struct cam_jpeg_res_info {
  uint32_t format;
  uint32_t width;
  uint32_t height;
  uint32_t fps;
};
struct cam_jpeg_acquire_dev_info {
  uint32_t dev_type;
  uint32_t reserved;
  struct cam_jpeg_res_info in_res;
  struct cam_jpeg_res_info out_res;
};
struct cam_jpeg_config_inout_param_info {
  int32_t clk_index;
  int32_t output_size;
};
#endif

