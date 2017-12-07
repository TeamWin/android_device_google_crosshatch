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
#ifndef __UAPI_CAM_SYNC_H__
#define __UAPI_CAM_SYNC_H__
#include <linux/videodev2.h>
#include <linux/types.h>
#include <linux/ioctl.h>
#include <linux/media.h>
#define CAM_SYNC_DEVICE_NAME "cam_sync_device"
#define CAM_SYNC_V4L_EVENT (V4L2_EVENT_PRIVATE_START + 0)
#define CAM_SYNC_V4L_EVENT_ID_CB_TRIG 0
#define CAM_SYNC_USER_PAYLOAD_SIZE 2
#define CAM_SYNC_DEVICE_TYPE (MEDIA_ENT_F_OLD_BASE)
#define CAM_SYNC_GET_PAYLOAD_PTR(ev,type) (type *) ((char *) ev.u.data + sizeof(struct cam_sync_ev_header))
#define CAM_SYNC_GET_HEADER_PTR(ev) ((struct cam_sync_ev_header *) ev.u.data)
#define CAM_SYNC_STATE_INVALID 0
#define CAM_SYNC_STATE_ACTIVE 1
#define CAM_SYNC_STATE_SIGNALED_SUCCESS 2
#define CAM_SYNC_STATE_SIGNALED_ERROR 3
struct cam_sync_ev_header {
  int32_t sync_obj;
  int32_t status;
};
struct cam_sync_info {
  char name[64];
  int32_t sync_obj;
};
struct cam_sync_signal {
  int32_t sync_obj;
  uint32_t sync_state;
};
struct cam_sync_merge {
  __u64 sync_objs;
  uint32_t num_objs;
  int32_t merged;
};
struct cam_sync_userpayload_info {
  int32_t sync_obj;
  uint32_t reserved;
  __u64 payload[CAM_SYNC_USER_PAYLOAD_SIZE];
};
struct cam_sync_wait {
  int32_t sync_obj;
  uint32_t reserved;
  uint64_t timeout_ms;
};
struct cam_private_ioctl_arg {
  __u32 id;
  __u32 size;
  __u32 result;
  __u32 reserved;
  __u64 ioctl_ptr;
};
#define CAM_PRIVATE_IOCTL_CMD _IOWR('V', BASE_VIDIOC_PRIVATE, struct cam_private_ioctl_arg)
#define CAM_SYNC_CREATE 0
#define CAM_SYNC_DESTROY 1
#define CAM_SYNC_SIGNAL 2
#define CAM_SYNC_MERGE 3
#define CAM_SYNC_REGISTER_PAYLOAD 4
#define CAM_SYNC_DEREGISTER_PAYLOAD 5
#define CAM_SYNC_WAIT 6
#endif

