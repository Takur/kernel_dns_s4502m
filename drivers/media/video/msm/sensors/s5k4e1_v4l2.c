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
 *
 */
#include <linux/gpio.h>
#include "msm_sensor.h"
#define SENSOR_NAME "s5k4e1"
#define PLATFORM_DRIVER_NAME "msm_camera_s5k4e1"
#define s5k4e1_obj s5k4e1_##obj
#define MSB                             1
#define LSB                             0

#ifdef CDBG
#undef CDBG
#endif
#ifdef CDBG_HIGH
#undef CDBG_HIGH
#endif

//#define S5K4E1_VERBOSE_DGB

#ifdef S5K4E1_VERBOSE_DGB
#define CDBG(fmt, args...) printk(fmt, ##args)
#define CDBG_HIGH(fmt, args...) printk(fmt, ##args)
#else
#define CDBG(fmt, args...) do { } while (0)
#define CDBG_HIGH(fmt, args...) printk(fmt, ##args)
#endif

DEFINE_MUTEX(s5k4e1_mut);
static struct msm_sensor_ctrl_t s5k4e1_s_ctrl;

static struct msm_camera_i2c_reg_conf s5k4e1_start_settings[] = {
	{0x0100, 0x01},
};

static struct msm_camera_i2c_reg_conf s5k4e1_stop_settings[] = {
	{0x0100, 0x00},
};

static struct msm_camera_i2c_reg_conf s5k4e1_groupon_settings[] = {
	{0x0104, 0x01},
};

static struct msm_camera_i2c_reg_conf s5k4e1_groupoff_settings[] = {
	{0x0104, 0x00},
};

static struct msm_camera_i2c_reg_conf s5k4e1_prev_settings[] = {
	/* output size (1296 x 972) */
	{0x30A9, 0x02},/* Horizontal Binning On */
	{0x300E, 0xEB},/* Vertical Binning On */
	{0x0387, 0x03},/* y_odd_inc 03(10b AVG) */
	{0x0344, 0x00},/* x_addr_start 8 */
	{0x0345, 0x08},
	{0x0348, 0x0A},/* x_addr_end 2599 */
	{0x0349, 0x27},
	{0x0346, 0x00},/* y_addr_start 8 */
	{0x0347, 0x08},
	{0x034A, 0x07},/* y_addr_end 1951 */
	{0x034B, 0x9F},
	{0x0380, 0x00},/* x_even_inc 1 */
	{0x0381, 0x01},
	{0x0382, 0x00},/* x_odd_inc 1 */
	{0x0383, 0x01},
	{0x0384, 0x00},/* y_even_inc 1 */
	{0x0385, 0x01},
	{0x0386, 0x00},/* y_odd_inc 3 */
	{0x0387, 0x03},
	{0x034C, 0x05},/* x_output_size 1296 */
	{0x034D, 0x10},
	{0x034E, 0x03},/* y_output_size 972 */
	{0x034F, 0xCC},
	{0x30BF, 0xAB},/* outif_enable[7], data_type[5:0](2Bh = bayer 10bit} */
	{0x30C0, 0x00},/* video_offset[7:4] 3260%12 */
	{0x30C8, 0x06},/* video_data_length 1600 = 1304 * 1.25 */
	{0x30C9, 0x54},
	/* Timing Configuration */
	{0x0202, 0x03},
	{0x0203, 0x14},
	{0x0204, 0x00},
	{0x0205, 0x80},
	{0x0340, 0x03},/* Frame Length */
	{0x0341, 0xE0},
	{0x0342, 0x0A},/* 2738  Line Length */
	{0x0343, 0xB2},
};

static struct msm_camera_i2c_reg_conf s5k4e1_snap_settings[] = {
	/*Output Size (2592x1944)*/
	{0x30A9, 0x03},/* Horizontal Binning Off */
	{0x300E, 0xE8},/* Vertical Binning Off */
	{0x0387, 0x01},/* y_odd_inc */
	{0x034C, 0x0A},/* x_output size */
	{0x034D, 0x20},
	{0x034E, 0x07},/* y_output size */
	{0x034F, 0x98},
	{0x30BF, 0xAB},/* outif_enable[7], data_type[5:0](2Bh = bayer 10bit} */
	{0x30C0, 0x00},/* video_offset[7:4] 3260%12 */
	{0x30C8, 0x0C},/* video_data_length 3260 = 2608 * 1.25 */
	{0x30C9, 0xA8},
	/*Timing configuration*/
	{0x0202, 0x06},
	{0x0203, 0x28},
	{0x0204, 0x00},
	{0x0205, 0x80},
	{0x0340, 0x07},/* Frame Length */
	{0x0341, 0xB4},
	{0x0342, 0x0A},/* 2738 Line Length */
	{0x0343, 0xB2},
};

static struct msm_camera_i2c_reg_conf s5k4e1_recommend_settings[] = {
	/* Reset setting */
	{0x0103, 0x01},
	/* MIPI settings */
	{0x30BD, 0x00},/* SEL_CCP[0] */
	{0x3084, 0x15},/* SYNC Mode */
	{0x30BE, 0x1A},/* M_PCLKDIV_AUTO[4], M_DIV_PCLK[3:0] */
	{0x30C1, 0x01},/* pack video enable [0] */
	{0x30EE, 0x02},/* DPHY enable [ 1] */
	{0x3111, 0x86},/* Embedded data off [5] */

	/* REC Settings */
	/*CDS timing setting ... */
	{0x3000,0x05},
	{0x3001,0x03},
	{0x3002,0x08},
	{0x3003,0x09},
	{0x3004,0x2E},
	{0x3005,0x06},
	{0x3006,0x34},
	{0x3007,0x00},
	{0x3008,0x3C},
	{0x3009,0x3C},
	{0x300A,0x28},
	{0x300B,0x04},
	{0x300C,0x0A},
	{0x300D,0x02},
	{0x300E,0xE8},
	{0x300F,0x82},
	{0x301B,0x77},

	/* CDS option setting ... */
	{0x3010, 0x00},
	{0x3011, 0x3A},
	{0x3029, 0x04},
	{0x3012, 0x30},
	{0x3013, 0xA0},
	{0x3014, 0x00},
	{0x3015, 0x00},
	{0x3016, 0x30},
	{0x3017, 0x94},
	{0x3018, 0x70},
	{0x301D, 0xD4},
	{0x3021, 0x02},
	{0x3022, 0x24},
	{0x3024, 0x40},
	{0x3027, 0x08},

	/* Pixel option setting ...   */
	{0x301C, 0x04},
	{0x30D8, 0x3F},
	{0x302B, 0x01},

	{0x3070, 0x5F},
	{0x3071, 0x00},
	{0x3080, 0x04},
	{0x3081, 0x38},

	/* PLL settings */
	//	{0x0305, 0x04},
	//	{0x0306, 0x00},
	//	{0x0307, 0x44},
	//	{0x30B5, 0x00},
	//	{0x30E2, 0x01},/* num lanes[1:0] = 2 */
	//	{0x30F1, 0xB0},

	// PLL setting ...
	// input clock 24MHz
	// (3) MIPI 2-lane Serial(TST = 0000b or TST = 0010b), 15 fps
	{0x0305,0x04},//06////PLL P = 6
	{0x0306,0x00},//PLL M[8] = 0
	{0x0307,0x44},//36//65//PLL M = 101
	{0x30B5,0x01},//01//PLL S = 1 
	{0x30E2,0x02},//02//num lanes[1:0] = 2
	{0x30F1,0x70},//70//DPHY BANDCTRL 408MHz=40.8MHz

	{0x3096,0x40},
	{0x3097,0x52}, 	//sh4ch_blk_width = 82
	{0x3098,0x7b}, 	//sh4ch_blk_height = 123
	{0x3099,0x03}, 	//sh4ch_step_x msb (sh4ch_step_x = 799)
	{0x309a,0x1f}, 	//sh4ch_step_x lsb
	{0x309b,0x02}, 	//sh4ch_step_y msb (sh4ch_step_y = 533)
	{0x309c,0x15}, 	//sh4ch_step_y lsb
	{0x309d,0x00}, 	//sh4ch_start_blk_cnt_x = 0
	{0x309e,0x00}, 	//sh4ch_start_int_cnt_x = 0
	{0x309f,0x00}, 	//sh4ch_start_frac_cnt_x msb (sh4ch_start_frac_cnt_x = 0)
	{0x30a0,0x00}, 	//sh4ch_start_frac_cnt_x lsb
	{0x30a1,0x00}, 	//sh4ch_start_blk_cnt_y = 0
	{0x30a2,0x00}, 	//sh4ch_start_int_cnt_y = 0
	{0x30a3,0x00}, 	//sh4ch_start_frac_cnt_y msb (sh4ch_start_frac_cnt_x = 0)
	{0x30a4,0x00}, 	//sh4ch_start_frac_cnt_y lsb
	{0x30a5,0x01},
	{0x30a6,0x00}, 	// gs_pedestal	= 64
	{0x3200,0x00},
	{0x3201,0x71},
	{0x3202,0xbb},
	{0x3203,0x0f},
	{0x3204,0xe9},
	{0x3205,0x16},
	{0x3206,0x00},
	{0x3207,0x09},
	{0x3208,0xa2},
	{0x3209,0x00},
	{0x320a,0x07},
	{0x320b,0xd6},
	{0x320c,0x0f},
	{0x320d,0xef},
	{0x320e,0x47},
	{0x320f,0x00},
	{0x3210,0x10},
	{0x3211,0x5f},
	{0x3212,0x0f},
	{0x3213,0xe2},
	{0x3214,0xca},
	{0x3215,0x00},
	{0x3216,0x0c},
	{0x3217,0xda},
	{0x3218,0x0f},
	{0x3219,0xf5},
	{0x321a,0xc4},
	{0x321b,0x0f},
	{0x321c,0xff},
	{0x321d,0x11},
	{0x321e,0x00},
	{0x321f,0x0f},
	{0x3220,0xc5},
	{0x3221,0x0f},
	{0x3222,0xeb},
	{0x3223,0x18},
	{0x3224,0x00},
	{0x3225,0x16},
	{0x3226,0x6f},
	{0x3227,0x0f},
	{0x3228,0xe8},
	{0x3229,0xbb},
	{0x322a,0x00},
	{0x322b,0x14},
	{0x322c,0x21},
	{0x322d,0x0f},
	{0x322e,0xfa},
	{0x322f,0x52},
	{0x3230,0x0f},
	{0x3231,0xf5},
	{0x3232,0x7f},
	{0x3233,0x00},
	{0x3234,0x14},
	{0x3235,0x48},
	{0x3236,0x0f},
	{0x3237,0xf3},
	{0x3238,0x34},
	{0x3239,0x00},
	{0x323a,0x13},
	{0x323b,0xe0},
	{0x323c,0x0f},
	{0x323d,0xf4},
	{0x323e,0xc6},
	{0x323f,0x0f},
	{0x3240,0xfe},
	{0x3241,0x30},
	{0x3242,0x00},
	{0x3243,0x09},
	{0x3244,0x4e},
	{0x3245,0x0f},
	{0x3246,0xef},
	{0x3247,0x46},
	{0x3248,0x00},
	{0x3249,0x05},
	{0x324a,0x4d},
	{0x324b,0x00},
	{0x324c,0x01},
	{0x324d,0xba},
	{0x324e,0x0f},
	{0x324f,0xf3},
	{0x3250,0xf1},
	{0x3251,0x00},
	{0x3252,0x0d},
	{0x3253,0x03},
	{0x3254,0x0f},
	{0x3255,0xfd},
	{0x3256,0x7a},
	{0x3257,0x00},
	{0x3258,0x02},
	{0x3259,0xf0},
	{0x325a,0x00},
	{0x325b,0x02},
	{0x325c,0x37},
	{0x325d,0x0f},
	{0x325e,0xf2},
	{0x325f,0xe0},
	{0x3260,0x00},
	{0x3261,0x10},
	{0x3262,0x94},
	{0x3263,0x0f},
	{0x3264,0xf9},
	{0x3265,0x15},
	{0x3266,0x0f},
	{0x3267,0xf5},
	{0x3268,0xa0},
	{0x3269,0x00},
	{0x326a,0x0c},
	{0x326b,0x44},
	{0x326c,0x00},
	{0x326d,0x75},
	{0x326e,0x56},
	{0x326f,0x0f},
	{0x3270,0xe7},
	{0x3271,0x42},
	{0x3272,0x00},
	{0x3273,0x0d},
	{0x3274,0xce},
	{0x3275,0x00},
	{0x3276,0x02},
	{0x3277,0x11},
	{0x3278,0x0f},
	{0x3279,0xf4},
	{0x327a,0x7d},
	{0x327b,0x00},
	{0x327c,0x0e},
	{0x327d,0x01},
	{0x327e,0x0f},
	{0x327f,0xe0},
	{0x3280,0xa3},
	{0x3281,0x00},
	{0x3282,0x0a},
	{0x3283,0xfc},
	{0x3284,0x0f},
	{0x3285,0xed},
	{0x3286,0xe9},
	{0x3287,0x00},
	{0x3288,0x0e},
	{0x3289,0x80},
	{0x328a,0x00},
	{0x328b,0x04},
	{0x328c,0x31},
	{0x328d,0x0f},
	{0x328e,0xee},
	{0x328f,0xf0},
	{0x3290,0x00},
	{0x3291,0x17},
	{0x3292,0xfe},
	{0x3293,0x0f},
	{0x3294,0xee},
	{0x3295,0x4f},
	{0x3296,0x00},
	{0x3297,0x1e},
	{0x3298,0x84},
	{0x3299,0x0f},
	{0x329a,0xe8},
	{0x329b,0x19},
	{0x329c,0x0f},
	{0x329d,0xf9},
	{0x329e,0x80},
	{0x329f,0x00},
	{0x32a0,0x19},
	{0x32a1,0x4e},
	{0x32a2,0x0f},
	{0x32a3,0xf2},
	{0x32a4,0x3a},
	{0x32a5,0x00},
	{0x32a6,0x0f},
	{0x32a7,0xf5},
	{0x32a8,0x0f},
	{0x32a9,0xe4},
	{0x32aa,0xcd},
	{0x32ab,0x00},
	{0x32ac,0x11},
	{0x32ad,0xdc},
	{0x32ae,0x00},
	{0x32af,0x09},
	{0x32b0,0xeb},
	{0x32b1,0x0f},
	{0x32b2,0xea},
	{0x32b3,0x3b},
	{0x32b4,0x00},
	{0x32b5,0x06},
	{0x32b6,0xd6},
	{0x32b7,0x0f},
	{0x32b8,0xfe},
	{0x32b9,0x86},
	{0x32ba,0x00},
	{0x32bb,0x02},
	{0x32bc,0xcd},
	{0x32bd,0x00},
	{0x32be,0x04},
	{0x32bf,0x5b},
	{0x32c0,0x0f},
	{0x32c1,0xfc},
	{0x32c2,0x04},
	{0x32c3,0x0f},
	{0x32c4,0xf9},
	{0x32c5,0xc1},
	{0x32c6,0x00},
	{0x32c7,0x01},
	{0x32c8,0x66},
	{0x32c9,0x0f},
	{0x32ca,0xf7},
	{0x32cb,0xdb},
	{0x32cc,0x00},
	{0x32cd,0x0d},
	{0x32ce,0x81},
	{0x32cf,0x0f},
	{0x32d0,0xf1},
	{0x32d1,0x77},
	{0x32d2,0x0f},
	{0x32d3,0xf9},
	{0x32d4,0x53},
	{0x32d5,0x00},
	{0x32d6,0x1a},
	{0x32d7,0xa4},
	{0x32d8,0x00},
	{0x32d9,0x63},
	{0x32da,0x1d},
	{0x32db,0x0f},
	{0x32dc,0xeb},
	{0x32dd,0x5e},
	{0x32de,0x00},
	{0x32df,0x0d},
	{0x32e0,0x31},
	{0x32e1,0x0f},
	{0x32e2,0xfe},
	{0x32e3,0x17},
	{0x32e4,0x0f},
	{0x32e5,0xfc},
	{0x32e6,0x3b},
	{0x32e7,0x00},
	{0x32e8,0x08},
	{0x32e9,0x60},
	{0x32ea,0x0f},
	{0x32eb,0xe6},
	{0x32ec,0xf9},
	{0x32ed,0x00},
	{0x32ee,0x0c},
	{0x32ef,0x1c},
	{0x32f0,0x0f},
	{0x32f1,0xf1},
	{0x32f2,0xf1},
	{0x32f3,0x00},
	{0x32f4,0x09},
	{0x32f5,0x84},
	{0x32f6,0x00},
	{0x32f7,0x01},
	{0x32f8,0xaa},
	{0x32f9,0x0f},
	{0x32fa,0xf3},
	{0x32fb,0x87},
	{0x32fc,0x00},
	{0x32fd,0x16},
	{0x32fe,0x11},
	{0x32ff,0x0f},
	{0x3300,0xf6},
	{0x3301,0x3b},
	{0x3302,0x00},
	{0x3303,0x08},
	{0x3304,0x55},
	{0x3305,0x00},
	{0x3306,0x00},
	{0x3307,0x32},
	{0x3308,0x0f},
	{0x3309,0xf7},
	{0x330a,0x14},
	{0x330b,0x00},
	{0x330c,0x10},
	{0x330d,0x20},
	{0x330e,0x0f},
	{0x330f,0xf3},
	{0x3310,0xdb},
	{0x3311,0x00},
	{0x3312,0x02},
	{0x3313,0x86},
	{0x3314,0x00},
	{0x3315,0x00},
	{0x3316,0x5d},
	{0x3317,0x0f},
	{0x3318,0xf8},
	{0x3319,0xfe},
	{0x331a,0x00},
	{0x331b,0x04},
	{0x331c,0xd3},
	{0x331d,0x0f},
	{0x331e,0xf8},
	{0x331f,0xdb},
	{0x3320,0x00},
	{0x3321,0x04},
	{0x3322,0x28},
	{0x3323,0x00},
	{0x3324,0x04},
	{0x3325,0xe3},
	{0x3326,0x0f},
	{0x3327,0xf7},
	{0x3328,0x16},
	{0x3329,0x00},
	{0x332a,0x0b},
	{0x332b,0x60},
	{0x332c,0x00},
	{0x332d,0x01},
	{0x332e,0x51},
	{0x332f,0x0f},
	{0x3330,0xfa},
	{0x3331,0x01},
	{0x3332,0x00},
	{0x3333,0x02},
	{0x3334,0x40},
	{0x3335,0x0f},
	{0x3336,0xfa},
	{0x3337,0x75},
	{0x3338,0x00},
	{0x3339,0x07},
	{0x333a,0xec},
	{0x333b,0x0f},
	{0x333c,0xf9},
	{0x333d,0x49},
	{0x333e,0x0f},
	{0x333f,0xfa},
	{0x3340,0xa7},
	{0x3341,0x00},
	{0x3342,0x0c},
	{0x3343,0x14},
	{0x3344,0x00},
	{0x3345,0x6c},
	{0x3346,0x6f},
	{0x3347,0x0f},
	{0x3348,0xe9},
	{0x3349,0x96},
	{0x334a,0x00},
	{0x334b,0x0f},
	{0x334c,0xdb},
	{0x334d,0x0f},
	{0x334e,0xfb},
	{0x334f,0xa8},
	{0x3350,0x0f},
	{0x3351,0xfe},
	{0x3352,0xa0},
	{0x3353,0x00},
	{0x3354,0x07},
	{0x3355,0x43},
	{0x3356,0x0f},
	{0x3357,0xe4},
	{0x3358,0x63},
	{0x3359,0x00},
	{0x335a,0x0c},
	{0x335b,0x82},
	{0x335c,0x0f},
	{0x335d,0xf3},
	{0x335e,0x99},
	{0x335f,0x00},
	{0x3360,0x06},
	{0x3361,0x1f},
	{0x3362,0x00},
	{0x3363,0x05},
	{0x3364,0x3c},
	{0x3365,0x0f},
	{0x3366,0xf0},
	{0x3367,0x99},
	{0x3368,0x00},
	{0x3369,0x15},
	{0x336a,0xfe},
	{0x336b,0x0f},
	{0x336c,0xec},
	{0x336d,0xba},
	{0x336e,0x00},
	{0x336f,0x0c},
	{0x3370,0xcc},
	{0x3371,0x00},
	{0x3372,0x00},
	{0x3373,0x4a},
	{0x3374,0x0f},
	{0x3375,0xf5},
	{0x3376,0xc4},
	{0x3377,0x00},
	{0x3378,0x13},
	{0x3379,0x50},
	{0x337a,0x0f},
	{0x337b,0xf3},
	{0x337c,0xf9},
	{0x337d,0x00},
	{0x337e,0x0f},
	{0x337f,0xd2},
	{0x3380,0x0f},
	{0x3381,0xfa},
	{0x3382,0xe7},
	{0x3383,0x0f},
	{0x3384,0xf6},
	{0x3385,0xc2},
	{0x3386,0x00},
	{0x3387,0x09},
	{0x3388,0xc0},
	{0x3389,0x0f},
	{0x338a,0xf2},
	{0x338b,0x30},
	{0x338c,0x00},
	{0x338d,0x08},
	{0x338e,0x8d},
	{0x338f,0x0f},
	{0x3390,0xfe},
	{0x3391,0xe3},
	{0x3392,0x0f},
	{0x3393,0xf8},
	{0x3394,0xd7},
	{0x3395,0x00},
	{0x3396,0x0e},
	{0x3397,0xa7},
	{0x3398,0x0f},
	{0x3399,0xf9},
	{0x339a,0xa7},
	{0x339b,0x00},
	{0x339c,0x03},
	{0x339d,0x86},
	{0x339e,0x0f},
	{0x339f,0xfc},
	{0x33a0,0xb2},
	{0x33a1,0x0f},
	{0x33a2,0xf9},
	{0x33a3,0x75},
	{0x33a4,0x00},
	{0x33a5,0x07},
	{0x33a6,0x1d},
	{0x33a7,0x0f},
	{0x33a8,0xfb},
	{0x33a9,0x25},
	{0x33aa,0x0f},
	{0x33ab,0xfc},
	{0x33ac,0x4c},
	{0x33ad,0x00},
	{0x33ae,0x06},
	{0x33af,0xf2},
	{0x3096,0x60},
	{0x3096,0x40},

	//lens 70%
};

static struct v4l2_subdev_info s5k4e1_subdev_info[] = {
	{
	.code   = V4L2_MBUS_FMT_SGRBG10_1X10,
	.colorspace = V4L2_COLORSPACE_JPEG,
	.fmt    = 1,
	.order    = 0,
	},
	/* more can be supported, to be added later */
};

static struct msm_camera_i2c_conf_array s5k4e1_init_conf[] = {
	{&s5k4e1_recommend_settings[0],
	ARRAY_SIZE(s5k4e1_recommend_settings), 0, MSM_CAMERA_I2C_BYTE_DATA}
};

static struct msm_camera_i2c_conf_array s5k4e1_confs[] = {
	{&s5k4e1_snap_settings[0],
	ARRAY_SIZE(s5k4e1_snap_settings), 0, MSM_CAMERA_I2C_BYTE_DATA},
	{&s5k4e1_prev_settings[0],
	ARRAY_SIZE(s5k4e1_prev_settings), 0, MSM_CAMERA_I2C_BYTE_DATA},
};


static struct msm_sensor_output_info_t s5k4e1_dimensions[] = {
	{
		.x_output = 0xa20,
		.y_output = 0x790,
		.line_length_pclk = 0xAB2,
		.frame_length_lines = 0x7B2,
		.vt_pixel_clk = 81600000,
		.op_pixel_clk = 81600000,
		.binning_factor = 0,
	},
	{
		.x_output = 0x510,
		.y_output = 0x3CC,
		.line_length_pclk = 0xAB2,
		.frame_length_lines = 0x3E0,
		.vt_pixel_clk = 81600000,
		.op_pixel_clk = 81600000,
		.binning_factor = 1,
	},
};

static struct msm_camera_csi_params s5k4e1_csi_params = {
	.data_format = CSI_10BIT,
	.lane_cnt    = 2,
	.lane_assign = 0xe4,
	.dpcm_scheme = 0,
	.settle_cnt  = 0x14,
};

static struct msm_camera_csi_params *s5k4e1_csi_params_array[] = {
	&s5k4e1_csi_params,
	&s5k4e1_csi_params,
};

static struct msm_sensor_output_reg_addr_t s5k4e1_reg_addr = {
	.x_output = 0x034C,
	.y_output = 0x034E,
	.line_length_pclk = 0x0342,
	.frame_length_lines = 0x0340,
};

static struct msm_sensor_id_info_t s5k4e1_id_info = {
	.sensor_id_reg_addr = 0x0000,
	.sensor_id = 0x4E10,
};

static struct msm_sensor_exp_gain_info_t s5k4e1_exp_gain_info = {
	.coarse_int_time_addr = 0x0202,
	.global_gain_addr = 0x0204,
	.vert_offset = 4,
};

static inline uint8_t s5k4e1_byte(uint16_t word, uint8_t offset)
{
	return word >> (offset * BITS_PER_BYTE);
}
int32_t s5k4e1_sensor_power_up(struct msm_sensor_ctrl_t *s_ctrl)
{
	int32_t rc = 0;
	struct msm_camera_sensor_info *info = NULL;
	info = s_ctrl->sensordata;

	usleep_range(1000, 1100);
	gpio_set_value(info->sensor_pwd, 0);
	usleep_range(10000, 11000);
	gpio_set_value(info->sensor_reset, 0);

	rc = msm_sensor_power_up(s_ctrl);
	if (rc < 0) {
		CDBG("%s: msm_sensor_power_up failed\n", __func__);
		return rc;
	}
	usleep_range(1000, 1100);
	/* turn on ldo and vreg */
	gpio_set_value(info->sensor_pwd, 1);
	gpio_set_value(info->sensor_reset, 1);
 
	return rc;

}

int32_t s5k4e1_sensor_power_down(struct msm_sensor_ctrl_t *s_ctrl)
{
	int32_t rc = 0;

	struct msm_camera_sensor_info *info = NULL;

	info = s_ctrl->sensordata;

	msm_sensor_stop_stream(s_ctrl);
	msleep(20);

	gpio_set_value(info->sensor_reset, 0);
	usleep_range(5000, 5100);
	gpio_set_value(info->sensor_pwd, 0);
	usleep_range(5000, 5100);

	rc = msm_sensor_power_down(s_ctrl);
	msleep(20);
#if 0
	if (info->vcm_enable)
	{
		ret = gpio_request(info->vcm_pwd, "s5k4e1_vcm_pwd");
		if (ret < 0)
			printk("%s: gpio_request s5k4e1_vcm_pwd failed!",
				__func__);
		else
		{
			gpio_direction_output(info->vcm_pwd, 0);
			gpio_free(info->vcm_pwd);
		}
	}
#endif

	return rc;
}

static int32_t s5k4e1_write_prev_exp_gain(struct msm_sensor_ctrl_t *s_ctrl,
						uint16_t gain, uint32_t line)
{
	uint16_t max_legal_gain = 0x0200;
	int32_t rc = 0;
	static uint32_t fl_lines, offset;

	CDBG("s5k4e1_write_prev_exp_gain :%d %d\n", gain, line);
	offset = s_ctrl->sensor_exp_gain_info->vert_offset;
	if (gain > max_legal_gain) {
		CDBG("Max legal gain Line:%d\n", __LINE__);
		gain = max_legal_gain;
	}

	/* Analogue Gain */
	msm_camera_i2c_write(s_ctrl->sensor_i2c_client,
		s_ctrl->sensor_exp_gain_info->global_gain_addr,
		s5k4e1_byte(gain, MSB),
		MSM_CAMERA_I2C_BYTE_DATA);
	msm_camera_i2c_write(s_ctrl->sensor_i2c_client,
		s_ctrl->sensor_exp_gain_info->global_gain_addr + 1,
		s5k4e1_byte(gain, LSB),
		MSM_CAMERA_I2C_BYTE_DATA);

	fl_lines = s_ctrl->curr_frame_length_lines;
	fl_lines = (fl_lines * s_ctrl->fps_divider) / Q10;
	
	if (line > (fl_lines- offset)) {
		fl_lines = line + offset;
		s_ctrl->func_tbl->sensor_group_hold_on(s_ctrl);
		msm_camera_i2c_write(s_ctrl->sensor_i2c_client,
			s_ctrl->sensor_output_reg_addr->frame_length_lines,
			s5k4e1_byte(fl_lines, MSB),
			MSM_CAMERA_I2C_BYTE_DATA);
		msm_camera_i2c_write(s_ctrl->sensor_i2c_client,
			s_ctrl->sensor_output_reg_addr->frame_length_lines + 1,
			s5k4e1_byte(fl_lines, LSB),
			MSM_CAMERA_I2C_BYTE_DATA);
		/* Coarse Integration Time */
		msm_camera_i2c_write(s_ctrl->sensor_i2c_client,
			s_ctrl->sensor_exp_gain_info->coarse_int_time_addr,
			s5k4e1_byte(line, MSB),
			MSM_CAMERA_I2C_BYTE_DATA);
		msm_camera_i2c_write(s_ctrl->sensor_i2c_client,
			s_ctrl->sensor_exp_gain_info->coarse_int_time_addr + 1,
			s5k4e1_byte(line, LSB),
			MSM_CAMERA_I2C_BYTE_DATA);
		s_ctrl->func_tbl->sensor_group_hold_off(s_ctrl);
	} else if (line < (fl_lines - offset)) {
		//fl_lines = line + offset;
		if (fl_lines < s_ctrl->curr_frame_length_lines)
			fl_lines = s_ctrl->curr_frame_length_lines;

		s_ctrl->func_tbl->sensor_group_hold_on(s_ctrl);
		/* Coarse Integration Time */
		msm_camera_i2c_write(s_ctrl->sensor_i2c_client,
			s_ctrl->sensor_exp_gain_info->coarse_int_time_addr,
			s5k4e1_byte(line, MSB),
			MSM_CAMERA_I2C_BYTE_DATA);
		msm_camera_i2c_write(s_ctrl->sensor_i2c_client,
			s_ctrl->sensor_exp_gain_info->coarse_int_time_addr + 1,
			s5k4e1_byte(line, LSB),
			MSM_CAMERA_I2C_BYTE_DATA);
		msm_camera_i2c_write(s_ctrl->sensor_i2c_client,
			s_ctrl->sensor_output_reg_addr->frame_length_lines,
			s5k4e1_byte(fl_lines, MSB),
			MSM_CAMERA_I2C_BYTE_DATA);
		msm_camera_i2c_write(s_ctrl->sensor_i2c_client,
			s_ctrl->sensor_output_reg_addr->frame_length_lines + 1,
			s5k4e1_byte(fl_lines, LSB),
			MSM_CAMERA_I2C_BYTE_DATA);
		s_ctrl->func_tbl->sensor_group_hold_off(s_ctrl);
	} else {
		//fl_lines = line+4;
		s_ctrl->func_tbl->sensor_group_hold_on(s_ctrl);
		/* Coarse Integration Time */
		msm_camera_i2c_write(s_ctrl->sensor_i2c_client,
			s_ctrl->sensor_exp_gain_info->coarse_int_time_addr,
			s5k4e1_byte(line, MSB),
			MSM_CAMERA_I2C_BYTE_DATA);
		msm_camera_i2c_write(s_ctrl->sensor_i2c_client,
			s_ctrl->sensor_exp_gain_info->coarse_int_time_addr + 1,
			s5k4e1_byte(line, LSB),
			MSM_CAMERA_I2C_BYTE_DATA);
		s_ctrl->func_tbl->sensor_group_hold_off(s_ctrl);
	}
	return rc;
}
#if 0
static int32_t s5k4e1_write_pict_exp_gain(struct msm_sensor_ctrl_t *s_ctrl,
		uint16_t gain, uint32_t line)
{
	uint16_t max_legal_gain = 0x0200;
	uint16_t min_ll_pck = 0x0AB2;
	uint32_t ll_pck, fl_lines;
	uint32_t ll_ratio;
	uint8_t gain_msb, gain_lsb;
	uint8_t intg_time_msb, intg_time_lsb;
	uint8_t ll_pck_msb, ll_pck_lsb;

	if (gain > max_legal_gain) {
		CDBG("Max legal gain Line:%d\n", __LINE__);
		gain = max_legal_gain;
	}

	pr_info("s5k4e1_write_exp_gain : gain = %d line = %d\n", gain, line);
	line = (uint32_t) (line * s_ctrl->fps_divider);
	fl_lines = s_ctrl->curr_frame_length_lines * s_ctrl->fps_divider / Q10;
	ll_pck = s_ctrl->curr_line_length_pclk;

	if (fl_lines < (line / Q10))
		ll_ratio = (line / (fl_lines - 4));
	else
		ll_ratio = Q10;

	ll_pck = ll_pck * ll_ratio / Q10;
	line = line / ll_ratio;
	if (ll_pck < min_ll_pck)
		ll_pck = min_ll_pck;

	gain_msb = (uint8_t) ((gain & 0xFF00) >> 8);
	gain_lsb = (uint8_t) (gain & 0x00FF);

	intg_time_msb = (uint8_t) ((line & 0xFF00) >> 8);
	intg_time_lsb = (uint8_t) (line & 0x00FF);

	ll_pck_msb = (uint8_t) ((ll_pck & 0xFF00) >> 8);
	ll_pck_lsb = (uint8_t) (ll_pck & 0x00FF);

	s_ctrl->func_tbl->sensor_group_hold_on(s_ctrl);
	msm_camera_i2c_write(s_ctrl->sensor_i2c_client,
		s_ctrl->sensor_exp_gain_info->global_gain_addr,
		gain_msb,
		MSM_CAMERA_I2C_BYTE_DATA);
	msm_camera_i2c_write(s_ctrl->sensor_i2c_client,
		s_ctrl->sensor_exp_gain_info->global_gain_addr + 1,
		gain_lsb,
		MSM_CAMERA_I2C_BYTE_DATA);

	msm_camera_i2c_write(s_ctrl->sensor_i2c_client,
		s_ctrl->sensor_output_reg_addr->line_length_pclk,
		ll_pck_msb,
		MSM_CAMERA_I2C_BYTE_DATA);
	msm_camera_i2c_write(s_ctrl->sensor_i2c_client,
		s_ctrl->sensor_output_reg_addr->line_length_pclk + 1,
		ll_pck_lsb,
		MSM_CAMERA_I2C_BYTE_DATA);

	/* Coarse Integration Time */
	msm_camera_i2c_write(s_ctrl->sensor_i2c_client,
		s_ctrl->sensor_exp_gain_info->coarse_int_time_addr,
		intg_time_msb,
		MSM_CAMERA_I2C_BYTE_DATA);
	msm_camera_i2c_write(s_ctrl->sensor_i2c_client,
		s_ctrl->sensor_exp_gain_info->coarse_int_time_addr + 1,
		intg_time_lsb,
		MSM_CAMERA_I2C_BYTE_DATA);
	s_ctrl->func_tbl->sensor_group_hold_off(s_ctrl);

	return 0;
}
#endif

extern void camera_af_software_powerdown(struct i2c_client *client);
int32_t s5k4e1_sensor_i2c_probe(struct i2c_client *client,
	const struct i2c_device_id *id)
{
	int rc = 0;
	struct msm_camera_sensor_info *s_info;

	rc = msm_sensor_i2c_probe(client, id);

	s_info = client->dev.platform_data;
	if (s_info == NULL) {
		pr_err("%s %s NULL sensor data\n", __func__, client->name);
		return -EFAULT;
	}

#if 0
	if (s_info->actuator_info->vcm_enable) {
		rc = gpio_request(s_info->actuator_info->vcm_pwd,
				"msm_actuator");
		if (rc < 0)
			pr_err("%s: gpio_request:msm_actuator %d failed\n",
				__func__, s_info->actuator_info->vcm_pwd);
		rc = gpio_direction_output(s_info->actuator_info->vcm_pwd, 0);
		if (rc < 0)
			pr_err("%s: gpio:msm_actuator %d direction can't be set\n",
				__func__, s_info->actuator_info->vcm_pwd);
		gpio_free(s_info->actuator_info->vcm_pwd);
	}
#endif

#ifdef CONFIG_CAMERA_AF_FEATURE
	/*  send software powerdown cmd to AF motor, avoid current leak */
	if(0 == rc)
	{
	    camera_af_software_powerdown(client);
	}
	usleep_range(5000, 5100);
#endif

	return rc;
}

static const struct i2c_device_id s5k4e1_i2c_id[] = {
	{SENSOR_NAME, (kernel_ulong_t)&s5k4e1_s_ctrl},
	{ }
};

static struct i2c_driver s5k4e1_i2c_driver = {
	.id_table = s5k4e1_i2c_id,
	.probe  = s5k4e1_sensor_i2c_probe,
	.driver = {
		.name = SENSOR_NAME,
	},
};

static struct msm_camera_i2c_client s5k4e1_sensor_i2c_client = {
	.addr_type = MSM_CAMERA_I2C_WORD_ADDR,
};

static int __init msm_sensor_init_module(void)
{
	return i2c_add_driver(&s5k4e1_i2c_driver);
}

static struct v4l2_subdev_core_ops s5k4e1_subdev_core_ops = {
	.ioctl = msm_sensor_subdev_ioctl,
	.s_power = msm_sensor_power,
};

static struct v4l2_subdev_video_ops s5k4e1_subdev_video_ops = {
	.enum_mbus_fmt = msm_sensor_v4l2_enum_fmt,
};

static struct v4l2_subdev_ops s5k4e1_subdev_ops = {
	.core = &s5k4e1_subdev_core_ops,
	.video  = &s5k4e1_subdev_video_ops,
};

static struct msm_sensor_fn_t s5k4e1_func_tbl = {
	.sensor_start_stream = msm_sensor_start_stream,
	.sensor_stop_stream = msm_sensor_stop_stream,
	.sensor_group_hold_on = msm_sensor_group_hold_on,
	.sensor_group_hold_off = msm_sensor_group_hold_off,
	.sensor_set_fps = msm_sensor_set_fps,
	.sensor_write_exp_gain = s5k4e1_write_prev_exp_gain,
	.sensor_write_snapshot_exp_gain = s5k4e1_write_prev_exp_gain,//s5k4e1_write_pict_exp_gain,
	.sensor_csi_setting = msm_sensor_setting1,
	.sensor_set_sensor_mode = msm_sensor_set_sensor_mode,
	.sensor_mode_init = msm_sensor_mode_init,
	.sensor_get_output_info = msm_sensor_get_output_info,
	.sensor_config = msm_sensor_config,
	.sensor_power_up = s5k4e1_sensor_power_up,
	.sensor_power_down = s5k4e1_sensor_power_down,
};

static struct msm_sensor_reg_t s5k4e1_regs = {
	.default_data_type = MSM_CAMERA_I2C_BYTE_DATA,
	.start_stream_conf = s5k4e1_start_settings,
	.start_stream_conf_size = ARRAY_SIZE(s5k4e1_start_settings),
	.stop_stream_conf = s5k4e1_stop_settings,
	.stop_stream_conf_size = ARRAY_SIZE(s5k4e1_stop_settings),
	.group_hold_on_conf = s5k4e1_groupon_settings,
	.group_hold_on_conf_size = ARRAY_SIZE(s5k4e1_groupon_settings),
	.group_hold_off_conf = s5k4e1_groupoff_settings,
	.group_hold_off_conf_size =
		ARRAY_SIZE(s5k4e1_groupoff_settings),
	.init_settings = &s5k4e1_init_conf[0],
	.init_size = ARRAY_SIZE(s5k4e1_init_conf),
	.mode_settings = &s5k4e1_confs[0],
	.output_settings = &s5k4e1_dimensions[0],
	.num_conf = ARRAY_SIZE(s5k4e1_confs),
};

static struct msm_sensor_ctrl_t s5k4e1_s_ctrl = {
	.msm_sensor_reg = &s5k4e1_regs,
	.sensor_i2c_client = &s5k4e1_sensor_i2c_client,
	.sensor_i2c_addr = 0x20,
	.sensor_output_reg_addr = &s5k4e1_reg_addr,
	.sensor_id_info = &s5k4e1_id_info,
	.sensor_exp_gain_info = &s5k4e1_exp_gain_info,
	.cam_mode = MSM_SENSOR_MODE_INVALID,
	.csic_params = &s5k4e1_csi_params_array[0],
	.msm_sensor_mutex = &s5k4e1_mut,
	.sensor_i2c_driver = &s5k4e1_i2c_driver,
	.sensor_v4l2_subdev_info = s5k4e1_subdev_info,
	.sensor_v4l2_subdev_info_size = ARRAY_SIZE(s5k4e1_subdev_info),
	.sensor_v4l2_subdev_ops = &s5k4e1_subdev_ops,
	.func_tbl = &s5k4e1_func_tbl,
	.clk_rate = MSM_SENSOR_MCLK_24HZ,
};

module_init(msm_sensor_init_module);
MODULE_DESCRIPTION("Samsung 5MP Bayer sensor driver");
MODULE_LICENSE("GPL v2");


