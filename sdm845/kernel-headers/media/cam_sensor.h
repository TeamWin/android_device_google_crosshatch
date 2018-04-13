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
#ifndef __UAPI_CAM_SENSOR_H__
#define __UAPI_CAM_SENSOR_H__
#include <linux/types.h>
#include <linux/ioctl.h>
#include <media/cam_defs.h>
#define CAM_SENSOR_PROBE_CMD (CAM_COMMON_OPCODE_MAX + 1)
#define CAM_FLASH_MAX_LED_TRIGGERS 3
#define MAX_OIS_NAME_SIZE 32
#define CAM_CSIPHY_SECURE_MODE_ENABLED 1
#define MAX_RAINBOW_CONFIG_SIZE 32
enum rainbow_op_type {
  RAINBOW_SEQ_READ,
  RAINBOW_RANDOM_READ,
  RAINBOW_SEQ_WRITE,
  RAINBOW_RANDOM_WRITE
};
struct rainbow_config {
  enum rainbow_op_type operation;
  uint32_t size;
  uint32_t reg_addr[MAX_RAINBOW_CONFIG_SIZE];
  uint32_t reg_data[MAX_RAINBOW_CONFIG_SIZE];
} __attribute__((packed));
#define RAINBOW_CONFIG _IOWR('R', 1, struct rainbow_config)
struct cam_sensor_query_cap {
  uint32_t slot_info;
  uint32_t secure_camera;
  uint32_t pos_pitch;
  uint32_t pos_roll;
  uint32_t pos_yaw;
  uint32_t actuator_slot_id;
  uint32_t eeprom_slot_id;
  uint32_t ois_slot_id;
  uint32_t flash_slot_id;
  uint32_t csiphy_slot_id;
} __attribute__((packed));
struct cam_csiphy_query_cap {
  uint32_t slot_info;
  uint32_t version;
  uint32_t clk_lane;
  uint32_t reserved;
} __attribute__((packed));
struct cam_actuator_query_cap {
  uint32_t slot_info;
  uint32_t reserved;
} __attribute__((packed));
struct cam_eeprom_query_cap_t {
  uint32_t slot_info;
  uint16_t eeprom_kernel_probe;
  uint16_t reserved;
} __attribute__((packed));
struct cam_ois_query_cap_t {
  uint32_t slot_info;
  uint16_t reserved;
} __attribute__((packed));
struct cam_cmd_i2c_info {
  uint16_t slave_addr;
  uint8_t i2c_freq_mode;
  uint8_t cmd_type;
} __attribute__((packed));
struct cam_cmd_get_ois_data {
  uint32_t reg_addr;
  uint32_t reg_data;
  uint64_t query_size_handle;
  uint64_t query_data_handle;
} __attribute__((packed));
struct cam_ois_shift {
  int16_t ois_shift_x;
  int16_t ois_shift_y;
  int64_t time_readout;
} __attribute__((packed));
struct cam_ois_opcode {
  uint32_t prog;
  uint32_t coeff;
  uint32_t pheripheral;
  uint32_t memory;
} __attribute__((packed));
struct cam_cmd_ois_info {
  uint16_t slave_addr;
  uint8_t i2c_freq_mode;
  uint8_t cmd_type;
  uint8_t ois_fw_flag;
  uint8_t is_ois_calib;
  char ois_name[MAX_OIS_NAME_SIZE];
  struct cam_ois_opcode opcode;
} __attribute__((packed));
struct cam_cmd_probe {
  uint8_t data_type;
  uint8_t addr_type;
  uint8_t op_code;
  uint8_t cmd_type;
  uint32_t reg_addr;
  uint32_t expected_data;
  uint32_t data_mask;
  uint16_t camera_id;
  uint8_t fw_update_flag;
  uint16_t reserved;
} __attribute__((packed));
struct cam_power_settings {
  uint16_t power_seq_type;
  uint16_t reserved;
  uint32_t config_val_low;
  uint32_t config_val_high;
} __attribute__((packed));
struct cam_cmd_power {
  uint16_t count;
  uint8_t reserved;
  uint8_t cmd_type;
  struct cam_power_settings power_settings[1];
} __attribute__((packed));
struct i2c_rdwr_header {
  uint16_t count;
  uint8_t op_code;
  uint8_t cmd_type;
  uint8_t data_type;
  uint8_t addr_type;
  uint16_t slave_addr;
} __attribute__((packed));
struct i2c_random_wr_payload {
  uint32_t reg_addr;
  uint32_t reg_data;
} __attribute__((packed));
struct cam_cmd_i2c_random_wr {
  struct i2c_rdwr_header header;
  struct i2c_random_wr_payload random_wr_payload[1];
} __attribute__((packed));
struct cam_cmd_read {
  uint32_t reg_data;
  uint32_t reserved;
} __attribute__((packed));
struct cam_cmd_i2c_continuous_wr {
  struct i2c_rdwr_header header;
  uint32_t reg_addr;
  struct cam_cmd_read data_read[1];
} __attribute__((packed));
struct cam_cmd_i2c_random_rd {
  struct i2c_rdwr_header header;
  struct cam_cmd_read data_read[1];
} __attribute__((packed));
struct cam_cmd_i2c_continuous_rd {
  struct i2c_rdwr_header header;
  uint32_t reg_addr;
} __attribute__((packed));
struct cam_cmd_conditional_wait {
  uint8_t data_type;
  uint8_t addr_type;
  uint8_t op_code;
  uint8_t cmd_type;
  uint16_t timeout;
  uint16_t reserved;
  uint32_t reg_addr;
  uint32_t reg_data;
  uint32_t data_mask;
} __attribute__((packed));
struct cam_cmd_unconditional_wait {
  int16_t delay;
  uint8_t op_code;
  uint8_t cmd_type;
} __attribute__((packed));
struct cam_csiphy_info {
  uint16_t lane_mask;
  uint16_t lane_assign;
  uint8_t csiphy_3phase;
  uint8_t combo_mode;
  uint8_t lane_cnt;
  uint8_t secure_mode;
  uint64_t settle_time;
  uint64_t data_rate;
} __attribute__((packed));
struct cam_csiphy_acquire_dev_info {
  uint32_t combo_mode;
  uint32_t reserved;
} __attribute__((packed));
struct cam_sensor_acquire_dev {
  uint32_t session_handle;
  uint32_t device_handle;
  uint32_t handle_type;
  uint32_t reserved;
  uint64_t info_handle;
} __attribute__((packed));
struct cam_sensor_streamon_dev {
  uint32_t session_handle;
  uint32_t device_handle;
  uint32_t handle_type;
  uint32_t reserved;
  uint64_t info_handle;
} __attribute__((packed));
struct cam_flash_init {
  uint8_t flash_type;
  uint16_t reserved;
  uint8_t cmd_type;
} __attribute__((packed));
struct cam_flash_set_rer {
  uint16_t count;
  uint8_t opcode;
  uint8_t cmd_type;
  uint16_t num_iteration;
  uint16_t reserved;
  uint32_t led_on_delay_ms;
  uint32_t led_off_delay_ms;
  uint32_t led_current_ma[CAM_FLASH_MAX_LED_TRIGGERS];
} __attribute__((packed));
struct cam_flash_set_on_off {
  uint16_t count;
  uint8_t opcode;
  uint8_t cmd_type;
  uint32_t led_current_ma[CAM_FLASH_MAX_LED_TRIGGERS];
} __attribute__((packed));
struct cam_flash_query_curr {
  uint16_t reserved;
  uint8_t opcode;
  uint8_t cmd_type;
  uint32_t query_current_ma;
} __attribute__((packed));
struct cam_flash_query_cap_info {
  uint32_t slot_info;
  uint32_t max_current_flash[CAM_FLASH_MAX_LED_TRIGGERS];
  uint32_t max_duration_flash[CAM_FLASH_MAX_LED_TRIGGERS];
  uint32_t max_current_torch[CAM_FLASH_MAX_LED_TRIGGERS];
} __attribute__((packed));
#endif

