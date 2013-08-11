/* Copyright (c) 2012, The Linux Foundation. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 and
 * only version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <linux/i2c.h>
#include <linux/i2c/sx150x.h>
#include <linux/gpio.h>
#include <linux/regulator/consumer.h>
#include <linux/kernel.h>
#include <linux/platform_device.h>
#include <linux/module.h>
#include <asm/mach-types.h>
#include <mach/msm_iomap.h>
#include <mach/board.h>
#include <mach/irqs-7xxx.h>
#include "devices-msm7x2xa.h"
#include "board-msm7627a.h"
#include <mach/vreg.h>

#define GPIO_BACK_CAM_SHDN_N   16      /* PWDN */
#define GPIO_BACK_CAM_RESET   79
#define GPIO_BACK_CAM_VCM_PWDN 5 
#define GPIO_FRONT_CAM_SHDN 85

#define GPIO_SKUD_CAM_LED_EN 34
#define GPIO_SKUD_CAM_LED_FLASH_EN 48
#define GPIO_SKUD_CAM_5MP_CAMID 127 /*CAMID*/

#define GPIO_NOT_CONFIGURED -1
#define MOUNT_ANGLE_NOT_CONFIGURED -1

#ifdef CONFIG_MSM_CAMERA_V4L2
static uint32_t camera_off_gpio_table[] = {
	GPIO_CFG(15, 0, GPIO_CFG_OUTPUT, GPIO_CFG_PULL_DOWN, GPIO_CFG_2MA),
};

static uint32_t camera_on_gpio_table[] = {
	GPIO_CFG(15, 1, GPIO_CFG_OUTPUT, GPIO_CFG_PULL_DOWN, GPIO_CFG_2MA),
};

#if 0
static struct gpio s5k4e1_cam_req_gpio[] = {
	{GPIO_CAM_GP_CAMIF_RESET_N, GPIOF_DIR_OUT, "CAM_RESET"},
};

static struct msm_gpio_set_tbl s5k4e1_cam_gpio_set_tbl[] = {
	{GPIO_CAM_GP_CAMIF_RESET_N, GPIOF_OUT_INIT_LOW, 1000},
	{GPIO_CAM_GP_CAMIF_RESET_N, GPIOF_OUT_INIT_HIGH, 4000},
};
#endif

#ifdef CONFIG_S5K4E1
static struct msm_camera_gpio_conf gpio_conf_s5k4e1 = {
	.camera_off_table = camera_off_gpio_table,
	.camera_off_table_size = ARRAY_SIZE(camera_off_gpio_table),
	.camera_on_table = camera_on_gpio_table,
	.camera_on_table_size = ARRAY_SIZE(camera_on_gpio_table),
#if 0
	.cam_gpio_req_tbl = s5k4e1_cam_req_gpio,
	.cam_gpio_req_tbl_size = ARRAY_SIZE(s5k4e1_cam_req_gpio),
	.cam_gpio_set_tbl = s5k4e1_cam_gpio_set_tbl,
	.cam_gpio_set_tbl_size = ARRAY_SIZE(s5k4e1_cam_gpio_set_tbl),
#endif
	.gpio_no_mux = 1,
};
#endif

#ifdef CONFIG_WEBCAM_OV9726
static struct msm_camera_gpio_conf gpio_conf_ov9726 = {
	.camera_off_table = camera_off_gpio_table,
	.camera_on_table = camera_on_gpio_table,
	.gpio_no_mux = 1,
};

#endif
#ifdef CONFIG_OV7692
static struct gpio ov7692_cam_req_gpio[] = {
	{GPIO_SKU1_CAM_VGA_RESET_N, GPIOF_DIR_OUT, "CAM_VGA_RESET"},
};

static struct gpio ov7692_cam_req_gpio_sku7[] = {
	{GPIO_SKU7_CAM_VGA_SHDN, GPIOF_DIR_OUT, "CAM_VGA_SHDN"},
	{GPIO_SKU1_CAM_VGA_RESET_N, GPIOF_DIR_OUT, "CAM_VGA_RESET"},
};

static struct msm_gpio_set_tbl ov7692_cam_gpio_set_tbl[] = {
	{GPIO_SKU1_CAM_VGA_SHDN, GPIOF_OUT_INIT_HIGH, 5000},
	{GPIO_SKU1_CAM_VGA_SHDN, GPIOF_OUT_INIT_LOW, 5000},
	{GPIO_SKU1_CAM_VGA_RESET_N, GPIOF_OUT_INIT_HIGH, 5000},
	{GPIO_SKU1_CAM_VGA_RESET_N, GPIOF_OUT_INIT_LOW, 5000},
};

static struct msm_gpio_set_tbl ov7692_cam_gpio_set_tbl_sku7[] = {
	{GPIO_SKU7_CAM_VGA_SHDN, GPIOF_OUT_INIT_HIGH, 5000},
	{GPIO_SKU7_CAM_VGA_SHDN, GPIOF_OUT_INIT_LOW, 5000},
	{GPIO_SKU1_CAM_VGA_RESET_N, GPIOF_OUT_INIT_HIGH, 5000},
	{GPIO_SKU1_CAM_VGA_RESET_N, GPIOF_OUT_INIT_LOW, 5000},
};

static struct msm_camera_gpio_conf gpio_conf_ov7692 = {
	.cam_gpio_req_tbl = ov7692_cam_req_gpio,
	.cam_gpio_req_tbl_size = ARRAY_SIZE(ov7692_cam_req_gpio),
	.cam_gpio_set_tbl = ov7692_cam_gpio_set_tbl,
	.cam_gpio_set_tbl_size = ARRAY_SIZE(ov7692_cam_gpio_set_tbl),
	.gpio_no_mux = 1,
};

static struct msm_camera_gpio_conf sku7_gpio_conf_ov7692 = {
	.cam_gpio_req_tbl = ov7692_cam_req_gpio_sku7,
	.cam_gpio_req_tbl_size = ARRAY_SIZE(ov7692_cam_req_gpio_sku7),
	.cam_gpio_set_tbl = ov7692_cam_gpio_set_tbl_sku7,
	.cam_gpio_set_tbl_size = ARRAY_SIZE(ov7692_cam_gpio_set_tbl_sku7),
	.gpio_no_mux = 1,
};
#endif

#ifdef CONFIG_OV5647
static struct msm_camera_gpio_conf gpio_conf_ov5647 = {
	.camera_off_table = camera_off_gpio_table,
	.camera_on_table = camera_on_gpio_table,
	.gpio_no_mux = 1,
};
#endif
#ifdef CONFIG_OV5647_TRULY_CM6868
static struct msm_camera_gpio_conf gpio_conf_ov5647_truly_cm6868 = {
	.camera_off_table = camera_off_gpio_table,
	.camera_on_table = camera_on_gpio_table,
	.gpio_no_mux = 1,
};
#endif

#ifdef CONFIG_AR0543
static struct msm_camera_gpio_conf gpio_conf_ar0543 = {
	.camera_off_table = camera_off_gpio_table,
	.camera_on_table = camera_on_gpio_table,
	.gpio_no_mux = 1,
};
#endif
#ifdef CONFIG_S5K3H2_SUNNY_Q8S02E
static struct msm_camera_gpio_conf gpio_conf_s5k3h2_sunny_q8s02e = {
	.camera_off_table = camera_off_gpio_table,
	.camera_on_table = camera_on_gpio_table,
	.gpio_no_mux = 1,
};
#endif

#ifdef CONFIG_AR0542
static struct msm_camera_gpio_conf gpio_conf_ar0542 = {
	.camera_off_table = camera_off_gpio_table,
	.camera_on_table = camera_on_gpio_table,
	.gpio_no_mux = 1,
};
#endif

#ifdef CONFIG_OV5648_TRULY_CM8352
static struct msm_camera_gpio_conf gpio_conf_ov5648_truly_cm8352 = {
	.camera_off_table = camera_off_gpio_table,
	.camera_on_table = camera_on_gpio_table,
	.gpio_no_mux = 1,
};
#endif




#ifdef CONFIG_OV5647
static struct camera_vreg_t ov5647_gpio_vreg[] = {
	{"cam_ov5647_avdd", REG_GPIO, 0, 0, 0},
	{"cam_ov5647_vdd", REG_GPIO, 0, 0, 0},
};
#endif
#ifdef CONFIG_OV5647_TRULY_CM6868
static struct camera_vreg_t ov5647_truly_cm6868_gpio_vreg[] = {
	{"cam_ov5647_truly_cm6868_avdd", REG_GPIO, 0, 0, 0},
	{"cam_ov5647_truly_cm6868_vdd", REG_GPIO, 0, 0, 0},
};
#endif
#ifdef CONFIG_OV5648_TRULY_CM8352
static struct camera_vreg_t ov5648_truly_cm8352_gpio_vreg[] = {
	{"ldo12", REG_LDO, 2700000, 3300000, 0},
	{"smps3", REG_LDO, 1800000, 1800000, 0},
};

static struct camera_vreg_t ov5648_gpio_vreg_evbd[] = {
	{"ldo12", REG_LDO, 2700000, 3300000, 0},
	{"smps3", REG_LDO, 1800000, 1800000, 0},
	{"cam_ov5648_avdd", REG_GPIO, 0, 0, 0},
	{"cam_ov5648_vdd", REG_GPIO, 0, 0, 0},
};
#endif
#ifdef CONFIG_OV8825
static struct camera_vreg_t ov8825_gpio_vreg[] = {
	{"cam_ov8825_avdd", REG_GPIO, 0, 0, 0},
	{"cam_ov8825_vdd", REG_GPIO, 0, 0, 0},
};
#endif
#ifdef CONFIG_OV7692
static struct camera_vreg_t ov7692_gpio_vreg[] = {
	{"cam_ov7692_avdd", REG_GPIO, 0, 0, 0},
	{"cam_ov7692_vdd", REG_GPIO, 0, 0, 0},
};
#endif

#ifdef CONFIG_AR0543
static struct camera_vreg_t ar0543_gpio_vreg[] = {
	{"cam_ar0543_avdd", REG_GPIO, 0, 0, 0},
	{"cam_ar0543_vdd",  REG_GPIO, 0, 0, 0},
};
#endif
#ifdef CONFIG_S5K3H2_SUNNY_Q8S02E
static struct camera_vreg_t s5k3h2_sunny_q8s02e_gpio_vreg[] = {
	{"cam_s5k3h2_sunny_q8s02e_avdd", REG_GPIO, 0, 0, 0},
	{"cam_s5k3h2_sunny_q8s02e_vdd",  REG_GPIO, 0, 0, 0},
};
#endif
#ifdef CONFIG_AR0542
#endif

#ifdef CONFIG_S5K4E1
static struct msm_camera_sensor_info msm_camera_sensor_s5k4e1_data;
#endif 
#ifdef CONFIG_GC0339
static struct msm_camera_sensor_info msm_camera_sensor_gc0339_data;
#endif

struct msm_camera_device_platform_data msm_camera_device_data_csi1[] = {
	{
		.csid_core = 1,
		.ioclk = {
			.vfe_clk_rate = 192000000,
		},
	},
	{
		.csid_core = 1,
		.ioclk = {
			.vfe_clk_rate = 266667000,
		},
	},
};

struct msm_camera_device_platform_data msm_camera_device_data_csi0[] = {
	{
		.csid_core = 0,
		.ioclk = {
			.vfe_clk_rate = 192000000,
		},
	},
	{
		.csid_core = 0,
		.ioclk = {
			.vfe_clk_rate = 266667000,
		},
	},
};

#if 0
static struct i2c_board_info msm_act_main_cam_i2c_info = {
	I2C_BOARD_INFO("msm_actuator", 0x11),
};

static struct msm_actuator_info msm_act_main_cam_4_info = {
	.board_info     = &msm_act_main_cam_i2c_info,
	.cam_name   = MSM_ACTUATOR_MAIN_CAM_4,
	.bus_id         = MSM_GSBI0_QUP_I2C_BUS_ID,
	.vcm_pwd        = GPIO_CAM_GP_CAM_PWDN,
	.vcm_enable     = 1,
};
#endif


#ifdef CONFIG_S5K4E1
static struct msm_camera_sensor_flash_data flash_s5k4e1 = {
	.flash_type             = MSM_CAMERA_FLASH_LED,
	.flash_src              = &msm_flash_src
};

static struct msm_camera_sensor_platform_info sensor_board_info_s5k4e1 = {
	.mount_angle	= 90,
	.cam_vreg = msm_cam_vreg,
	.num_vreg = ARRAY_SIZE(msm_cam_vreg),
	.gpio_conf = &gpio_conf_s5k4e1,
};

static struct msm_camera_sensor_info msm_camera_sensor_s5k4e1_data = {
	.sensor_name    = "s5k4e1",
	.sensor_reset_enable = 1,
	.pdata                  = &msm_camera_device_data_csi1[0],
	.flash_data             = &flash_s5k4e1,
	.sensor_platform_info   = &sensor_board_info_s5k4e1,
	.csi_if                 = 1,
	.camera_type = BACK_CAMERA_2D,
	.sensor_type = BAYER_SENSOR,
	//.actuator_info = &msm_act_main_cam_4_info,
};
#endif

#ifdef CONFIG_GC0339
static struct camera_vreg_t gc0339_gpio_vreg[] = {
};

static struct msm_camera_gpio_conf gpio_conf_gc0339 = {
	.camera_off_table = camera_off_gpio_table,
	.camera_off_table_size = ARRAY_SIZE(camera_off_gpio_table),
	.camera_on_table = camera_on_gpio_table,
	.camera_on_table_size = ARRAY_SIZE(camera_on_gpio_table),
	.gpio_no_mux = 1,
};

static struct msm_camera_sensor_platform_info sensor_board_info_gc0339 = {
	.mount_angle = 90,
	.cam_vreg = gc0339_gpio_vreg,
	.num_vreg = ARRAY_SIZE(gc0339_gpio_vreg),
	.gpio_conf = &gpio_conf_gc0339,

};

static struct msm_camera_sensor_flash_data flash_gc0339 = {
	.flash_type     = MSM_CAMERA_FLASH_NONE,
};

static struct msm_camera_sensor_info msm_camera_sensor_gc0339_data = {
	.sensor_name	    = "gc0339",
	.sensor_reset_enable    = 0,
	.sensor_reset	   = -1,
	.sensor_pwd	     = GPIO_FRONT_CAM_SHDN,
	.pdata			= &msm_camera_device_data_csi0[0],
	.flash_data	     = &flash_gc0339,
	.sensor_platform_info   = &sensor_board_info_gc0339,
	.csi_if		 = 1,
	.camera_type = FRONT_CAMERA_2D,
	.sensor_type = BAYER_SENSOR,
};
#endif

#ifdef CONFIG_OV7692
static struct msm_camera_sensor_platform_info sensor_board_info_ov7692 = {
	.mount_angle = 270,
	.cam_vreg = msm_cam_vreg,
	.num_vreg = ARRAY_SIZE(msm_cam_vreg),
	.gpio_conf = &gpio_conf_ov7692,
};

static struct msm_camera_sensor_flash_data flash_ov7692 = {
	.flash_type     = MSM_CAMERA_FLASH_NONE,
};

static struct msm_camera_sensor_info msm_camera_sensor_ov7692_data = {
	.sensor_name	    = "ov7692",
	.sensor_reset_enable    = 0,
	.sensor_reset	   = GPIO_NOT_CONFIGURED,
	.sensor_pwd	     = GPIO_NOT_CONFIGURED,
	.pdata			= &msm_camera_device_data_csi0[0],
	.flash_data	     = &flash_ov7692,
	.sensor_platform_info   = &sensor_board_info_ov7692,
	.csi_if		 = 1,
	.camera_type = FRONT_CAMERA_2D,
	.sensor_type = YUV_SENSOR,
};
#endif

#ifdef CONFIG_OV7695
static struct gpio ov7695_cam_req_gpio_skud[] = {
	{GPIO_FRONT_CAM_SHDN, GPIOF_DIR_OUT, "CAM_VGA_SHDN"},
};

static struct msm_gpio_set_tbl ov7695_cam_gpio_set_tbl_skud[] = {
	{GPIO_FRONT_CAM_SHDN, GPIOF_OUT_INIT_LOW, 5000},
	{GPIO_FRONT_CAM_SHDN, GPIOF_OUT_INIT_HIGH, 5000},
};
static struct msm_camera_gpio_conf skud_gpio_conf_ov7695 = {
	.cam_gpio_req_tbl = ov7695_cam_req_gpio_skud,
	.cam_gpio_req_tbl_size = ARRAY_SIZE(ov7695_cam_req_gpio_skud),
	.cam_gpio_set_tbl = ov7695_cam_gpio_set_tbl_skud,
	.cam_gpio_set_tbl_size = ARRAY_SIZE(ov7695_cam_gpio_set_tbl_skud),
	.camera_off_table = camera_off_gpio_table,
	.camera_on_table = camera_on_gpio_table,
	.gpio_no_mux = 1,
};

static struct camera_vreg_t ov7695_gpio_vreg[] = {
	{"ldo12", REG_LDO, 2700000, 3300000, 0},
	{"smps3", REG_LDO, 1800000, 1800000, 0},
};

static struct camera_vreg_t ov7695_gpio_vreg_evbd[] = {
	{"ldo12", REG_LDO, 2700000, 3300000, 0},
	{"smps3", REG_LDO, 1800000, 1800000, 0},
	{"cam_ov7695_avdd", REG_GPIO, 0, 0, 0},
	{"cam_ov7695_vdd", REG_GPIO, 0, 0, 0},
};
static struct msm_camera_sensor_platform_info sensor_board_info_ov7695 = {
	.mount_angle = 90,
	.cam_vreg = msm_cam_vreg,
	.num_vreg = ARRAY_SIZE(msm_cam_vreg),
	.gpio_conf = &skud_gpio_conf_ov7695,
};

static struct msm_camera_sensor_flash_data flash_ov7695 = {
	.flash_type     = MSM_CAMERA_FLASH_NONE,
};

static struct msm_camera_sensor_info msm_camera_sensor_ov7695_data = {
	.sensor_name	    = "ov7695",
	.sensor_reset_enable    = 0,
	.sensor_reset	   = GPIO_NOT_CONFIGURED,
	.sensor_pwd	     = GPIO_NOT_CONFIGURED,
	.pdata			= &msm_camera_device_data_csi0[0],
	.flash_data	     = &flash_ov7695,
	.sensor_platform_info   = &sensor_board_info_ov7695,
	.csi_if		 = 1,
	.camera_type = FRONT_CAMERA_2D,
	.sensor_type = YUV_SENSOR,
};
#endif

#ifdef CONFIG_OV7695_RAW
static struct msm_camera_gpio_conf skud_gpio_conf_ov7695_raw = {
	.camera_off_table = camera_off_gpio_table,
	.camera_on_table = camera_on_gpio_table,
	.gpio_no_mux = 1,
};

static struct camera_vreg_t ov7695_raw_gpio_vreg[] = {
	{"ldo12", REG_LDO, 2700000, 3300000, 0},
	{"smps3", REG_LDO, 1800000, 1800000, 0},
};
static struct msm_camera_sensor_platform_info sensor_board_info_ov7695_raw = {
	.mount_angle = 90,
	.cam_vreg = msm_cam_vreg,
	.num_vreg = ARRAY_SIZE(msm_cam_vreg),
	.gpio_conf = &skud_gpio_conf_ov7695_raw,
};

static struct msm_camera_sensor_flash_data flash_ov7695_raw = {
	.flash_type     = MSM_CAMERA_FLASH_NONE,
};

static struct msm_camera_sensor_info msm_camera_sensor_ov7695_raw_data = {
	.sensor_name	    = "ov7695_raw",
	.sensor_reset_enable    = 0,
	.sensor_reset	   = GPIO_NOT_CONFIGURED,
	.sensor_pwd	     = GPIO_NOT_CONFIGURED,
	.pdata			= &msm_camera_device_data_csi1[0],//&msm_camera_device_data_csi0[0],
	.flash_data	     = &flash_ov7695_raw,
	.sensor_platform_info   = &sensor_board_info_ov7695_raw,
	.csi_if		 = 1,
	.camera_type = FRONT_CAMERA_2D,
	.sensor_type = BAYER_SENSOR,
};
#endif

#ifdef CONFIG_MT9V115
static struct gpio mt9v115_cam_req_gpio[] = {
	{GPIO_SKUE_CAM_VGA_SHDN, GPIOF_DIR_OUT, "CAM_VGA_SHDN"},
};

static struct msm_gpio_set_tbl mt9v115_cam_gpio_set_tbl[] = {
	{GPIO_SKUE_CAM_VGA_SHDN, GPIOF_OUT_INIT_HIGH, 5000},
	{GPIO_SKUE_CAM_VGA_SHDN, GPIOF_OUT_INIT_LOW, 10000},
};
static struct msm_camera_gpio_conf gpio_conf_mt9v115 = {
	.cam_gpio_req_tbl = mt9v115_cam_req_gpio,
	.cam_gpio_req_tbl_size = ARRAY_SIZE(mt9v115_cam_req_gpio),
	.cam_gpio_set_tbl = mt9v115_cam_gpio_set_tbl,
	.cam_gpio_set_tbl_size = ARRAY_SIZE(mt9v115_cam_gpio_set_tbl),
	.camera_off_table = camera_off_gpio_table,
	.camera_on_table = camera_on_gpio_table,
	.gpio_no_mux = 1,
};

static struct camera_vreg_t mt9v115_gpio_vreg[] = {
	{"ldo12", REG_LDO, 2700000, 3300000, 0},
	{"smps3", REG_LDO, 1800000, 1800000, 0},
};
static struct msm_camera_sensor_platform_info sensor_board_info_mt9v115 = {
	.mount_angle = 90,
	.cam_vreg = msm_cam_vreg,
	.num_vreg = ARRAY_SIZE(msm_cam_vreg),
	.gpio_conf = &gpio_conf_mt9v115,
};

static struct msm_camera_sensor_flash_data flash_mt9v115 = {
	.flash_type     = MSM_CAMERA_FLASH_NONE,
};

static struct msm_camera_sensor_info msm_camera_sensor_mt9v115_data = {
	.sensor_name	    = "mt9v115",
	.sensor_reset_enable    = 0,
	.sensor_reset	   = GPIO_NOT_CONFIGURED,
	.sensor_pwd	     = GPIO_NOT_CONFIGURED,
	.pdata			= &msm_camera_device_data_csi0[0],
	.flash_data	     = &flash_mt9v115,
	.sensor_platform_info   = &sensor_board_info_mt9v115,
	.csi_if		 = 1,
	.camera_type = FRONT_CAMERA_2D,
	.sensor_type = YUV_SENSOR,
};
#endif

#ifdef CONFIG_OV5647

static struct msm_actuator_info msm_act_main_cam_5_info = {
	.board_info     = &msm_act_main_cam_i2c_info,
	.cam_name   = MSM_ACTUATOR_MAIN_CAM_5,
	.bus_id         = MSM_GSBI0_QUP_I2C_BUS_ID,
	.vcm_pwd        = GPIO_NOT_CONFIGURED,
	.vcm_enable     = 0,
};

static struct msm_camera_sensor_platform_info sensor_board_info_ov5647 = {
	.mount_angle = MOUNT_ANGLE_NOT_CONFIGURED,
	.cam_vreg = msm_cam_vreg,
	.num_vreg = ARRAY_SIZE(msm_cam_vreg),
	.gpio_conf = &gpio_conf_ov5647,
};

static struct msm_camera_sensor_flash_src msm_flash_src_ov5647 = {
	.flash_sr_type = MSM_CAMERA_FLASH_SRC_LED1,
	._fsrc.ext_driver_src.led_en = 13,
	._fsrc.ext_driver_src.led_flash_en = 32,
};

static struct msm_camera_sensor_flash_data flash_ov5647 = {
	.flash_type             = MSM_CAMERA_FLASH_LED,
	.flash_src              = &msm_flash_src_ov5647,
};

static struct msm_camera_sensor_info msm_camera_sensor_ov5647_data = {
	.sensor_name    = "ov5647",
	.sensor_reset_enable = 1,
	.pmic_gpio_enable  = 1,
	.sensor_reset   = GPIO_NOT_CONFIGURED,
	.sensor_pwd     = GPIO_NOT_CONFIGURED,
	.pdata			= &msm_camera_device_data_csi1[0],
	.flash_data     = &flash_ov5647,
	.sensor_platform_info   = &sensor_board_info_ov5647,
	.csi_if                 = 1,
	.camera_type	= BACK_CAMERA_2D,
	.sensor_type = BAYER_SENSOR,
	.actuator_info = &msm_act_main_cam_5_info,
};
#endif

#ifdef CONFIG_OV5647_TRULY_CM6868
static struct msm_actuator_info msm_act_main_cam_6_info = {
	.board_info     = &msm_act_main_cam_i2c_info,
	.cam_name   = MSM_ACTUATOR_MAIN_CAM_6,
	.bus_id         = MSM_GSBI0_QUP_I2C_BUS_ID,
	.vcm_pwd        = GPIO_NOT_CONFIGURED,
	.vcm_enable     = 0,
};

static struct msm_camera_sensor_platform_info sensor_board_info_ov5647_truly_cm6868 = {
	.mount_angle = MOUNT_ANGLE_NOT_CONFIGURED,
	.cam_vreg = msm_cam_vreg,
	.num_vreg = ARRAY_SIZE(msm_cam_vreg),
	.gpio_conf = &gpio_conf_ov5647_truly_cm6868,
};

static struct msm_camera_sensor_flash_src msm_flash_src_ov5647_truly_cm6868 = {
	.flash_sr_type = MSM_CAMERA_FLASH_SRC_LED1,
	._fsrc.ext_driver_src.led_en = 13,
	._fsrc.ext_driver_src.led_flash_en = 32,
};

static struct msm_camera_sensor_flash_data flash_ov5647_truly_cm6868 = {
	.flash_type             = MSM_CAMERA_FLASH_LED,
	.flash_src              = &msm_flash_src_ov5647_truly_cm6868,
};

static struct msm_camera_sensor_info msm_camera_sensor_ov5647_truly_cm6868_data = {
	.sensor_name    = "ov5647_truly_cm6868",
	.sensor_reset_enable = 1,
	.pmic_gpio_enable  = 1,
	.sensor_reset   = GPIO_NOT_CONFIGURED,
	.sensor_pwd     = GPIO_NOT_CONFIGURED,
	.pdata          = &msm_camera_device_data_csi1[0],
	.flash_data     = &flash_ov5647_truly_cm6868,
	.sensor_platform_info   = &sensor_board_info_ov5647_truly_cm6868,
	.csi_if                 = 1,
	.camera_type	= BACK_CAMERA_2D,
	.sensor_type = BAYER_SENSOR,
	.actuator_info = &msm_act_main_cam_6_info,
};
#endif

#ifdef CONFIG_OV5648_TRULY_CM8352
static struct msm_actuator_info msm_act_main_cam_7_info = {
	.board_info     = &msm_act_main_cam_i2c_info,
	.cam_name   = MSM_ACTUATOR_MAIN_CAM_7,
	.bus_id         = MSM_GSBI0_QUP_I2C_BUS_ID,
	.vcm_pwd        = GPIO_NOT_CONFIGURED,
	.vcm_enable     = 0,
};

static struct msm_camera_sensor_platform_info sensor_board_info_ov5648_truly_cm8352 = {
	.mount_angle = MOUNT_ANGLE_NOT_CONFIGURED,
	.cam_vreg = msm_cam_vreg,
	.num_vreg = ARRAY_SIZE(msm_cam_vreg),
	.gpio_conf = &gpio_conf_ov5648_truly_cm8352,
};

static struct msm_camera_sensor_flash_src msm_flash_src_ov5648_truly_cm8352 = {
	.flash_sr_type = MSM_CAMERA_FLASH_SRC_LED1,
	._fsrc.ext_driver_src.led_en =  GPIO_SKUD_CAM_LED_EN,
	._fsrc.ext_driver_src.led_flash_en = GPIO_SKUD_CAM_LED_FLASH_EN,
};

static struct msm_camera_sensor_flash_data flash_ov5648_truly_cm8352 = {
	.flash_type             = MSM_CAMERA_FLASH_LED,
	.flash_src              = &msm_flash_src_ov5648_truly_cm8352,
};

static struct msm_camera_sensor_info msm_camera_sensor_ov5648_truly_cm8352_data = {
	.sensor_name    = "ov5648_truly_cm8352",
	.sensor_reset_enable = 1,
	.pmic_gpio_enable  = 1,
	.sensor_reset   = GPIO_NOT_CONFIGURED,
	.sensor_pwd     = GPIO_NOT_CONFIGURED,
	.pdata          = &msm_camera_device_data_csi1[0],
	.flash_data     = &flash_ov5648_truly_cm8352,
	.sensor_platform_info   = &sensor_board_info_ov5648_truly_cm8352,
	.csi_if                 = 1,
	.camera_type	= BACK_CAMERA_2D,
	.sensor_type = BAYER_SENSOR,
	.actuator_info = &msm_act_main_cam_7_info,
};
#endif


#ifdef CONFIG_OV8825
static struct msm_camera_gpio_conf gpio_conf_ov8825 = {
	.camera_off_table = camera_off_gpio_table,
	.camera_on_table = camera_on_gpio_table,
	.gpio_no_mux = 1,
};

static struct msm_camera_sensor_flash_src msm_flash_src_ov8825 = {
	.flash_sr_type = MSM_CAMERA_FLASH_SRC_LED1,
	._fsrc.ext_driver_src.led_en = 13,
	._fsrc.ext_driver_src.led_flash_en = 32,
};

static struct msm_camera_sensor_flash_data flash_ov8825 = {
	.flash_type     = MSM_CAMERA_FLASH_LED,
	.flash_src      = &msm_flash_src_ov8825,
};

static struct msm_camera_sensor_platform_info sensor_board_info_ov8825 = {
	.mount_angle  = MOUNT_ANGLE_NOT_CONFIGURED,
	.cam_vreg = msm_cam_vreg,
	.num_vreg = ARRAY_SIZE(msm_cam_vreg),
	.gpio_conf = &gpio_conf_ov8825,
};

static struct msm_actuator_info msm_act_main_cam_3_info = {
	.board_info     = &msm_act_main_cam_i2c_info,
	.cam_name   = MSM_ACTUATOR_MAIN_CAM_3,
	.bus_id         = MSM_GSBI0_QUP_I2C_BUS_ID,
	.vcm_pwd        = GPIO_NOT_CONFIGURED,
	.vcm_enable     = 0,
};

static struct msm_camera_sensor_info msm_camera_sensor_ov8825_data = {
	.sensor_name    = "ov8825",
	.sensor_reset_enable    = 1,
	.pmic_gpio_enable = 1,
	.sensor_reset           = GPIO_NOT_CONFIGURED,
	.sensor_pwd     = GPIO_NOT_CONFIGURED,
	.pdata  = &msm_camera_device_data_csi1[1],
	.flash_data     = &flash_ov8825,
	.sensor_platform_info = &sensor_board_info_ov8825,
	.csi_if = 1,
	.camera_type = BACK_CAMERA_2D,
	.sensor_type = BAYER_SENSOR,
	.actuator_info = &msm_act_main_cam_3_info,
};
#endif


#ifdef CONFIG_AR0543
static struct msm_camera_sensor_platform_info sensor_board_info_ar0543 = {
  .mount_angle            = MOUNT_ANGLE_NOT_CONFIGURED,
  .cam_vreg               = msm_cam_vreg,
  .num_vreg               = ARRAY_SIZE(msm_cam_vreg),
  .gpio_conf              = &gpio_conf_ar0543,
};

static struct msm_camera_sensor_flash_src msm_flash_src_ar0543 = {
  .flash_sr_type                     = MSM_CAMERA_FLASH_SRC_LED1,
  ._fsrc.ext_driver_src.led_en       = 13,
  ._fsrc.ext_driver_src.led_flash_en = 32,
};

static struct msm_camera_sensor_flash_data flash_ar0543 = {
  .flash_type             = MSM_CAMERA_FLASH_LED,
  .flash_src              = &msm_flash_src_ar0543,
};

static struct msm_camera_sensor_info msm_camera_sensor_ar0543_data = {
  .sensor_name            = "ar0543",
  .sensor_reset_enable    = 1,
  .pmic_gpio_enable       = 1,
  .sensor_reset           = GPIO_NOT_CONFIGURED,
  .sensor_pwd             = GPIO_NOT_CONFIGURED,
  .pdata                  = &msm_camera_device_data_csi1[0],
  .flash_data             = &flash_ar0543,
  .sensor_platform_info   = &sensor_board_info_ar0543,
  .csi_if                 = 1,
  .camera_type            = BACK_CAMERA_2D,
  .sensor_type            = BAYER_SENSOR,
};
#endif

#ifdef CONFIG_S5K3H2_SUNNY_Q8S02E
static struct msm_actuator_info msm_act_main_cam_9_info = {
  .board_info             = &msm_act_main_cam_i2c_info,
  .cam_name               = MSM_ACTUATOR_MAIN_CAM_9,
  .bus_id                 = MSM_GSBI0_QUP_I2C_BUS_ID,
  .vcm_pwd                = GPIO_NOT_CONFIGURED,
  .vcm_enable             = 0,
};

static struct msm_camera_sensor_platform_info sensor_board_info_s5k3h2_sunny_q8s02e = {
  .mount_angle            = MOUNT_ANGLE_NOT_CONFIGURED,
  .cam_vreg               = msm_cam_vreg,
  .num_vreg               = ARRAY_SIZE(msm_cam_vreg),
  .gpio_conf              = &gpio_conf_s5k3h2_sunny_q8s02e,
};

static struct msm_camera_sensor_flash_src msm_flash_src_s5k3h2_sunny_q8s02e = {
  .flash_sr_type                     = MSM_CAMERA_FLASH_SRC_LED1,
  ._fsrc.ext_driver_src.led_en       = 13,
  ._fsrc.ext_driver_src.led_flash_en = 32,
};

static struct msm_camera_sensor_flash_data flash_s5k3h2_sunny_q8s02e = {
  .flash_type             = MSM_CAMERA_FLASH_LED,
  .flash_src              = &msm_flash_src_s5k3h2_sunny_q8s02e,
};

static struct msm_camera_sensor_info msm_camera_sensor_s5k3h2_sunny_q8s02e_data = {
  .sensor_name            = "s5k3h2_sunny_q8s02e",
  .sensor_reset_enable    = 1,
  .pmic_gpio_enable       = 1,
  .sensor_reset           = GPIO_NOT_CONFIGURED,
  .sensor_pwd             = GPIO_NOT_CONFIGURED,
  .pdata                  = &msm_camera_device_data_csi1[0],
  .flash_data             = &flash_s5k3h2_sunny_q8s02e,
  .sensor_platform_info   = &sensor_board_info_s5k3h2_sunny_q8s02e,
  .csi_if                 = 1,
  .camera_type            = BACK_CAMERA_2D,
  .sensor_type            = BAYER_SENSOR,
  .actuator_info          = &msm_act_main_cam_9_info,
};
#endif
#ifdef CONFIG_AR0542
static struct msm_camera_sensor_platform_info sensor_board_info_ar0542 = {
  .mount_angle            = MOUNT_ANGLE_NOT_CONFIGURED,
  .cam_vreg               = msm_cam_vreg,
  .num_vreg               = ARRAY_SIZE(msm_cam_vreg),
  .gpio_conf              = &gpio_conf_ar0542,
};

static struct msm_camera_sensor_flash_src msm_flash_src_ar0542 = {
  .flash_sr_type                     = MSM_CAMERA_FLASH_SRC_LED1,
  ._fsrc.ext_driver_src.led_en       = 13,
  ._fsrc.ext_driver_src.led_flash_en = 32,
};

static struct msm_camera_sensor_flash_data flash_ar0542 = {
  .flash_type             = MSM_CAMERA_FLASH_LED,
  .flash_src              = &msm_flash_src_ar0542,
};

static struct msm_actuator_info msm_act_main_cam_10_info = {
        .board_info     = &msm_act_main_cam_i2c_info,
        .cam_name   = MSM_ACTUATOR_MAIN_CAM_10,
        .bus_id         = MSM_GSBI0_QUP_I2C_BUS_ID,
        .vcm_pwd        = GPIO_NOT_CONFIGURED,
        .vcm_enable     = 0,
};

static struct msm_camera_sensor_info msm_camera_sensor_ar0542_data = {
  .sensor_name            = "ar0542",
  .sensor_reset_enable    = 1,
  .pmic_gpio_enable       = 1,
  .sensor_reset           = GPIO_NOT_CONFIGURED,
  .sensor_pwd             = GPIO_NOT_CONFIGURED,
  .pdata                  = &msm_camera_device_data_csi1[0],
  .flash_data             = &flash_ar0542,
  .sensor_platform_info   = &sensor_board_info_ar0542,
  .csi_if                 = 1,
  .camera_type            = BACK_CAMERA_2D,
  .sensor_type            = BAYER_SENSOR,
  .actuator_info = &msm_act_main_cam_10_info,
};
#endif

#ifdef CONFIG_WEBCAM_OV9726
static struct msm_camera_sensor_flash_data flash_ov9726 = {
	.flash_type             = MSM_CAMERA_FLASH_LED,
	.flash_src              = &msm_flash_src
};

static struct msm_camera_sensor_platform_info sensor_board_info_ov9726 = {
	.mount_angle	= 90,
	.cam_vreg = msm_cam_vreg,
	.num_vreg = ARRAY_SIZE(msm_cam_vreg),
	.gpio_conf = &gpio_conf_ov9726,
};

static struct msm_camera_sensor_info msm_camera_sensor_ov9726_data = {
	.sensor_name    = "ov9726",
	.sensor_reset_enable = 0,
	.pdata                  = &msm_camera_device_data_csi0[0],
	.flash_data             = &flash_ov9726,
	.sensor_platform_info   = &sensor_board_info_ov9726,
	.csi_if                 = 1,
	.camera_type = FRONT_CAMERA_2D,
	.sensor_type = BAYER_SENSOR,
};
#endif

static struct platform_device msm_camera_server = {
	.name = "msm_cam_server",
	.id = 0,
};

static void __init msm7x27a_init_cam(void)
{
	if(machine_is_msm8625q_evbd()||
		machine_is_msm8625q_skud())
	{  //for SKUD
#ifdef CONFIG_OV5648_TRULY_CM8352
		if(machine_is_msm8625q_evbd()) {
			sensor_board_info_ov5648_truly_cm8352.cam_vreg = ov5648_gpio_vreg_evbd;
			sensor_board_info_ov5648_truly_cm8352.num_vreg = ARRAY_SIZE(ov5648_gpio_vreg_evbd);
		} else {
			sensor_board_info_ov5648_truly_cm8352.cam_vreg = ov5648_truly_cm8352_gpio_vreg;
			sensor_board_info_ov5648_truly_cm8352.num_vreg = ARRAY_SIZE(ov5648_truly_cm8352_gpio_vreg);
		}
		msm_act_main_cam_7_info.vcm_pwd = GPIO_BACK_CAM_VCM_PWDN;
		msm_act_main_cam_7_info.vcm_enable = 1;
		msm_camera_sensor_ov5648_truly_cm8352_data.sensor_reset=GPIO_BACK_CAM_RESET;
		msm_camera_sensor_ov5648_truly_cm8352_data.sensor_pwd = GPIO_BACK_CAM_SHDN_N;
		sensor_board_info_ov5648_truly_cm8352.mount_angle = 90;
		msm_flash_src_ov5648_truly_cm8352._fsrc.ext_driver_src.led_en = GPIO_SKUD_CAM_LED_EN;
		msm_flash_src_ov5648_truly_cm8352._fsrc.ext_driver_src.led_flash_en = GPIO_SKUD_CAM_LED_FLASH_EN;
#endif
#ifdef CONFIG_OV7695
		if(machine_is_msm8625q_evbd()) {
			sensor_board_info_ov7695.cam_vreg = ov7695_gpio_vreg_evbd;
			sensor_board_info_ov7695.num_vreg = ARRAY_SIZE(ov7695_gpio_vreg_evbd);
		} else {
			sensor_board_info_ov7695.cam_vreg = ov7695_gpio_vreg;
			sensor_board_info_ov7695.num_vreg = ARRAY_SIZE(ov7695_gpio_vreg);
		}
		msm_camera_sensor_ov7695_data.vcm_pwd = 0;
		msm_camera_sensor_ov7695_data.vcm_enable = 0;
		sensor_board_info_ov7695.gpio_conf = &skud_gpio_conf_ov7695;
		sensor_board_info_ov7695.mount_angle = 90;
#endif
#ifdef CONFIG_OV7695_RAW
		sensor_board_info_ov7695_raw.cam_vreg = ov7695_raw_gpio_vreg;
		sensor_board_info_ov7695_raw.num_vreg = ARRAY_SIZE(ov7695_raw_gpio_vreg);
		msm_camera_sensor_ov7695_raw_data.vcm_pwd = 0;
		msm_camera_sensor_ov7695_raw_data.vcm_enable = 0;
		msm_camera_sensor_ov7695_raw_data.sensor_pwd = GPIO_FRONT_CAM_SHDN;
		sensor_board_info_ov7695_raw.gpio_conf = &skud_gpio_conf_ov7695_raw;
		sensor_board_info_ov7695_raw.mount_angle = 270;
#endif
	}else if(machine_is_msm8625q_skue()) {

	}

	platform_device_register(&msm_camera_server);
	if (machine_is_msm8625_surf() || machine_is_msm8625_evb()
			|| machine_is_msm8625_qrd5() || machine_is_msm7x27a_qrd5a() 
			|| machine_is_msm8625_qrd7()
			|| machine_is_msm8625q_evbd()
			|| machine_is_msm8625q_skud()
			|| machine_is_msm8625q_skue()) {
		platform_device_register(&msm8625_device_csic0);
		platform_device_register(&msm8625_device_csic1);
	} else {
		platform_device_register(&msm7x27a_device_csic0);
		platform_device_register(&msm7x27a_device_csic1);
	}
	if (machine_is_msm8625_evb()
			|| machine_is_msm8625_qrd5()
			|| machine_is_msm7627a_qrd3()
			|| machine_is_msm8625_qrd7()
			|| machine_is_msm7x27a_qrd5a()
			|| machine_is_msm8625q_evbd()
			|| machine_is_msm8625q_skud()
			|| machine_is_msm8625q_skue())
		*(int *) msm7x27a_device_clkctl.dev.platform_data = 1;
	platform_device_register(&msm7x27a_device_clkctl);
	platform_device_register(&msm7x27a_device_vfe);
}

static struct i2c_board_info i2c_camera_devices_skud[] = {
#ifdef CONFIG_OV7695
	{
		I2C_BOARD_INFO("ov7695", 0x21 << 1),
		.platform_data = &msm_camera_sensor_ov7695_data,
	},
#endif
#ifdef CONFIG_OV7695_RAW
	{
		I2C_BOARD_INFO("ov7695_raw", 0x21 << 1),
		.platform_data = &msm_camera_sensor_ov7695_raw_data,
	},
#endif
#ifdef CONFIG_S5K4E1
	{
		//I2C_BOARD_INFO("s5k4e1", 0x36),
		//I2C_BOARD_INFO("s5k4e1", 0x6E),
		I2C_BOARD_INFO("s5k4e1", 0x20),
		.platform_data = &msm_camera_sensor_s5k4e1_data,
	},
#endif
#ifdef CONFIG_GC0339
	{
		I2C_BOARD_INFO("gc0339", 0x21 << 1),
		.platform_data = &msm_camera_sensor_gc0339_data,
	},
#endif
};

#endif

static void skud_camera_gpio_cfg(void)
{
	int rc = 0;
	printk("skuD_camera_gpio_cfg in, cfg gpio\n");
	printk("gpio request: GPIO_BACK_CAM_SHDN_N is %d\n", GPIO_BACK_CAM_SHDN_N);
	rc = gpio_request(GPIO_BACK_CAM_SHDN_N, "ov5648");
	if (rc < 0)
		printk("%s: gpio_request OV5648 sensor_pwd: %d failed!",
			 __func__, GPIO_BACK_CAM_SHDN_N);

	rc = gpio_tlmm_config(GPIO_CFG(GPIO_BACK_CAM_SHDN_N,
				0, GPIO_CFG_OUTPUT, GPIO_CFG_PULL_DOWN,
				GPIO_CFG_2MA), GPIO_CFG_ENABLE);
	if (rc < 0) {
		printk("%s:unable to enable Powr Dwn gpio for main camera!\n",
			 __func__);
		gpio_free(GPIO_BACK_CAM_SHDN_N);
	}

	rc = gpio_direction_output(GPIO_BACK_CAM_SHDN_N, 1);
	if (rc < 0)
		pr_err("%s: unable to set gpio: %d direction for ov5648 camera\n",
			__func__, GPIO_BACK_CAM_SHDN_N);

	printk("gpio request: GPIO_BACK_CAM_RESET is %d\n", GPIO_BACK_CAM_RESET);
	rc = gpio_request(GPIO_BACK_CAM_RESET, "ov5648");
	if (rc < 0)
		pr_err("%s: gpio_request OV5648 sensor_reset: %d failed!",
			 __func__, GPIO_BACK_CAM_RESET);

	rc = gpio_tlmm_config(GPIO_CFG(
				GPIO_BACK_CAM_RESET,
				0, GPIO_CFG_OUTPUT, GPIO_CFG_PULL_DOWN,
				GPIO_CFG_2MA), GPIO_CFG_ENABLE);
	if (rc < 0) {
		pr_err("%s: unable to enable reset gpio for main camera!\n",
			 __func__);
		gpio_free(GPIO_BACK_CAM_RESET);
	}

	rc = gpio_direction_output(
			GPIO_BACK_CAM_RESET, 1);
	if (rc < 0)
		pr_err("%s: unable to set gpio: %d direction for ov5648 camera\n",
			__func__, GPIO_BACK_CAM_RESET);

	printk("gpio request: GPIO_FRONT_CAM_SHDN is %d\n", GPIO_FRONT_CAM_SHDN);
		rc = gpio_request(GPIO_FRONT_CAM_SHDN, "OV7695");
	if (rc < 0)
	pr_err("%s: gpio_request OV7695 sensor_pwdn: %d failed!",
		__func__, GPIO_FRONT_CAM_SHDN);

	rc = gpio_tlmm_config(GPIO_CFG(
		GPIO_FRONT_CAM_SHDN,
		0, GPIO_CFG_OUTPUT, GPIO_CFG_PULL_DOWN,
		GPIO_CFG_2MA), GPIO_CFG_ENABLE);
	if (rc < 0) {
		pr_err("%s: unable to enable reset gpio for front camera!\n", __func__);
		gpio_free(GPIO_FRONT_CAM_SHDN);
	}

	rc = gpio_direction_output(GPIO_FRONT_CAM_SHDN, 0);
	if (rc < 0)
		pr_err("%s: unable to set gpio: %d direction for ov7695 camera\n",
		__func__, GPIO_FRONT_CAM_SHDN);

	// ================================
	printk("gpio request: GPIO_BACK_CAM_VCM_PWDN is %d\n", GPIO_BACK_CAM_VCM_PWDN);
	rc = gpio_request(GPIO_BACK_CAM_VCM_PWDN, "msm_actuator");
	if (rc < 0)
		printk("%s: gpio_request msm_actuator : %d failed!",
		__func__, GPIO_BACK_CAM_VCM_PWDN);

	rc = gpio_tlmm_config(GPIO_CFG(
		GPIO_BACK_CAM_VCM_PWDN,
		0, GPIO_CFG_OUTPUT, GPIO_CFG_PULL_DOWN,
		GPIO_CFG_2MA), GPIO_CFG_ENABLE);
	if (rc < 0) {
		printk("%s:unable to enable Powr Dwn gpio for msm_actuator!\n", __func__);
		gpio_free(GPIO_BACK_CAM_VCM_PWDN);
		}
	rc = gpio_direction_output(GPIO_BACK_CAM_VCM_PWDN, 0);
	if (rc < 0)
		pr_err("%s: unable to set gpio: %d direction for ov5648 camera\n",
		__func__, GPIO_BACK_CAM_VCM_PWDN);

	//Free the gpio for the actuator will request it again, only do tlmm config here.
	gpio_free(GPIO_BACK_CAM_VCM_PWDN);
	//=================================

	//Fix CAMID leak current issue when phone sleep.
	printk("gpio request: set CAMID GPIO to INPUT, now the gpio is %d\n", GPIO_SKUD_CAM_5MP_CAMID);
	rc = gpio_request(GPIO_SKUD_CAM_5MP_CAMID, "ov5648");
	if (rc < 0)
		printk("%s: gpio_request GPIO %d : failed!", __func__, GPIO_SKUD_CAM_5MP_CAMID);

	gpio_tlmm_config(GPIO_CFG(
		GPIO_SKUD_CAM_5MP_CAMID,
		0, GPIO_CFG_INPUT, GPIO_CFG_PULL_UP,
		GPIO_CFG_2MA), GPIO_CFG_ENABLE);

	//Free the gpio, only do tlmm config to input pull high here.
	gpio_free(GPIO_SKUD_CAM_5MP_CAMID);
}

void camera_af_software_powerdown(struct i2c_client *client)
{
	int rc = 0;
	unsigned char txdata[] = {0x80, 0x0};
	struct i2c_msg msg[] = {
		{
			.addr = 0x18>>1,
			.flags = 0,
			.len = 2,
			.buf = txdata,
		 },
	};
	printk("camera_af_software_powerdown IN...\n");
	/* send software powerdown cmd to AF motor, avoid current leak */
	if (machine_is_msm8625_qrd5()|| machine_is_msm7x27a_qrd5a() || machine_is_msm8625q_evbd() ||
	machine_is_msm8625q_skud() || machine_is_msm8625q_skue()){
		printk("SKUA AF Motor software powerdown, write i2c saddr:0x18, waddr:0x80, wdata:0x00\n");
		rc = i2c_transfer(client->adapter, msg, 1);
		if (rc < 0)
			printk("AF software powerdown faild\n");
		return;
	}
}
EXPORT_SYMBOL(camera_af_software_powerdown);
#ifdef CONFIG_S5K3H2_SUNNY_Q8S02E
int lcd_camera_power_l5_onoff(int on)
{
	static struct regulator *ldo5;
	int rc;

	printk("%s: on = %d\n", __func__, on);
	ldo5 = regulator_get(NULL, "ldo5");
	if (IS_ERR(ldo5)) {
		rc = PTR_ERR(ldo5);
		pr_err("%s: could not get ldo5: %d\n", __func__, rc);
		goto out;
	}

	rc = regulator_set_voltage(ldo5, 1200000, 1200000);
	if (rc) {
		pr_err("%s: could not set ldo5 voltage: %d\n", __func__, rc);
		goto ldo5_free;
	}

	rc = regulator_enable(ldo5);
	if (rc) {
		pr_err("%s: could not enable ldo5: %d\n", __func__, rc);
		goto ldo5_free;
	}

	if (on) {
		rc = regulator_enable(ldo5);
		if (rc) {
			pr_err("'%s' regulator enable failed, rc=%d\n",
				"ldo5", rc);
      goto ldo5_free;
		}
		pr_debug("%s(on): success\n", __func__);
	} else {
		rc = regulator_disable(ldo5);
		if (rc){
			pr_warning("'%s' regulator disable failed, rc=%d\n",
				"ldo5", rc);
      goto ldo5_free;
    }
		pr_debug("%s(off): success\n", __func__);
	}

ldo5_free:
	regulator_put(ldo5);
out:
	ldo5 = NULL;
	return rc;
}
EXPORT_SYMBOL(lcd_camera_power_l5_onoff);
#endif
void __init msm7627a_camera_init(void)
{
	pr_debug("msm7627a_camera_init Entered\n");

	/* LCD and camera power (VREG & LDO) init */
	if(machine_is_msm8625q_skud()||
			machine_is_msm8625q_evbd())
	{
		skud_camera_gpio_cfg();
	}

	msm7x27a_init_cam();

	pr_debug("i2c_register_board_info\n");
	if(machine_is_msm8625q_skud() || machine_is_msm8625q_evbd())
	{
		printk("i2c_camera_devices_SKUD registered\n");
		i2c_register_board_info(MSM_GSBI0_QUP_I2C_BUS_ID,
				i2c_camera_devices_skud,
				ARRAY_SIZE(i2c_camera_devices_skud));
	}
}
