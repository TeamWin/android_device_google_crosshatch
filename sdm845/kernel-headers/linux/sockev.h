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
#ifndef _SOCKEV_H_
#define _SOCKEV_H_
#include <linux/types.h>
#include <linux/netlink.h>
#include <linux/socket.h>
enum sknetlink_groups {
  SKNLGRP_UNICAST,
  SKNLGRP_SOCKEV,
  __SKNLGRP_MAX
};
#define SOCKEV_STR_MAX 32
struct sknlsockevmsg {
  __u8 event[SOCKEV_STR_MAX];
  __u32 pid;
  __u16 skfamily;
  __u8 skstate;
  __u8 skprotocol;
  __u16 sktype;
  __u64 skflags;
};
#endif

