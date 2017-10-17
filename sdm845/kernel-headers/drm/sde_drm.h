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
#ifndef _SDE_DRM_H_
#define _SDE_DRM_H_
#include "drm.h"
#define SDE_MAX_PLANES 4
#define SDE_MAX_DE_CURVES 3
#define FILTER_EDGE_DIRECTED_2D 0x0
#define FILTER_CIRCULAR_2D 0x1
#define FILTER_SEPARABLE_1D 0x2
#define FILTER_BILINEAR 0x3
#define FILTER_ALPHA_DROP_REPEAT 0x0
#define FILTER_ALPHA_BILINEAR 0x1
#define FILTER_ALPHA_2D 0x3
#define FILTER_BLEND_CIRCULAR_2D 0x0
#define FILTER_BLEND_SEPARABLE_1D 0x1
#define SCALER_LUT_SWAP 0x1
#define SCALER_LUT_DIR_WR 0x2
#define SCALER_LUT_Y_CIR_WR 0x4
#define SCALER_LUT_UV_CIR_WR 0x8
#define SCALER_LUT_Y_SEP_WR 0x10
#define SCALER_LUT_UV_SEP_WR 0x20
#define SDE_DRM_BLEND_OP_NOT_DEFINED 0
#define SDE_DRM_BLEND_OP_OPAQUE 1
#define SDE_DRM_BLEND_OP_PREMULTIPLIED 2
#define SDE_DRM_BLEND_OP_COVERAGE 3
#define SDE_DRM_BLEND_OP_MAX 4
#define SDE_DRM_DEINTERLACE 0
#define SDE_DRM_BITMASK_COUNT 64
#define SDE_DRM_FB_NON_SEC 0
#define SDE_DRM_FB_SEC 1
#define SDE_DRM_FB_NON_SEC_DIR_TRANS 2
#define SDE_DRM_FB_SEC_DIR_TRANS 3
#define SDE_DRM_SEC_NON_SEC 0
#define SDE_DRM_SEC_ONLY 1
struct sde_drm_pix_ext_v1 {
  int32_t num_ext_pxls_lr[SDE_MAX_PLANES];
  int32_t num_ext_pxls_tb[SDE_MAX_PLANES];
  int32_t left_ftch[SDE_MAX_PLANES];
  int32_t right_ftch[SDE_MAX_PLANES];
  int32_t top_ftch[SDE_MAX_PLANES];
  int32_t btm_ftch[SDE_MAX_PLANES];
  int32_t left_rpt[SDE_MAX_PLANES];
  int32_t right_rpt[SDE_MAX_PLANES];
  int32_t top_rpt[SDE_MAX_PLANES];
  int32_t btm_rpt[SDE_MAX_PLANES];
};
struct sde_drm_scaler_v1 {
  struct sde_drm_pix_ext_v1 pe;
  int32_t init_phase_x[SDE_MAX_PLANES];
  int32_t phase_step_x[SDE_MAX_PLANES];
  int32_t init_phase_y[SDE_MAX_PLANES];
  int32_t phase_step_y[SDE_MAX_PLANES];
  uint32_t horz_filter[SDE_MAX_PLANES];
  uint32_t vert_filter[SDE_MAX_PLANES];
};
struct sde_drm_de_v1 {
  uint32_t enable;
  int16_t sharpen_level1;
  int16_t sharpen_level2;
  uint16_t clip;
  uint16_t limit;
  uint16_t thr_quiet;
  uint16_t thr_dieout;
  uint16_t thr_low;
  uint16_t thr_high;
  uint16_t prec_shift;
  int16_t adjust_a[SDE_MAX_DE_CURVES];
  int16_t adjust_b[SDE_MAX_DE_CURVES];
  int16_t adjust_c[SDE_MAX_DE_CURVES];
};
struct sde_drm_scaler_v2 {
  uint32_t enable;
  uint32_t dir_en;
  struct sde_drm_pix_ext_v1 pe;
  uint32_t horz_decimate;
  uint32_t vert_decimate;
  int32_t init_phase_x[SDE_MAX_PLANES];
  int32_t phase_step_x[SDE_MAX_PLANES];
  int32_t init_phase_y[SDE_MAX_PLANES];
  int32_t phase_step_y[SDE_MAX_PLANES];
  uint32_t preload_x[SDE_MAX_PLANES];
  uint32_t preload_y[SDE_MAX_PLANES];
  uint32_t src_width[SDE_MAX_PLANES];
  uint32_t src_height[SDE_MAX_PLANES];
  uint32_t dst_width;
  uint32_t dst_height;
  uint32_t y_rgb_filter_cfg;
  uint32_t uv_filter_cfg;
  uint32_t alpha_filter_cfg;
  uint32_t blend_cfg;
  uint32_t lut_flag;
  uint32_t dir_lut_idx;
  uint32_t y_rgb_cir_lut_idx;
  uint32_t uv_cir_lut_idx;
  uint32_t y_rgb_sep_lut_idx;
  uint32_t uv_sep_lut_idx;
  struct sde_drm_de_v1 de;
};
#define SDE_MAX_DS_COUNT 2
#define SDE_DRM_DESTSCALER_ENABLE 0x1
#define SDE_DRM_DESTSCALER_SCALE_UPDATE 0x2
#define SDE_DRM_DESTSCALER_ENHANCER_UPDATE 0x4
#define SDE_DRM_DESTSCALER_PU_ENABLE 0x8
struct sde_drm_dest_scaler_cfg {
  uint32_t flags;
  uint32_t index;
  uint32_t lm_width;
  uint32_t lm_height;
  uint64_t scaler_cfg;
};
struct sde_drm_dest_scaler_data {
  uint32_t num_dest_scaler;
  struct sde_drm_dest_scaler_cfg ds_cfg[SDE_MAX_DS_COUNT];
};
#define SDE_CSC_MATRIX_COEFF_SIZE 9
#define SDE_CSC_CLAMP_SIZE 6
#define SDE_CSC_BIAS_SIZE 3
struct sde_drm_csc_v1 {
  int64_t ctm_coeff[SDE_CSC_MATRIX_COEFF_SIZE];
  uint32_t pre_bias[SDE_CSC_BIAS_SIZE];
  uint32_t post_bias[SDE_CSC_BIAS_SIZE];
  uint32_t pre_clamp[SDE_CSC_CLAMP_SIZE];
  uint32_t post_clamp[SDE_CSC_CLAMP_SIZE];
};
struct sde_drm_color {
  uint32_t color_0;
  uint32_t color_1;
  uint32_t color_2;
  uint32_t color_3;
};
#define SDE_MAX_DIM_LAYERS 7
#define SDE_DRM_DIM_LAYER_INCLUSIVE 0x1
#define SDE_DRM_DIM_LAYER_EXCLUSIVE 0x2
struct sde_drm_dim_layer_cfg {
  uint32_t flags;
  uint32_t stage;
  struct sde_drm_color color_fill;
  struct drm_clip_rect rect;
};
struct sde_drm_dim_layer_v1 {
  uint32_t num_layers;
  struct sde_drm_dim_layer_cfg layer_cfg[SDE_MAX_DIM_LAYERS];
};
#define SDE_DRM_WB_CFG 0x1
#define SDE_DRM_WB_CFG_FLAGS_CONNECTED (1 << 0)
struct sde_drm_wb_cfg {
  uint32_t flags;
  uint32_t connector_id;
  uint32_t count_modes;
  uint64_t modes;
};
#define SDE_MAX_ROI_V1 4
struct sde_drm_roi_v1 {
  uint32_t num_rects;
  struct drm_clip_rect roi[SDE_MAX_ROI_V1];
};
#define SDE_MODE_DPMS_ON 0
#define SDE_MODE_DPMS_LP1 1
#define SDE_MODE_DPMS_LP2 2
#define SDE_MODE_DPMS_STANDBY 3
#define SDE_MODE_DPMS_SUSPEND 4
#define SDE_MODE_DPMS_OFF 5
#endif

