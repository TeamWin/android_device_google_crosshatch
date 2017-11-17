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
#ifndef _RMNET_IPA_FD_IOCTL_H
#define _RMNET_IPA_FD_IOCTL_H
#include <linux/ioctl.h>
#include <linux/ipa_qmi_service_v01.h>
#include <linux/msm_ipa.h>
#define WAN_IOC_MAGIC 0x69
#define WAN_IOCTL_ADD_FLT_RULE 0
#define WAN_IOCTL_ADD_FLT_INDEX 1
#define WAN_IOCTL_VOTE_FOR_BW_MBPS 2
#define WAN_IOCTL_POLL_TETHERING_STATS 3
#define WAN_IOCTL_SET_DATA_QUOTA 4
#define WAN_IOCTL_SET_TETHER_CLIENT_PIPE 5
#define WAN_IOCTL_QUERY_TETHER_STATS 6
#define WAN_IOCTL_RESET_TETHER_STATS 7
#define WAN_IOCTL_QUERY_DL_FILTER_STATS 8
#define WAN_IOCTL_ADD_FLT_RULE_EX 9
#define WAN_IOCTL_QUERY_TETHER_STATS_ALL 10
#define WAN_IOCTL_NOTIFY_WAN_STATE 11
#define WAN_IOCTL_ADD_UL_FLT_RULE 12
#define WAN_IOCTL_ENABLE_PER_CLIENT_STATS 13
#define WAN_IOCTL_QUERY_PER_CLIENT_STATS 14
#define WAN_IOCTL_SET_LAN_CLIENT_INFO 15
#define WAN_IOCTL_CLEAR_LAN_CLIENT_INFO 16
#define WAN_IOCTL_SEND_LAN_CLIENT_MSG 17
#ifndef IFNAMSIZ
#define IFNAMSIZ 16
#endif
struct wan_ioctl_poll_tethering_stats {
  uint64_t polling_interval_secs;
  uint8_t reset_stats;
};
struct wan_ioctl_set_data_quota {
  char interface_name[IFNAMSIZ];
  uint64_t quota_mbytes;
  uint8_t set_quota;
};
struct wan_ioctl_set_tether_client_pipe {
  enum ipacm_client_enum ipa_client;
  uint8_t reset_client;
  uint32_t ul_src_pipe_len;
  uint32_t ul_src_pipe_list[QMI_IPA_MAX_PIPES_V01];
  uint32_t dl_dst_pipe_len;
  uint32_t dl_dst_pipe_list[QMI_IPA_MAX_PIPES_V01];
};
struct wan_ioctl_query_tether_stats {
  char upstreamIface[IFNAMSIZ];
  char tetherIface[IFNAMSIZ];
  enum ipacm_client_enum ipa_client;
  uint64_t ipv4_tx_packets;
  uint64_t ipv4_tx_bytes;
  uint64_t ipv4_rx_packets;
  uint64_t ipv4_rx_bytes;
  uint64_t ipv6_tx_packets;
  uint64_t ipv6_tx_bytes;
  uint64_t ipv6_rx_packets;
  uint64_t ipv6_rx_bytes;
};
struct wan_ioctl_query_tether_stats_all {
  char upstreamIface[IFNAMSIZ];
  enum ipacm_client_enum ipa_client;
  uint8_t reset_stats;
  uint64_t tx_bytes;
  uint64_t rx_bytes;
};
struct wan_ioctl_reset_tether_stats {
  char upstreamIface[IFNAMSIZ];
  uint8_t reset_stats;
};
struct wan_ioctl_query_dl_filter_stats {
  uint8_t reset_stats;
  struct ipa_get_data_stats_resp_msg_v01 stats_resp;
  uint32_t index;
};
struct wan_ioctl_notify_wan_state {
  uint8_t up;
};
struct wan_ioctl_send_lan_client_msg {
  struct ipa_lan_client_msg lan_client;
  enum ipa_per_client_stats_event client_event;
};
struct wan_ioctl_lan_client_info {
  enum ipacm_per_client_device_type device_type;
  uint8_t mac[IPA_MAC_ADDR_SIZE];
  uint8_t client_init;
  int8_t client_idx;
  uint8_t hdr_len;
  enum ipa_client_type ul_src_pipe;
};
struct wan_ioctl_per_client_info {
  uint8_t mac[IPA_MAC_ADDR_SIZE];
  uint64_t ipv4_tx_bytes;
  uint64_t ipv4_rx_bytes;
  uint64_t ipv6_tx_bytes;
  uint64_t ipv6_rx_bytes;
};
struct wan_ioctl_query_per_client_stats {
  enum ipacm_per_client_device_type device_type;
  uint8_t reset_stats;
  uint8_t disconnect_clnt;
  uint8_t num_clients;
  struct wan_ioctl_per_client_info client_info[IPA_MAX_NUM_HW_PATH_CLIENTS];
};
#define WAN_IOC_ADD_FLT_RULE _IOWR(WAN_IOC_MAGIC, WAN_IOCTL_ADD_FLT_RULE, struct ipa_install_fltr_rule_req_msg_v01 *)
#define WAN_IOC_ADD_FLT_RULE_INDEX _IOWR(WAN_IOC_MAGIC, WAN_IOCTL_ADD_FLT_INDEX, struct ipa_fltr_installed_notif_req_msg_v01 *)
#define WAN_IOC_VOTE_FOR_BW_MBPS _IOWR(WAN_IOC_MAGIC, WAN_IOCTL_VOTE_FOR_BW_MBPS, uint32_t *)
#define WAN_IOC_POLL_TETHERING_STATS _IOWR(WAN_IOC_MAGIC, WAN_IOCTL_POLL_TETHERING_STATS, struct wan_ioctl_poll_tethering_stats *)
#define WAN_IOC_SET_DATA_QUOTA _IOWR(WAN_IOC_MAGIC, WAN_IOCTL_SET_DATA_QUOTA, struct wan_ioctl_set_data_quota *)
#define WAN_IOC_SET_TETHER_CLIENT_PIPE _IOWR(WAN_IOC_MAGIC, WAN_IOCTL_SET_TETHER_CLIENT_PIPE, struct wan_ioctl_set_tether_client_pipe *)
#define WAN_IOC_QUERY_TETHER_STATS _IOWR(WAN_IOC_MAGIC, WAN_IOCTL_QUERY_TETHER_STATS, struct wan_ioctl_query_tether_stats *)
#define WAN_IOC_RESET_TETHER_STATS _IOWR(WAN_IOC_MAGIC, WAN_IOCTL_RESET_TETHER_STATS, struct wan_ioctl_reset_tether_stats *)
#define WAN_IOC_QUERY_DL_FILTER_STATS _IOWR(WAN_IOC_MAGIC, WAN_IOCTL_QUERY_DL_FILTER_STATS, struct wan_ioctl_query_dl_filter_stats *)
#define WAN_IOC_ADD_FLT_RULE_EX _IOWR(WAN_IOC_MAGIC, WAN_IOCTL_ADD_FLT_RULE_EX, struct ipa_install_fltr_rule_req_ex_msg_v01 *)
#define WAN_IOC_QUERY_TETHER_STATS_ALL _IOWR(WAN_IOC_MAGIC, WAN_IOCTL_QUERY_TETHER_STATS_ALL, struct wan_ioctl_query_tether_stats_all *)
#define WAN_IOC_NOTIFY_WAN_STATE _IOWR(WAN_IOC_MAGIC, WAN_IOCTL_NOTIFY_WAN_STATE, struct wan_ioctl_notify_wan_state *)
#define WAN_IOC_ADD_UL_FLT_RULE _IOWR(WAN_IOC_MAGIC, WAN_IOCTL_ADD_UL_FLT_RULE, struct ipa_configure_ul_firewall_rules_req_msg_v01 *)
#define WAN_IOC_ENABLE_PER_CLIENT_STATS _IOWR(WAN_IOC_MAGIC, WAN_IOCTL_ENABLE_PER_CLIENT_STATS, bool *)
#define WAN_IOC_QUERY_PER_CLIENT_STATS _IOWR(WAN_IOC_MAGIC, WAN_IOCTL_QUERY_PER_CLIENT_STATS, struct wan_ioctl_query_per_client_stats *)
#define WAN_IOC_SET_LAN_CLIENT_INFO _IOWR(WAN_IOC_MAGIC, WAN_IOCTL_SET_LAN_CLIENT_INFO, struct wan_ioctl_lan_client_info *)
#define WAN_IOC_SEND_LAN_CLIENT_MSG _IOWR(WAN_IOC_MAGIC, WAN_IOCTL_SEND_LAN_CLIENT_MSG, struct wan_ioctl_send_lan_client_msg *)
#define WAN_IOC_CLEAR_LAN_CLIENT_INFO _IOWR(WAN_IOC_MAGIC, WAN_IOCTL_CLEAR_LAN_CLIENT_INFO, struct wan_ioctl_lan_client_info *)
#endif

