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
#ifndef __UAPI_LINUX_CAM_REQ_MGR_H
#define __UAPI_LINUX_CAM_REQ_MGR_H
#include <linux/videodev2.h>
#include <linux/types.h>
#include <linux/ioctl.h>
#include <linux/media.h>
#include <media/cam_defs.h>
#define CAM_REQ_MGR_VNODE_NAME "cam-req-mgr-devnode"
#define CAM_DEVICE_TYPE_BASE (MEDIA_ENT_F_OLD_BASE)
#define CAM_VNODE_DEVICE_TYPE (CAM_DEVICE_TYPE_BASE)
#define CAM_SENSOR_DEVICE_TYPE (CAM_DEVICE_TYPE_BASE + 1)
#define CAM_IFE_DEVICE_TYPE (CAM_DEVICE_TYPE_BASE + 2)
#define CAM_ICP_DEVICE_TYPE (CAM_DEVICE_TYPE_BASE + 3)
#define CAM_LRME_DEVICE_TYPE (CAM_DEVICE_TYPE_BASE + 4)
#define CAM_JPEG_DEVICE_TYPE (CAM_DEVICE_TYPE_BASE + 5)
#define CAM_FD_DEVICE_TYPE (CAM_DEVICE_TYPE_BASE + 6)
#define CAM_CPAS_DEVICE_TYPE (CAM_DEVICE_TYPE_BASE + 7)
#define CAM_CSIPHY_DEVICE_TYPE (CAM_DEVICE_TYPE_BASE + 8)
#define CAM_ACTUATOR_DEVICE_TYPE (CAM_DEVICE_TYPE_BASE + 9)
#define CAM_CCI_DEVICE_TYPE (CAM_DEVICE_TYPE_BASE + 10)
#define CAM_FLASH_DEVICE_TYPE (CAM_DEVICE_TYPE_BASE + 11)
#define CAM_EEPROM_DEVICE_TYPE (CAM_DEVICE_TYPE_BASE + 12)
#define CAM_OIS_DEVICE_TYPE (CAM_DEVICE_TYPE_BASE + 13)
#define CAM_REQ_MGR_HDL_IDX_POS 8
#define CAM_REQ_MGR_HDL_IDX_MASK ((1 << CAM_REQ_MGR_HDL_IDX_POS) - 1)
#define CAM_REQ_MGR_GET_HDL_IDX(hdl) (hdl & CAM_REQ_MGR_HDL_IDX_MASK)
#define CAM_REQ_MGR_MAX_HANDLES 64
#define MAX_LINKS_PER_SESSION 2
#define V4L_EVENT_CAM_REQ_MGR_EVENT (V4L2_EVENT_PRIVATE_START + 0)
#define V4L_EVENT_CAM_REQ_MGR_SOF 0
#define V4L_EVENT_CAM_REQ_MGR_ERROR 1
#define V4L_EVENT_CAM_REQ_MGR_SOF_BOOT_TS 2
#define V4L_EVENT_CAM_REQ_MGR_MAX 3
#define CAM_REQ_MGR_SOF_EVENT_SUCCESS 0
#define CAM_REQ_MGR_SOF_EVENT_ERROR 1
#define CAM_REQ_MGR_LINK_ACTIVATE 0
#define CAM_REQ_MGR_LINK_DEACTIVATE 1
#define CAM_REQ_MGR_FLUSH_TYPE_ALL 0
#define CAM_REQ_MGR_FLUSH_TYPE_CANCEL_REQ 1
#define CAM_REQ_MGR_FLUSH_TYPE_MAX 2
#define CAM_REQ_MGR_SYNC_MODE_NO_SYNC 0
#define CAM_REQ_MGR_SYNC_MODE_SYNC 1
struct cam_req_mgr_event_data {
  int32_t session_hdl;
  int32_t link_hdl;
  int32_t frame_id;
  int32_t reserved;
  int64_t req_id;
  uint64_t tv_sec;
  uint64_t tv_usec;
};
struct cam_req_mgr_session_info {
  int32_t session_hdl;
  int32_t reserved;
};
struct cam_req_mgr_link_info {
  int32_t session_hdl;
  uint32_t num_devices;
  int32_t dev_hdls[CAM_REQ_MGR_MAX_HANDLES];
  int32_t link_hdl;
};
struct cam_req_mgr_unlink_info {
  int32_t session_hdl;
  int32_t link_hdl;
};
struct cam_req_mgr_flush_info {
  int32_t session_hdl;
  int32_t link_hdl;
  uint32_t flush_type;
  uint32_t reserved;
  int64_t req_id;
};
struct cam_req_mgr_sched_request {
  int32_t session_hdl;
  int32_t link_hdl;
  int32_t bubble_enable;
  int32_t sync_mode;
  int64_t req_id;
};
struct cam_req_mgr_sync_mode {
  int32_t session_hdl;
  int32_t sync_mode;
  int32_t num_links;
  int32_t link_hdls[MAX_LINKS_PER_SESSION];
  int32_t master_link_hdl;
  int32_t reserved;
};
struct cam_req_mgr_link_control {
  int32_t ops;
  int32_t session_hdl;
  int32_t num_links;
  int32_t reserved;
  int32_t link_hdls[MAX_LINKS_PER_SESSION];
};
#define CAM_REQ_MGR_CREATE_DEV_NODES (CAM_COMMON_OPCODE_MAX + 1)
#define CAM_REQ_MGR_CREATE_SESSION (CAM_COMMON_OPCODE_MAX + 2)
#define CAM_REQ_MGR_DESTROY_SESSION (CAM_COMMON_OPCODE_MAX + 3)
#define CAM_REQ_MGR_LINK (CAM_COMMON_OPCODE_MAX + 4)
#define CAM_REQ_MGR_UNLINK (CAM_COMMON_OPCODE_MAX + 5)
#define CAM_REQ_MGR_SCHED_REQ (CAM_COMMON_OPCODE_MAX + 6)
#define CAM_REQ_MGR_FLUSH_REQ (CAM_COMMON_OPCODE_MAX + 7)
#define CAM_REQ_MGR_SYNC_MODE (CAM_COMMON_OPCODE_MAX + 8)
#define CAM_REQ_MGR_ALLOC_BUF (CAM_COMMON_OPCODE_MAX + 9)
#define CAM_REQ_MGR_MAP_BUF (CAM_COMMON_OPCODE_MAX + 10)
#define CAM_REQ_MGR_RELEASE_BUF (CAM_COMMON_OPCODE_MAX + 11)
#define CAM_REQ_MGR_CACHE_OPS (CAM_COMMON_OPCODE_MAX + 12)
#define CAM_REQ_MGR_LINK_CONTROL (CAM_COMMON_OPCODE_MAX + 13)
#define CAM_MEM_FLAG_HW_READ_WRITE (1 << 0)
#define CAM_MEM_FLAG_HW_READ_ONLY (1 << 1)
#define CAM_MEM_FLAG_HW_WRITE_ONLY (1 << 2)
#define CAM_MEM_FLAG_KMD_ACCESS (1 << 3)
#define CAM_MEM_FLAG_UMD_ACCESS (1 << 4)
#define CAM_MEM_FLAG_PROTECTED_MODE (1 << 5)
#define CAM_MEM_FLAG_CMD_BUF_TYPE (1 << 6)
#define CAM_MEM_FLAG_PIXEL_BUF_TYPE (1 << 7)
#define CAM_MEM_FLAG_STATS_BUF_TYPE (1 << 8)
#define CAM_MEM_FLAG_PACKET_BUF_TYPE (1 << 9)
#define CAM_MEM_FLAG_CACHE (1 << 10)
#define CAM_MEM_FLAG_HW_SHARED_ACCESS (1 << 11)
#define CAM_MEM_MMU_MAX_HANDLE 16
#define CAM_MEM_BUFQ_MAX 1024
#define CAM_MEM_MGR_SECURE_BIT_POS 15
#define CAM_MEM_MGR_HDL_IDX_SIZE 15
#define CAM_MEM_MGR_HDL_FD_SIZE 16
#define CAM_MEM_MGR_HDL_IDX_END_POS 16
#define CAM_MEM_MGR_HDL_FD_END_POS 32
#define CAM_MEM_MGR_HDL_IDX_MASK ((1 << CAM_MEM_MGR_HDL_IDX_SIZE) - 1)
#define GET_MEM_HANDLE(idx,fd) ((idx & CAM_MEM_MGR_HDL_IDX_MASK) | (fd << (CAM_MEM_MGR_HDL_FD_END_POS - CAM_MEM_MGR_HDL_FD_SIZE)))
#define CAM_MEM_MGR_GET_HDL_IDX(hdl) (hdl & CAM_MEM_MGR_HDL_IDX_MASK)
#define CAM_MEM_MGR_SET_SECURE_HDL(hdl,flag) ((flag) ? (hdl |= (1 << CAM_MEM_MGR_SECURE_BIT_POS)) : ((hdl) &= ~(1 << CAM_MEM_MGR_SECURE_BIT_POS)))
#define CAM_MEM_MGR_IS_SECURE_HDL(hdl) (((hdl) & (1 << CAM_MEM_MGR_SECURE_BIT_POS)) >> CAM_MEM_MGR_SECURE_BIT_POS)
#define CAM_MEM_DMA_NONE 0
#define CAM_MEM_DMA_BIDIRECTIONAL 1
#define CAM_MEM_DMA_TO_DEVICE 2
#define CAM_MEM_DMA_FROM_DEVICE 3
#define CAM_MEM_CLEAN_CACHE 1
#define CAM_MEM_INV_CACHE 2
#define CAM_MEM_CLEAN_INV_CACHE 3
struct cam_mem_alloc_out_params {
  uint32_t buf_handle;
  int32_t fd;
  uint64_t vaddr;
};
struct cam_mem_map_out_params {
  uint32_t buf_handle;
  uint32_t reserved;
  uint64_t vaddr;
};
struct cam_mem_mgr_alloc_cmd {
  uint64_t len;
  uint64_t align;
  int32_t mmu_hdls[CAM_MEM_MMU_MAX_HANDLE];
  uint32_t num_hdl;
  uint32_t flags;
  struct cam_mem_alloc_out_params out;
};
struct cam_mem_mgr_map_cmd {
  int32_t mmu_hdls[CAM_MEM_MMU_MAX_HANDLE];
  uint32_t num_hdl;
  uint32_t flags;
  int32_t fd;
  uint32_t reserved;
  struct cam_mem_map_out_params out;
};
struct cam_mem_mgr_release_cmd {
  int32_t buf_handle;
  uint32_t reserved;
};
struct cam_mem_cache_ops_cmd {
  int32_t buf_handle;
  uint32_t mem_cache_ops;
};
#define CAM_REQ_MGR_ERROR_TYPE_DEVICE 0
#define CAM_REQ_MGR_ERROR_TYPE_REQUEST 1
#define CAM_REQ_MGR_ERROR_TYPE_BUFFER 2
struct cam_req_mgr_error_msg {
  uint32_t error_type;
  uint32_t request_id;
  int32_t device_hdl;
  int32_t link_hdl;
  uint64_t resource_size;
};
struct cam_req_mgr_frame_msg {
  uint64_t request_id;
  uint64_t frame_id;
  uint64_t timestamp;
  int32_t link_hdl;
  uint32_t sof_status;
};
struct cam_req_mgr_message {
  int32_t session_hdl;
  int32_t reserved;
  union {
    struct cam_req_mgr_error_msg err_msg;
    struct cam_req_mgr_frame_msg frame_msg;
  } u;
};
#endif

