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
#ifndef _MSM_RMNET_H_
#define _MSM_RMNET_H_
#define RMNET_MODE_NONE (0x00)
#define RMNET_MODE_LLP_ETH (0x01)
#define RMNET_MODE_LLP_IP (0x02)
#define RMNET_MODE_QOS (0x04)
#define RMNET_MODE_MASK (RMNET_MODE_LLP_ETH | RMNET_MODE_LLP_IP | RMNET_MODE_QOS)
#define RMNET_IS_MODE_QOS(mode) ((mode & RMNET_MODE_QOS) == RMNET_MODE_QOS)
#define RMNET_IS_MODE_IP(mode) ((mode & RMNET_MODE_LLP_IP) == RMNET_MODE_LLP_IP)
#define RMNET_IOCTL_SET_LLP_ETHERNET 0x000089F1
#define RMNET_IOCTL_SET_LLP_IP 0x000089F2
#define RMNET_IOCTL_GET_LLP 0x000089F3
#define RMNET_IOCTL_SET_QOS_ENABLE 0x000089F4
#define RMNET_IOCTL_SET_QOS_DISABLE 0x000089F5
#define RMNET_IOCTL_GET_QOS 0x000089F6
#define RMNET_IOCTL_GET_OPMODE 0x000089F7
#define RMNET_IOCTL_OPEN 0x000089F8
#define RMNET_IOCTL_CLOSE 0x000089F9
#define RMNET_IOCTL_FLOW_ENABLE 0x000089FA
#define RMNET_IOCTL_FLOW_DISABLE 0x000089FB
#define RMNET_IOCTL_FLOW_SET_HNDL 0x000089FC
#define RMNET_IOCTL_EXTENDED 0x000089FD
#define RMNET_IOCTL_GET_SUPPORTED_FEATURES 0x0000
#define RMNET_IOCTL_SET_MRU 0x0001
#define RMNET_IOCTL_GET_MRU 0x0002
#define RMNET_IOCTL_GET_EPID 0x0003
#define RMNET_IOCTL_GET_DRIVER_NAME 0x0004
#define RMNET_IOCTL_ADD_MUX_CHANNEL 0x0005
#define RMNET_IOCTL_SET_EGRESS_DATA_FORMAT 0x0006
#define RMNET_IOCTL_SET_INGRESS_DATA_FORMAT 0x0007
#define RMNET_IOCTL_SET_AGGREGATION_COUNT 0x0008
#define RMNET_IOCTL_GET_AGGREGATION_COUNT 0x0009
#define RMNET_IOCTL_SET_AGGREGATION_SIZE 0x000A
#define RMNET_IOCTL_GET_AGGREGATION_SIZE 0x000B
#define RMNET_IOCTL_FLOW_CONTROL 0x000C
#define RMNET_IOCTL_GET_DFLT_CONTROL_CHANNEL 0x000D
#define RMNET_IOCTL_GET_HWSW_MAP 0x000E
#define RMNET_IOCTL_SET_RX_HEADROOM 0x000F
#define RMNET_IOCTL_GET_EP_PAIR 0x0010
#define RMNET_IOCTL_SET_QOS_VERSION 0x0011
#define RMNET_IOCTL_GET_QOS_VERSION 0x0012
#define RMNET_IOCTL_GET_SUPPORTED_QOS_MODES 0x0013
#define RMNET_IOCTL_SET_SLEEP_STATE 0x0014
#define RMNET_IOCTL_SET_XLAT_DEV_INFO 0x0015
#define RMNET_IOCTL_DEREGISTER_DEV 0x0016
#define RMNET_IOCTL_GET_SG_SUPPORT 0x0017
#define RMNET_IOCTL_FEAT_NOTIFY_MUX_CHANNEL (1 << 0)
#define RMNET_IOCTL_FEAT_SET_EGRESS_DATA_FORMAT (1 << 1)
#define RMNET_IOCTL_FEAT_SET_INGRESS_DATA_FORMAT (1 << 2)
#define RMNET_IOCTL_FEAT_SET_AGGREGATION_COUNT (1 << 3)
#define RMNET_IOCTL_FEAT_GET_AGGREGATION_COUNT (1 << 4)
#define RMNET_IOCTL_FEAT_SET_AGGREGATION_SIZE (1 << 5)
#define RMNET_IOCTL_FEAT_GET_AGGREGATION_SIZE (1 << 6)
#define RMNET_IOCTL_FEAT_FLOW_CONTROL (1 << 7)
#define RMNET_IOCTL_FEAT_GET_DFLT_CONTROL_CHANNEL (1 << 8)
#define RMNET_IOCTL_FEAT_GET_HWSW_MAP (1 << 9)
#define RMNET_IOCTL_EGRESS_FORMAT_MAP (1 << 1)
#define RMNET_IOCTL_EGRESS_FORMAT_AGGREGATION (1 << 2)
#define RMNET_IOCTL_EGRESS_FORMAT_MUXING (1 << 3)
#define RMNET_IOCTL_EGRESS_FORMAT_CHECKSUM (1 << 4)
#define RMNET_IOCTL_INGRESS_FORMAT_MAP (1 << 1)
#define RMNET_IOCTL_INGRESS_FORMAT_DEAGGREGATION (1 << 2)
#define RMNET_IOCTL_INGRESS_FORMAT_DEMUXING (1 << 3)
#define RMNET_IOCTL_INGRESS_FORMAT_CHECKSUM (1 << 4)
#define RMNET_IOCTL_INGRESS_FORMAT_AGG_DATA (1 << 5)
#ifndef IFNAMSIZ
#define IFNAMSIZ 16
#endif
struct rmnet_ioctl_extended_s {
  uint32_t extended_ioctl;
  union {
    uint32_t data;
    int8_t if_name[IFNAMSIZ];
    struct {
      uint32_t mux_id;
      int8_t vchannel_name[IFNAMSIZ];
    } rmnet_mux_val;
    struct {
      uint8_t flow_mode;
      uint8_t mux_id;
    } flow_control_prop;
    struct {
      uint32_t consumer_pipe_num;
      uint32_t producer_pipe_num;
    } ipa_ep_pair;
    struct {
      uint32_t __data;
      uint32_t agg_size;
      uint32_t agg_count;
    } ingress_format;
  } u;
};
struct rmnet_ioctl_data_s {
  union {
    uint32_t operation_mode;
    uint32_t tcm_handle;
  } u;
};
#define RMNET_IOCTL_QOS_MODE_6 (1 << 0)
#define RMNET_IOCTL_QOS_MODE_8 (1 << 1)
struct QMI_QOS_HDR_S {
  unsigned char version;
  unsigned char flags;
  uint32_t flow_id;
} __attribute((__packed__));
struct qmi_qos_hdr8_s {
  struct QMI_QOS_HDR_S hdr;
  uint8_t reserved[2];
} __attribute((__packed__));
#endif

