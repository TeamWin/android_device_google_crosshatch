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
#ifndef _MSM_AUDIO_VOICEMEMO_H
#define _MSM_AUDIO_VOICEMEMO_H
#include <linux/msm_audio.h>
#define AUDIO_GET_VOICEMEMO_CONFIG _IOW(AUDIO_IOCTL_MAGIC, (AUDIO_MAX_COMMON_IOCTL_NUM + 0), unsigned int)
#define AUDIO_SET_VOICEMEMO_CONFIG _IOR(AUDIO_IOCTL_MAGIC, (AUDIO_MAX_COMMON_IOCTL_NUM + 1), unsigned int)
enum rpc_voc_rec_dir_type {
  RPC_VOC_REC_NONE,
  RPC_VOC_REC_FORWARD,
  RPC_VOC_REC_REVERSE,
  RPC_VOC_REC_BOTH,
  RPC_VOC_MAX_REC_TYPE
};
enum rpc_voc_capability_type {
  RPC_VOC_CAP_IS733 = 4,
  RPC_VOC_CAP_IS127 = 8,
  RPC_VOC_CAP_AMR = 64,
  RPC_VOC_CAP_32BIT_DUMMY = 2147483647
};
enum rpc_voc_rate_type {
  RPC_VOC_0_RATE = 0,
  RPC_VOC_8_RATE,
  RPC_VOC_4_RATE,
  RPC_VOC_2_RATE,
  RPC_VOC_1_RATE,
  RPC_VOC_ERASURE,
  RPC_VOC_ERR_RATE,
  RPC_VOC_AMR_RATE_475 = 0,
  RPC_VOC_AMR_RATE_515 = 1,
  RPC_VOC_AMR_RATE_590 = 2,
  RPC_VOC_AMR_RATE_670 = 3,
  RPC_VOC_AMR_RATE_740 = 4,
  RPC_VOC_AMR_RATE_795 = 5,
  RPC_VOC_AMR_RATE_1020 = 6,
  RPC_VOC_AMR_RATE_1220 = 7,
};
enum rpc_voc_pb_len_rate_var_type {
  RPC_VOC_PB_NATIVE_QCP = 3,
  RPC_VOC_PB_AMR,
  RPC_VOC_PB_EVB
};
struct msm_audio_voicememo_config {
  uint32_t rec_type;
  uint32_t rec_interval_ms;
  uint32_t auto_stop_ms;
  uint32_t capability;
  uint32_t max_rate;
  uint32_t min_rate;
  uint32_t frame_format;
  uint32_t dtx_enable;
  uint32_t data_req_ms;
};
#endif

