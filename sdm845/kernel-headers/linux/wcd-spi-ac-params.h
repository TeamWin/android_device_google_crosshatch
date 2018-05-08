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
#ifndef __UAPI_WCD_SPI_AC_PARAMS_H__
#define __UAPI_WCD_SPI_AC_PARAMS_H__
#include <linux/types.h>
#define WCD_SPI_AC_CMD_CONC_BEGIN 0x01
#define WCD_SPI_AC_CMD_CONC_END 0x02
#define WCD_SPI_AC_CMD_BUF_DATA 0x03
#define WCD_SPI_AC_MAX_BUFFERS 2
#define WCD_SPI_AC_MAX_CH_PER_BUF 8
#define WCD_SPI_AC_CLIENT_CDEV_NAME "wcd-spi-ac-client"
#define WCD_SPI_AC_PROCFS_DIR_NAME "wcd-spi-ac"
#define WCD_SPI_AC_PROCFS_STATE_NAME "svc-state"
struct wcd_spi_ac_buf_data {
  __u32 addr[WCD_SPI_AC_MAX_CH_PER_BUF];
} __attribute__((packed));
struct wcd_spi_ac_write_cmd {
  __u32 cmd_type;
  __u8 payload[0];
} __attribute__((packed));
#endif

