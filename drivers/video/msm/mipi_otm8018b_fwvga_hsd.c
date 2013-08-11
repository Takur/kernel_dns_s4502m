/* Copyright (c) 2008-2011, Code Aurora Forum. All rights reserved.
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

// #define DEBUG
#include <linux/leds.h>
#include <mach/gpio.h>
#include <asm/gpio.h>

#include "msm_fb.h"
#include "mipi_dsi.h"

#define GPIO_OTM_LCD_RESET 78

static int gpio_backlight_en = 0xff;

static struct msm_panel_common_pdata *mipi_otm_pdata;
static struct dsi_buf otm_tx_buf;
static struct dsi_buf otm_rx_buf;

#ifndef CONFIG_FB_MSM_PWM_BACKLIGHT
#if defined(CONFIG_BACKLIGHT_AW9910)
static int prev_bl = 33;
#define PREV_BK_L   33
#define MAX_BACKLIGHT_BRIGHTNESS 255
#define MY_BACKLIGHT_MAX        32
#else
static int prev_bl = 17;
#define PREV_BK_L  17
#define MAX_BACKLIGHT_BRIGHTNESS 255
#define MY_BACKLIGHT_MAX         16
#endif
#endif
#define PWM_GPIO_EN        33

//#ifndef MIPI_OTM_FAKE_PANEL

#define OTM_CMD_DELAY 0
#define MIPI_SETTING_DELAY 0
#define OTM_SLEEP_OFF_DELAY 120
#define OTM_DISPLAY_ON_DELAY 50

/* common setting */
/*
static char extend_cmd_enable[4] = {0xB9, 0xFF, 0x83, 0x69};
*/
//static char swap_updown[2] = {0x36, 0xd0};
static char exit_sleep[2] = {0x11, 0x00};
static char display_on[2] = {0x29, 0x00};
static char display_off[2] = {0x28, 0x00};
static char enter_sleep[2] = {0x10, 0x00};
static unsigned char gpio_lcd_id;

static struct dsi_cmd_desc otm_display_off_cmds[] = {
	{DTYPE_DCS_WRITE, 1, 0, 0, 0, sizeof(display_off), display_off},
	{DTYPE_DCS_WRITE, 1, 0, 0, 10, sizeof(enter_sleep), enter_sleep}
};
/* video mode setting */
//*************************** verA IC for IVO  4.63' MIPI 480RGBx854 tengsheng **********************************************************

static char video1[2] = {0x00, 0x00};
static char video2[4] = {0xff, 0x80, 0x09, 0x01};
static char video3[2] = {0x00, 0x80};
static char video4[3] = {0xff, 0x80, 0x09};
static char video5[2] = {0x00, 0x80};
static char video6[2] = {0xd6, 0x48};
static char video7[2] = {0x00, 0x03};
static char video8[2] = {0xff, 0x01};
static char video9[2] = {0x00, 0xB4};
static char video10[2] = {0xC0,	0x10};
static char video11[2] = {0x00,	0x82};
static char video12[2] = {0xC5,	0xA3};
static char video13[2] = {0x00,	0x90};
static char video14[3] = {0xC5,	0x96, 0x87};
static char video15[2] = {0x00,	0x00};//GVDD = 4.5V
static char video16[3] = {0xD8,	0x74, 0x72};
//Vcomdc
static char video17[2] = {0x00,	0x00};
static char video18[2] = {0xD9,	0x56};//0x56
static char video19[2] = {0x00, 0x00};

static char video20[17] = {0xE1, 0x00, 0x06, 0x0A, 0x07, 0x03, 0x16, 0x08, 0x0A, 0x04,  0x06, 0x07, 0x08, 0x0F, 0x23, 0x22, 0x05};
static char video21[2] = {0x00, 0x00};
static char video22[17] = {0xE2, 0x00, 0x06, 0x0A, 0x07, 0x03, 0x16, 0x08, 0x0A, 0x04,  0x06, 0x07, 0x08, 0x0F, 0x23, 0x22, 0x05};
static char video23[2] = {0x00,	0x81};
static char video24[2] = {0xC1,	0x77};
// RGB I/F settin26VSYNC for OTM
static char video25[2] = {0x00,	0xA0};
static char video26[2] = {0xC1,	0xEA};
static char video27[2] = {0x00,	0xA1};
static char video28[2] = {0xC1,	0x08};//0X08
static char video29[2] = {0x00,	0x89};
static char video30[2] = {0xC4,	0x08};
static char video31[2] = {0x00,	0x81};
static char video32[2] = {0xC4,	0x83};
static char video33[2] = {0x00,	0x92};
static char video34[2] = {0xC5,	0x01};
static char video35[2] = {0x00,	0xB1};
static char video36[2] = {0xC5,	0xA9};
static char video37[2] = {0x00,	0x92};
static char video38[2] = {0xB3,	0x45};
static char video39[2] = {0x00,	0x90};
static char video40[2] = {0xB3,	0x02};
static char video41[2] = {0x00,	0x80};
static char video42[6] = {0xC0,	0x00, 0x58, 0x00, 0x14, 0x16};
static char video43[2] = {0x00,	0x80};
static char video44[2] = {0xC4,	0x30};
// C09x : mck_shift1/mck_shift2/mck_
static char video45[2] = {0x00,	0x90};
static char video46[7] = {0xC0,	0x00, 0x44, 0x00, 0x00, 0x00, 0x03};
static char video47[2] = {0x00,	0xA6};
static char video48[4] = {0xC1,	0x01, 0x00, 0x00};
// CE8x : vst1, vst2, vst3, vst4
static char video49[2] = {0x00,	0x80};
static char video50[13] = {0xCE, 0x87, 0x03, 0x00, 0x85, 0x03, 0x00, 0x86, 0x03, 0x00, 0x84, 0x03, 0x00,};
//CEAx : clka1, clka2
static char video51[2] = {0x00,	0xA0};
static char video52[15] = {0xCE, 0x38, 0x03, 0x03, 0x58, 0x00, 0x00, 0x00, 0x38, 0x02, 0x03, 0x59, 0x00, 0x00, 0x00};
static char video53[2] = {0x00,	0xB0};
static char video54[15] = {0xCE, 0x38, 0x01, 0x03, 0x5A, 0x00, 0x00, 0x00, 0x38, 0x00, 0x03, 0x5B, 0x00, 0x00, 0x00};
static char video55[2] = {0x00,	0xC0};
static char video56[15] = {0xCE, 0x30, 0x00, 0x03, 0x5C, 0x00, 0x00, 0x00, 0x30, 0x01, 0x03, 0x5D, 0x00, 0x00, 0x00};
//CEDx : clkb3, clkb4
static char video57[2] = {0x00,	0xD0};
static char video58[15] = {0xCE, 0x30, 0x02, 0x03, 0x5E, 0x00, 0x00, 0x00, 0x30, 0x03, 0x03, 0x5F, 0x00, 0x00, 0x00};
static char video59[2] = {0x00, 0xC7};
static char video60[2] = {0xCF, 0x00};
static char video61[2] = {0x00, 0xC9};
static char video62[2] = {0xCF, 0x00};

static char video63[2] = {0x00, 0xD0};
static char video64[2] = {0xCF, 0x00};
//--------------------------------------------------------------------------------
//		initial setting 3 < Panel setting >
//--------------------------------------------------------------------------------
static char video65[2] = {0x00, 0xC4};
static char video66[7] = {0xCB, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04};
//cbdx
static char video67[2] = {0x00, 0xD9};
static char video68[7] = {0xCB, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04};
static char video69[2] = {0x00, 0x84};
static char video70[7] = {0xCC, 0x0C, 0x0A, 0x10, 0x0E, 0x03, 0x04};
//cc9x
static char video71[2] = {0x00, 0x9E};
static char video72[2] = {0xCC, 0x0B};
//ccax
static char video73[2] = {0x00, 0xA0};
static char video74[6] = {0xCC, 0x09, 0x0F, 0x0D, 0x01, 0x02};
static char video75[2] = {0x00, 0xB4};
static char video76[7] = {0xCC, 0x0D, 0x0F, 0x09, 0x0B, 0x02, 0x01};
//cccx
static char video77[2] = {0x00, 0xCE};
static char video78[2] = {0xCC, 0x0E};
//ccdx
static char video79[2] = {0x00, 0xD0};
static char video80[6] = {0xCC, 0x10, 0x0A, 0x0C, 0x04, 0x03};
static char video81[2] = {0x00, 0x00};// disable access command 2 for ESD issue
static char video82[4] = {0xff, 0xff, 0xff, 0xff};
//static char video83[2] = {0x3A, 0x77};
//static char video84[2] = {0x35, 0x00};// enable TE function for ESD issue
//static char video85[2] = {0x20, 0x00};

static struct dsi_cmd_desc otm_video_display_on_cmds[] = {
	{DTYPE_GEN_LWRITE, 1, 0, 0, OTM_CMD_DELAY, sizeof(video1), video1},
	{DTYPE_GEN_LWRITE, 1, 0, 0, OTM_CMD_DELAY, sizeof(video2), video2},
	{DTYPE_GEN_LWRITE, 1, 0, 0, OTM_CMD_DELAY, sizeof(video3), video3},
	{DTYPE_GEN_LWRITE, 1, 0, 0, OTM_CMD_DELAY, sizeof(video4), video4},
	{DTYPE_GEN_LWRITE, 1, 0, 0, OTM_CMD_DELAY, sizeof(video5), video5},
	{DTYPE_GEN_LWRITE, 1, 0, 0, OTM_CMD_DELAY, sizeof(video6), video6},
	{DTYPE_GEN_LWRITE, 1, 0, 0, OTM_CMD_DELAY, sizeof(video7), video7},
	{DTYPE_GEN_LWRITE, 1, 0, 0, OTM_CMD_DELAY, sizeof(video8), video8},
	{DTYPE_GEN_LWRITE, 1, 0, 0, OTM_CMD_DELAY, sizeof(video9), video9},
	{DTYPE_GEN_LWRITE, 1, 0, 0, OTM_CMD_DELAY, sizeof(video10), video10},


	{DTYPE_GEN_LWRITE, 1, 0, 0, OTM_CMD_DELAY, sizeof(video11), video11},
	{DTYPE_GEN_LWRITE, 1, 0, 0, OTM_CMD_DELAY, sizeof(video12), video12},
	{DTYPE_GEN_LWRITE, 1, 0, 0, OTM_CMD_DELAY, sizeof(video13), video13},
	{DTYPE_GEN_LWRITE, 1, 0, 0, OTM_CMD_DELAY, sizeof(video14), video14},
	{DTYPE_GEN_LWRITE, 1, 0, 0, OTM_CMD_DELAY, sizeof(video15), video15},
	{DTYPE_GEN_LWRITE, 1, 0, 0, OTM_CMD_DELAY, sizeof(video16), video16},
	{DTYPE_GEN_LWRITE, 1, 0, 0, OTM_CMD_DELAY, sizeof(video17), video17},
	{DTYPE_GEN_LWRITE, 1, 0, 0, OTM_CMD_DELAY, sizeof(video18), video18},
	{DTYPE_GEN_LWRITE, 1, 0, 0, OTM_CMD_DELAY, sizeof(video19), video19},
	{DTYPE_GEN_LWRITE, 1, 0, 0, OTM_CMD_DELAY, sizeof(video20), video20},

	{DTYPE_GEN_LWRITE, 1, 0, 0, OTM_CMD_DELAY, sizeof(video21), video21},
	{DTYPE_GEN_LWRITE, 1, 0, 0, OTM_CMD_DELAY, sizeof(video22), video22},
	{DTYPE_GEN_LWRITE, 1, 0, 0, OTM_CMD_DELAY, sizeof(video23), video23},
	{DTYPE_GEN_LWRITE, 1, 0, 0, OTM_CMD_DELAY, sizeof(video24), video24},
	{DTYPE_GEN_LWRITE, 1, 0, 0, OTM_CMD_DELAY, sizeof(video25), video25},
	{DTYPE_GEN_LWRITE, 1, 0, 0, OTM_CMD_DELAY, sizeof(video26), video26},
	{DTYPE_GEN_LWRITE, 1, 0, 0, OTM_CMD_DELAY, sizeof(video27), video27},
	{DTYPE_GEN_LWRITE, 1, 0, 0, OTM_CMD_DELAY, sizeof(video28), video28},
	{DTYPE_GEN_LWRITE, 1, 0, 0, OTM_CMD_DELAY, sizeof(video29), video29},
	{DTYPE_GEN_LWRITE, 1, 0, 0, OTM_CMD_DELAY, sizeof(video30), video30},

	{DTYPE_GEN_LWRITE, 1, 0, 0, OTM_CMD_DELAY, sizeof(video31), video31},
	{DTYPE_GEN_LWRITE, 1, 0, 0, OTM_CMD_DELAY, sizeof(video32), video32},
	{DTYPE_GEN_LWRITE, 1, 0, 0, OTM_CMD_DELAY, sizeof(video33), video33},
	{DTYPE_GEN_LWRITE, 1, 0, 0, OTM_CMD_DELAY, sizeof(video34), video34},
	{DTYPE_GEN_LWRITE, 1, 0, 0, OTM_CMD_DELAY, sizeof(video35), video35},
	{DTYPE_GEN_LWRITE, 1, 0, 0, OTM_CMD_DELAY, sizeof(video36), video36},
	{DTYPE_GEN_LWRITE, 1, 0, 0, OTM_CMD_DELAY, sizeof(video37), video37},
	{DTYPE_GEN_LWRITE, 1, 0, 0, OTM_CMD_DELAY, sizeof(video38), video38},
	{DTYPE_GEN_LWRITE, 1, 0, 0, OTM_CMD_DELAY, sizeof(video39), video39},
	{DTYPE_GEN_LWRITE, 1, 0, 0, OTM_CMD_DELAY, sizeof(video40), video40},

	{DTYPE_GEN_LWRITE, 1, 0, 0, OTM_CMD_DELAY, sizeof(video41), video41},
	{DTYPE_GEN_LWRITE, 1, 0, 0, OTM_CMD_DELAY, sizeof(video42), video42},
	{DTYPE_GEN_LWRITE, 1, 0, 0, OTM_CMD_DELAY, sizeof(video43), video43},
	{DTYPE_GEN_LWRITE, 1, 0, 0, OTM_CMD_DELAY, sizeof(video44), video44},
	{DTYPE_GEN_LWRITE, 1, 0, 0, OTM_CMD_DELAY, sizeof(video45), video45},
	{DTYPE_GEN_LWRITE, 1, 0, 0, OTM_CMD_DELAY, sizeof(video46), video46},
	{DTYPE_GEN_LWRITE, 1, 0, 0, OTM_CMD_DELAY, sizeof(video47), video47},
	{DTYPE_GEN_LWRITE, 1, 0, 0, OTM_CMD_DELAY, sizeof(video48), video48},
	{DTYPE_GEN_LWRITE, 1, 0, 0, OTM_CMD_DELAY, sizeof(video49), video49},
	{DTYPE_GEN_LWRITE, 1, 0, 0, OTM_CMD_DELAY, sizeof(video50), video50},

	{DTYPE_GEN_LWRITE, 1, 0, 0, OTM_CMD_DELAY, sizeof(video51), video51},
	{DTYPE_GEN_LWRITE, 1, 0, 0, OTM_CMD_DELAY, sizeof(video52), video52},
	{DTYPE_GEN_LWRITE, 1, 0, 0, OTM_CMD_DELAY, sizeof(video53), video53},
	{DTYPE_GEN_LWRITE, 1, 0, 0, OTM_CMD_DELAY, sizeof(video54), video54},
	{DTYPE_GEN_LWRITE, 1, 0, 0, OTM_CMD_DELAY, sizeof(video55), video55},
	{DTYPE_GEN_LWRITE, 1, 0, 0, OTM_CMD_DELAY, sizeof(video56), video56},
	{DTYPE_GEN_LWRITE, 1, 0, 0, OTM_CMD_DELAY, sizeof(video57), video57},
	{DTYPE_GEN_LWRITE, 1, 0, 0, OTM_CMD_DELAY, sizeof(video58), video58},
	{DTYPE_GEN_LWRITE, 1, 0, 0, OTM_CMD_DELAY, sizeof(video59), video59},
	{DTYPE_GEN_LWRITE, 1, 0, 0, OTM_CMD_DELAY, sizeof(video60), video60},

	{DTYPE_GEN_LWRITE, 1, 0, 0, OTM_CMD_DELAY, sizeof(video61), video61},
	{DTYPE_GEN_LWRITE, 1, 0, 0, OTM_CMD_DELAY, sizeof(video62), video62},
	{DTYPE_GEN_LWRITE, 1, 0, 0, OTM_CMD_DELAY, sizeof(video63), video63},
	{DTYPE_GEN_LWRITE, 1, 0, 0, OTM_CMD_DELAY, sizeof(video64), video64},
	{DTYPE_GEN_LWRITE, 1, 0, 0, OTM_CMD_DELAY, sizeof(video65), video65},
	{DTYPE_GEN_LWRITE, 1, 0, 0, OTM_CMD_DELAY, sizeof(video66), video66},
	{DTYPE_GEN_LWRITE, 1, 0, 0, OTM_CMD_DELAY, sizeof(video67), video67},
	{DTYPE_GEN_LWRITE, 1, 0, 0, OTM_CMD_DELAY, sizeof(video68), video68},
	{DTYPE_GEN_LWRITE, 1, 0, 0, OTM_CMD_DELAY, sizeof(video69), video69},
	{DTYPE_GEN_LWRITE, 1, 0, 0, OTM_CMD_DELAY, sizeof(video70), video70},

	{DTYPE_GEN_LWRITE, 1, 0, 0, OTM_CMD_DELAY, sizeof(video71), video71},
	{DTYPE_GEN_LWRITE, 1, 0, 0, OTM_CMD_DELAY, sizeof(video72), video72},
	{DTYPE_GEN_LWRITE, 1, 0, 0, OTM_CMD_DELAY, sizeof(video73), video73},
	{DTYPE_GEN_LWRITE, 1, 0, 0, OTM_CMD_DELAY, sizeof(video74), video74},
	{DTYPE_GEN_LWRITE, 1, 0, 0, OTM_CMD_DELAY, sizeof(video75), video75},
	{DTYPE_GEN_LWRITE, 1, 0, 0, OTM_CMD_DELAY, sizeof(video76), video76},
	{DTYPE_GEN_LWRITE, 1, 0, 0, OTM_CMD_DELAY, sizeof(video77), video77},
	{DTYPE_GEN_LWRITE, 1, 0, 0, OTM_CMD_DELAY, sizeof(video78), video78},
	{DTYPE_GEN_LWRITE, 1, 0, 0, OTM_CMD_DELAY, sizeof(video79), video79},
	{DTYPE_GEN_LWRITE, 1, 0, 0, OTM_CMD_DELAY, sizeof(video80), video80},
	{DTYPE_GEN_LWRITE, 1, 0, 0, OTM_CMD_DELAY, sizeof(video81), video81},
	{DTYPE_GEN_LWRITE, 1, 0, 0, OTM_CMD_DELAY, sizeof(video82), video82},
//	{DTYPE_GEN_LWRITE, 1, 0, 0, OTM_CMD_DELAY, sizeof(video83), video83},
//	{DTYPE_GEN_LWRITE, 1, 0, 0, OTM_CMD_DELAY, sizeof(video84), video84},
//	{DTYPE_GEN_LWRITE, 1, 0, 0, OTM_CMD_DELAY, sizeof(video85), video85},


//	{DTYPE_DCS_WRITE1, 1, 0, 0, OTM_CMD_DELAY, sizeof(swap_updown), swap_updown},
	{DTYPE_DCS_WRITE, 1, 0, 0, OTM_SLEEP_OFF_DELAY, sizeof(exit_sleep), exit_sleep},
	{DTYPE_DCS_WRITE, 1, 0, 0, OTM_DISPLAY_ON_DELAY, sizeof(display_on), display_on},
};

static char video1_dzx[2] = {0x00,0x00};
static char video2_dzx[4] = {0xff,0x80,0x09,0x01};
static char video3_dzx[2] = {0x00,0x80};
static char video4_dzx[3] = {0xff,0x80,0x09};
static char video5_dzx[2] = {0x00,0xb4};
static char video6_dzx[2] = {0xc0,0x50};
static char video7_dzx[2] = {0x00,0x82};
static char video8_dzx[2] = {0xc5,0xa3};
static char video9_dzx[2] = {0x00,0x90};
static char video10_dzx[3] = {0xc5,0xD6,0x76};
static char video11_dzx[2] = {0x00,0x00};
static char video12_dzx[3] = {0xd8,0xAF,0xAF};
static char video13_dzx[2] = {0x00,0x00};
static char video14_dzx[2] = {0xd9,0x58};
static char video15_dzx[2] = {0x00,0x00};
static char video16_dzx[17] = {0xe1,0x00,0x04,0x11,0x0e,0x09,0x1F,0x0b,0x0b,0x00,0x05,0x00,0x06,0x0E,0x22,0x1E,0x00};
static char video17_dzx[2] = {0x00,0x00};
static char video18_dzx[17] = {0xe2,0x00,0x04,0x11,0x0e,0x09,0x1F,0x0b,0x0b,0x00,0x04,0x00,0x06,0x0E,0x22,0x1E,0x00};
static char video19_dzx[2] = {0x00,0x81};
static char video20_dzx[2] = {0xc1,0x66};
static char video21_dzx[2] = {0x00,0xa1};
static char video22_dzx[2] = {0xc1,0x00};
static char video23_dzx[2] = {0x00,0x89};
static char video24_dzx[2] = {0xc4,0x08};
static char video25_dzx[2] = {0x00,0xa2};
static char video26_dzx[4] = {0xc0,0x1b,0x00,0x02};
static char video27_dzx[2] = {0x00,0x81};
static char video28_dzx[2] = {0xc4,0x83};
static char video29_dzx[2] = {0x00,0x92};
static char video30_dzx[2] = {0xc5,0x01};
static char video31_dzx[2] = {0x00,0xb1};
static char video32_dzx[2] = {0xc5,0xa9};
static char video33_dzx[2] = {0x00,0x92};
static char video34_dzx[2] = {0xb3,0x45};
static char video35_dzx[2] = {0x00,0x90};
static char video36_dzx[2] = {0xb3,0x02};
static char video37_dzx[2] = {0x00,0x80};
static char video38_dzx[6] = {0xc0,0x00,0x58,0x00,0x14,0x16};
static char video39_dzx[2] = {0x00,0x80};
static char video40_dzx[2] = {0xc4,0x30};
static char video41_dzx[2] = {0x00,0x90};
static char video42_dzx[7] = {0xc0,0x00,0x44,0x00,0x00,0x00,0x03};
static char video43_dzx[2] = {0x00,0xa6};
static char video44_dzx[4] = {0xc1,0x00,0x00,0x00};
static char video45_dzx[2] = {0x00,0x80};
static char video46_dzx[13] = {0xce,0x87,0x03,0x00,0x85,0x03,0x00,0x86,0x03,0x00,0x84,0x03,0x00};
static char video47_dzx[2] = {0x00,0xa0};
static char video48_dzx[15] = {0xce,0x38,0x03,0x03,0x58,0x00,0x00,0x00,0x38,0x02,0x03,0x59,0x00,0x00,0x00};
static char video49_dzx[2] = {0x00,0xb0};
static char video50_dzx[15] = {0xce,0x38,0x01,0x03,0x5a,0x00,0x00,0x00,0x38,0x00,0x03,0x5b,0x00,0x00,0x00};
static char video51_dzx[2] = {0x00,0xc0};
static char video52_dzx[15] = {0xce,0x30,0x00,0x03,0x5c,0x00,0x00,0x00,0x30,0x01,0x03,0x5d,0x00,0x00,0x00};
static char video53_dzx[2] = {0x00,0xd0};
static char video54_dzx[15] = {0xce,0x30,0x02,0x03,0x5e,0x00,0x00,0x00,0x30,0x03,0x03,0x5f,0x00,0x00,0x00};
static char video55_dzx[2] = {0x00,0xc7};
static char video56_dzx[2] = {0xcf,0x00};
static char video57_dzx[2] = {0x00,0xc9};
static char video58_dzx[2] = {0xcf,0x00};
static char video59_dzx[2] = {0x00,0xc4};
static char video60_dzx[7] = {0xcb,0x04,0x04,0x04,0x04,0x04,0x04};
static char video61_dzx[2] = {0x00,0xd9};
static char video62_dzx[7] = {0xcb,0x04,0x04,0x04,0x04,0x04,0x04};
static char video63_dzx[2] = {0x00,0x84};
static char video64_dzx[7] = {0xcc,0x0c,0x0a,0x10,0x0e,0x03,0x04};
static char video65_dzx[2] = {0x00,0x9e};
static char video66_dzx[2] = {0xcc,0x0b};
static char video67_dzx[2] = {0x00,0xa0};
static char video68_dzx[6] = {0xcc,0x09,0x0f,0x0d,0x01,0x02};
static char video69_dzx[2] = {0x00,0xb4};
static char video70_dzx[7] = {0xcc,0x0d,0x0f,0x09,0x0b,0x02,0x01};
static char video71_dzx[2] = {0x00,0xce};
static char video72_dzx[2] = {0xcc,0x0e};
static char video73_dzx[2] = {0x00,0xd0};
static char video74_dzx[6] = {0xcc,0x10,0x0a,0x0c,0x04,0x03};
static char video75_dzx[2] = {0x00,0x92};
static char video76_dzx[3] = {0xff,0x10,0x02};
static char video77_dzx[2] = {0x00,0x00};
static char video78_dzx[4] = {0xff,0xff,0xff,0xff};

static struct dsi_cmd_desc otm_video_dzx_display_on_cmds[] = {
	{DTYPE_GEN_LWRITE, 1, 0, 0, OTM_CMD_DELAY, sizeof(video1_dzx), video1_dzx},
	{DTYPE_GEN_LWRITE, 1, 0, 0, OTM_CMD_DELAY, sizeof(video2_dzx), video2_dzx},
	{DTYPE_GEN_LWRITE, 1, 0, 0, OTM_CMD_DELAY, sizeof(video3_dzx), video3_dzx},
	{DTYPE_GEN_LWRITE, 1, 0, 0, OTM_CMD_DELAY, sizeof(video4_dzx), video4_dzx},
	{DTYPE_GEN_LWRITE, 1, 0, 0, OTM_CMD_DELAY, sizeof(video5_dzx), video5_dzx},
	{DTYPE_GEN_LWRITE, 1, 0, 0, OTM_CMD_DELAY, sizeof(video6_dzx), video6_dzx},
	{DTYPE_GEN_LWRITE, 1, 0, 0, OTM_CMD_DELAY, sizeof(video7_dzx), video7_dzx},
	{DTYPE_GEN_LWRITE, 1, 0, 0, OTM_CMD_DELAY, sizeof(video8_dzx), video8_dzx},
	{DTYPE_GEN_LWRITE, 1, 0, 0, OTM_CMD_DELAY, sizeof(video9_dzx), video9_dzx},
	{DTYPE_GEN_LWRITE, 1, 0, 0, OTM_CMD_DELAY, sizeof(video10_dzx), video10_dzx},


	{DTYPE_GEN_LWRITE, 1, 0, 0, OTM_CMD_DELAY, sizeof(video11_dzx), video11_dzx},
	{DTYPE_GEN_LWRITE, 1, 0, 0, OTM_CMD_DELAY, sizeof(video12_dzx), video12_dzx},
	{DTYPE_GEN_LWRITE, 1, 0, 0, OTM_CMD_DELAY, sizeof(video13_dzx), video13_dzx},
	{DTYPE_GEN_LWRITE, 1, 0, 0, OTM_CMD_DELAY, sizeof(video14_dzx), video14_dzx},
	{DTYPE_GEN_LWRITE, 1, 0, 0, OTM_CMD_DELAY, sizeof(video15_dzx), video15_dzx},
	{DTYPE_GEN_LWRITE, 1, 0, 0, OTM_CMD_DELAY, sizeof(video16_dzx), video16_dzx},
	{DTYPE_GEN_LWRITE, 1, 0, 0, OTM_CMD_DELAY, sizeof(video17_dzx), video17_dzx},
	{DTYPE_GEN_LWRITE, 1, 0, 0, OTM_CMD_DELAY, sizeof(video18_dzx), video18_dzx},
	{DTYPE_GEN_LWRITE, 1, 0, 0, OTM_CMD_DELAY, sizeof(video19_dzx), video19_dzx},
	{DTYPE_GEN_LWRITE, 1, 0, 0, OTM_CMD_DELAY, sizeof(video20_dzx), video20_dzx},

	{DTYPE_GEN_LWRITE, 1, 0, 0, OTM_CMD_DELAY, sizeof(video21_dzx), video21_dzx},
	{DTYPE_GEN_LWRITE, 1, 0, 0, OTM_CMD_DELAY, sizeof(video22_dzx), video22_dzx},
	{DTYPE_GEN_LWRITE, 1, 0, 0, OTM_CMD_DELAY, sizeof(video23_dzx), video23_dzx},
	{DTYPE_GEN_LWRITE, 1, 0, 0, OTM_CMD_DELAY, sizeof(video24_dzx), video24_dzx},
	{DTYPE_GEN_LWRITE, 1, 0, 0, OTM_CMD_DELAY, sizeof(video25_dzx), video25_dzx},
	{DTYPE_GEN_LWRITE, 1, 0, 0, OTM_CMD_DELAY, sizeof(video26_dzx), video26_dzx},
	{DTYPE_GEN_LWRITE, 1, 0, 0, OTM_CMD_DELAY, sizeof(video27_dzx), video27_dzx},
	{DTYPE_GEN_LWRITE, 1, 0, 0, OTM_CMD_DELAY, sizeof(video28_dzx), video28_dzx},
	{DTYPE_GEN_LWRITE, 1, 0, 0, OTM_CMD_DELAY, sizeof(video29_dzx), video29_dzx},
	{DTYPE_GEN_LWRITE, 1, 0, 0, OTM_CMD_DELAY, sizeof(video30_dzx), video30_dzx},

	{DTYPE_GEN_LWRITE, 1, 0, 0, OTM_CMD_DELAY, sizeof(video31_dzx), video31_dzx},
	{DTYPE_GEN_LWRITE, 1, 0, 0, OTM_CMD_DELAY, sizeof(video32_dzx), video32_dzx},
	{DTYPE_GEN_LWRITE, 1, 0, 0, OTM_CMD_DELAY, sizeof(video33_dzx), video33_dzx},
	{DTYPE_GEN_LWRITE, 1, 0, 0, OTM_CMD_DELAY, sizeof(video34_dzx), video34_dzx},
	{DTYPE_GEN_LWRITE, 1, 0, 0, OTM_CMD_DELAY, sizeof(video35_dzx), video35_dzx},
	{DTYPE_GEN_LWRITE, 1, 0, 0, OTM_CMD_DELAY, sizeof(video36_dzx), video36_dzx},
	{DTYPE_GEN_LWRITE, 1, 0, 0, OTM_CMD_DELAY, sizeof(video37_dzx), video37_dzx},
	{DTYPE_GEN_LWRITE, 1, 0, 0, OTM_CMD_DELAY, sizeof(video38_dzx), video38_dzx},
	{DTYPE_GEN_LWRITE, 1, 0, 0, OTM_CMD_DELAY, sizeof(video39_dzx), video39_dzx},
	{DTYPE_GEN_LWRITE, 1, 0, 0, OTM_CMD_DELAY, sizeof(video40_dzx), video40_dzx},

	{DTYPE_GEN_LWRITE, 1, 0, 0, OTM_CMD_DELAY, sizeof(video41_dzx), video41_dzx},
	{DTYPE_GEN_LWRITE, 1, 0, 0, OTM_CMD_DELAY, sizeof(video42_dzx), video42_dzx},
	{DTYPE_GEN_LWRITE, 1, 0, 0, OTM_CMD_DELAY, sizeof(video43_dzx), video43_dzx},
	{DTYPE_GEN_LWRITE, 1, 0, 0, OTM_CMD_DELAY, sizeof(video44_dzx), video44_dzx},
	{DTYPE_GEN_LWRITE, 1, 0, 0, OTM_CMD_DELAY, sizeof(video45_dzx), video45_dzx},
	{DTYPE_GEN_LWRITE, 1, 0, 0, OTM_CMD_DELAY, sizeof(video46_dzx), video46_dzx},
	{DTYPE_GEN_LWRITE, 1, 0, 0, OTM_CMD_DELAY, sizeof(video47_dzx), video47_dzx},
	{DTYPE_GEN_LWRITE, 1, 0, 0, OTM_CMD_DELAY, sizeof(video48_dzx), video48_dzx},
	{DTYPE_GEN_LWRITE, 1, 0, 0, OTM_CMD_DELAY, sizeof(video49_dzx), video49_dzx},
	{DTYPE_GEN_LWRITE, 1, 0, 0, OTM_CMD_DELAY, sizeof(video50_dzx), video50_dzx},

	{DTYPE_GEN_LWRITE, 1, 0, 0, OTM_CMD_DELAY, sizeof(video51_dzx), video51_dzx},
	{DTYPE_GEN_LWRITE, 1, 0, 0, OTM_CMD_DELAY, sizeof(video52_dzx), video52_dzx},
	{DTYPE_GEN_LWRITE, 1, 0, 0, OTM_CMD_DELAY, sizeof(video53_dzx), video53_dzx},
	{DTYPE_GEN_LWRITE, 1, 0, 0, OTM_CMD_DELAY, sizeof(video54_dzx), video54_dzx},
	{DTYPE_GEN_LWRITE, 1, 0, 0, OTM_CMD_DELAY, sizeof(video55_dzx), video55_dzx},
	{DTYPE_GEN_LWRITE, 1, 0, 0, OTM_CMD_DELAY, sizeof(video56_dzx), video56_dzx},
	{DTYPE_GEN_LWRITE, 1, 0, 0, OTM_CMD_DELAY, sizeof(video57_dzx), video57_dzx},
	{DTYPE_GEN_LWRITE, 1, 0, 0, OTM_CMD_DELAY, sizeof(video58_dzx), video58_dzx},
	{DTYPE_GEN_LWRITE, 1, 0, 0, OTM_CMD_DELAY, sizeof(video59_dzx), video59_dzx},
	{DTYPE_GEN_LWRITE, 1, 0, 0, OTM_CMD_DELAY, sizeof(video60_dzx), video60_dzx},

	{DTYPE_GEN_LWRITE, 1, 0, 0, OTM_CMD_DELAY, sizeof(video61_dzx), video61_dzx},
	{DTYPE_GEN_LWRITE, 1, 0, 0, OTM_CMD_DELAY, sizeof(video62_dzx), video62_dzx},
	{DTYPE_GEN_LWRITE, 1, 0, 0, OTM_CMD_DELAY, sizeof(video63_dzx), video63_dzx},
	{DTYPE_GEN_LWRITE, 1, 0, 0, OTM_CMD_DELAY, sizeof(video64_dzx), video64_dzx},
	{DTYPE_GEN_LWRITE, 1, 0, 0, OTM_CMD_DELAY, sizeof(video65_dzx), video65_dzx},
	{DTYPE_GEN_LWRITE, 1, 0, 0, OTM_CMD_DELAY, sizeof(video66_dzx), video66_dzx},
	{DTYPE_GEN_LWRITE, 1, 0, 0, OTM_CMD_DELAY, sizeof(video67_dzx), video67_dzx},
	{DTYPE_GEN_LWRITE, 1, 0, 0, OTM_CMD_DELAY, sizeof(video68_dzx), video68_dzx},
	{DTYPE_GEN_LWRITE, 1, 0, 0, OTM_CMD_DELAY, sizeof(video69_dzx), video69_dzx},
	{DTYPE_GEN_LWRITE, 1, 0, 0, OTM_CMD_DELAY, sizeof(video70_dzx), video70_dzx},

	{DTYPE_GEN_LWRITE, 1, 0, 0, OTM_CMD_DELAY, sizeof(video71_dzx), video71_dzx},
	{DTYPE_GEN_LWRITE, 1, 0, 0, OTM_CMD_DELAY, sizeof(video72_dzx), video72_dzx},
	{DTYPE_GEN_LWRITE, 1, 0, 0, OTM_CMD_DELAY, sizeof(video73_dzx), video73_dzx},
	{DTYPE_GEN_LWRITE, 1, 0, 0, OTM_CMD_DELAY, sizeof(video74_dzx), video74_dzx},
	{DTYPE_GEN_LWRITE, 1, 0, 0, OTM_CMD_DELAY, sizeof(video75_dzx), video75_dzx},
	{DTYPE_GEN_LWRITE, 1, 0, 0, OTM_CMD_DELAY, sizeof(video76_dzx), video76_dzx},
	{DTYPE_GEN_LWRITE, 1, 0, 0, OTM_CMD_DELAY, sizeof(video77_dzx), video77_dzx},
	{DTYPE_GEN_LWRITE, 1, 0, 0, OTM_CMD_DELAY, sizeof(video78_dzx), video78_dzx},

	{DTYPE_DCS_WRITE, 1, 0, 0, OTM_SLEEP_OFF_DELAY, sizeof(exit_sleep), exit_sleep},
	{DTYPE_DCS_WRITE, 1, 0, 0, OTM_DISPLAY_ON_DELAY, sizeof(display_on), display_on},
};


/* command mode setting */
static char cmd1[2] = {0x00, 0x00};
static char cmd2[4] = {0xFF, 0x80, 0x09, 0x01};
static char cmd3[2] = {0x00, 0x80};
static char cmd4[3] = {0xFF, 0x80, 0x09};
static char cmd5[2] = {0x00, 0xb4};
static char cmd6[2] = {0xc0, 0x10};
static char cmd7[2] = {0x00, 0x82};
static char cmd8[2] = {0xc5, 0xa3};
static char cmd9[2] = {0x00, 0x90};
static char cmd10[3] = {0xc5, 0x96, 0xB6};

static char cmd11[2] = {0x00, 0x00};
static char cmd12[3] = {0xd8, 0x87, 0x97};
static char cmd13[2] = {0x00, 0x00};
static char cmd14[2] = {0xd9, 0x59};
static char cmd15[2] = {0x00, 0x81};
static char cmd16[2] = {0xc1, 0x77};
static char cmd17[2] = {0x00, 0xa1};
static char cmd18[2] = {0xc1, 0x08};
static char cmd19[2] = {0x00, 0x89};
static char cmd20[2] = {0xc4, 0x08};
static char cmd21[2] = {0x00, 0xa3};
static char cmd22[2] = {0xc0, 0x00};
static char cmd23[2] = {0x00, 0x81};
static char cmd24[2] = {0xc4, 0x83};
static char cmd25[2] = {0x00, 0x92};
static char cmd26[2] = {0xc5, 0x01};
static char cmd27[2] = {0x00, 0xb1};
static char cmd28[2] = {0xc5, 0xa9};
static char cmd29[2] = {0x00, 0xc7};
static char cmd30[2] = {0xcf, 0x02};
static char cmd31[2] = {0x00, 0x90};
static char cmd32[2] = {0xb3, 0x02};
static char cmd33[2] = {0x00, 0x92};
static char cmd34[2] = {0xb3, 0x45};
static char cmd35[2] = {0x00, 0x80};
static char cmd36[10] = {
	0xc0, 0x00, 0x58, 0x00,
	0x15, 0x15, 0x00, 0x58,
	0x15, 0x15
};
static char cmd37[2] = {0x00, 0x90};
static char cmd38[7] = {
	0xc0, 0x00, 0x44, 0x00,
	0x00, 0x00, 0x03
};
static char cmd39[2] = {0x00, 0xa6};
static char cmd40[4] = {0xc1, 0x00, 0x00, 0x00};
static char cmd41[2] = {0x00, 0x80};
static char cmd42[13] = {
	0xce, 0x87, 0x03, 0x00,
	0x86, 0x03, 0x00, 0x85,
	0x03, 0x00, 0x84, 0x03,
	0x00
};
static char cmd43[2] = {0x00, 0x90};
static char cmd44[15] = {
	0xce, 0x33, 0x52, 0x00,
	0x33, 0x53, 0x00, 0x33,
	0x54, 0x00, 0x33, 0x55,
	0x00, 0x00, 0x00
};
static char cmd45[2] = {0x00, 0xa0};
static char cmd46[15] = {
	0xce, 0x38, 0x05, 0x03,
	0x56, 0x00, 0x00, 0x00,
	0x38, 0x04, 0x03, 0x57,
	0x00, 0x00, 0x00
};
static char cmd47[2] = {0x00, 0xb0};
static char cmd48[15] = {
	0xce, 0x38, 0x03, 0x03,
	0x58, 0x00, 0x00, 0x00,
	0x38, 0x02, 0x03, 0x59,
	0x00, 0x00, 0x00
};
static char cmd49[2] = {0x00, 0xc0};
static char cmd50[15] = {
	0xce, 0x38, 0x01, 0x03,
	0x5a, 0x00, 0x00, 0x00,
	0x38, 0x00, 0x03, 0x5c,
	0x00, 0x00, 0x00
};
static char cmd51[2] = {0x00, 0xd0};
static char cmd52[15] = {
	0xce, 0x30, 0x00, 0x03,
	0x5c, 0x00, 0x00, 0x00,
	0x30, 0x01, 0x03, 0x5d,
	0x00, 0x00, 0x00
};
static char cmd53[2] = {0x00, 0xc3};
static char cmd54[9] = {
	0xcb, 0x04, 0x04, 0x04,
	0x04, 0x04, 0x04, 0x04,
	0x04
};
static char cmd55[2] = {0x00, 0xd8};
static char cmd56[8] = {
	0xcb, 0x04, 0x04, 0x04,
	0x04, 0x04, 0x04, 0x04
};
static char cmd57[2] = {0x00, 0xe0};
static char cmd58[2] = {0xcb, 0x04};
static char cmd59[2] = {0x00, 0x83};
static char cmd60[8] = {
	0xcc, 0x03, 0x01, 0x09,
	0x0b, 0x0d, 0x0f, 0x05
};
static char cmd61[2] = {0x00, 0x90};
static char cmd62[2] = {0xcc, 0x07};
static char cmd63[2] = {0x00, 0x9d};
static char cmd64[3] = {0xcc, 0x04, 0x02};
static char cmd65[2] = {0x00, 0xa0};
static char cmd66[7] = {
	0xcc, 0x0a, 0x0c, 0x0e,
	0x10, 0x06, 0x08
};
static char cmd67[2] = {0x00, 0xb3};
static char cmd68[8] = {
	0xcc, 0x06, 0x08, 0x0a,
	0x10, 0x0e, 0x0c, 0x04
};
static char cmd69[2] = {0x00, 0xc0};
static char cmd70[2] = {0xcc, 0x02};
static char cmd71[2] = {0x00, 0xcd};
static char cmd72[3] = {0xcc, 0x05, 0x07};
static char cmd73[2] = {0x00, 0xd0};
static char cmd74[7] = {
	0xcc, 0x09, 0x0f, 0x0d,
	0x0b, 0x03, 0x01
};
static char cmd75[2] = {0x00, 0x00};
static char cmd76[17] = {
	0xE1, 0x09, 0x15, 0x20,
	0x11, 0x0B, 0x1C, 0x0E,
	0x0D, 0x00, 0x04, 0x03,
	0x05, 0x0D, 0x1E, 0x1B,
	0x14
};
static char cmd77[2] = {0x00, 0x00};
static char cmd78[17] = {
	0xE2, 0x09, 0x15, 0x20,
	0x11, 0x0B, 0x1C, 0x0E,
	0x0D, 0x00, 0x04, 0x03,
	0x05, 0x0D, 0x1E, 0x1B,
	0x14
};
static char cmd79[2] = {0x00, 0x00};
static char cmd80[4] = {0xff, 0xff, 0xff, 0xff};


static struct dsi_cmd_desc otm_cmd_display_on_cmds[] = {
	{DTYPE_GEN_LWRITE, 1, 0, 0, OTM_CMD_DELAY, sizeof(cmd1), cmd1},
	{DTYPE_GEN_LWRITE, 1, 0, 0, OTM_CMD_DELAY, sizeof(cmd2), cmd2},
	{DTYPE_GEN_LWRITE, 1, 0, 0, OTM_CMD_DELAY, sizeof(cmd3), cmd3},
	{DTYPE_GEN_LWRITE, 1, 0, 0, OTM_CMD_DELAY, sizeof(cmd4), cmd4},
	{DTYPE_GEN_LWRITE, 1, 0, 0, OTM_CMD_DELAY, sizeof(cmd5), cmd5},
	{DTYPE_GEN_LWRITE, 1, 0, 0, OTM_CMD_DELAY, sizeof(cmd6), cmd6},
	{DTYPE_GEN_LWRITE, 1, 0, 0, OTM_CMD_DELAY, sizeof(cmd7), cmd7},
	{DTYPE_GEN_LWRITE, 1, 0, 0, OTM_CMD_DELAY, sizeof(cmd8), cmd8},
	{DTYPE_GEN_LWRITE, 1, 0, 0, OTM_CMD_DELAY, sizeof(cmd9), cmd9},
	{DTYPE_GEN_LWRITE, 1, 0, 0, OTM_CMD_DELAY, sizeof(cmd10), cmd10},
	{DTYPE_GEN_LWRITE, 1, 0, 0, OTM_CMD_DELAY, sizeof(cmd11), cmd11},
	{DTYPE_GEN_LWRITE, 1, 0, 0, OTM_CMD_DELAY, sizeof(cmd12), cmd12},
	{DTYPE_GEN_LWRITE, 1, 0, 0, OTM_CMD_DELAY, sizeof(cmd13), cmd13},
	{DTYPE_GEN_LWRITE, 1, 0, 0, OTM_CMD_DELAY, sizeof(cmd14), cmd14},
	{DTYPE_GEN_LWRITE, 1, 0, 0, OTM_CMD_DELAY, sizeof(cmd15), cmd15},
	{DTYPE_GEN_LWRITE, 1, 0, 0, OTM_CMD_DELAY, sizeof(cmd16), cmd16},
	{DTYPE_GEN_LWRITE, 1, 0, 0, OTM_CMD_DELAY, sizeof(cmd17), cmd17},
	{DTYPE_GEN_LWRITE, 1, 0, 0, OTM_CMD_DELAY, sizeof(cmd18), cmd18},
	{DTYPE_GEN_LWRITE, 1, 0, 0, OTM_CMD_DELAY, sizeof(cmd19), cmd19},
	{DTYPE_GEN_LWRITE, 1, 0, 0, OTM_CMD_DELAY, sizeof(cmd20), cmd20},
	{DTYPE_GEN_LWRITE, 1, 0, 0, OTM_CMD_DELAY, sizeof(cmd21), cmd21},
	{DTYPE_GEN_LWRITE, 1, 0, 0, OTM_CMD_DELAY, sizeof(cmd22), cmd22},
	{DTYPE_GEN_LWRITE, 1, 0, 0, OTM_CMD_DELAY, sizeof(cmd23), cmd23},
	{DTYPE_GEN_LWRITE, 1, 0, 0, OTM_CMD_DELAY, sizeof(cmd24), cmd24},
	{DTYPE_GEN_LWRITE, 1, 0, 0, OTM_CMD_DELAY, sizeof(cmd25), cmd25},
	{DTYPE_GEN_LWRITE, 1, 0, 0, OTM_CMD_DELAY, sizeof(cmd26), cmd26},
	{DTYPE_GEN_LWRITE, 1, 0, 0, OTM_CMD_DELAY, sizeof(cmd27), cmd27},
	{DTYPE_GEN_LWRITE, 1, 0, 0, OTM_CMD_DELAY, sizeof(cmd28), cmd28},
	{DTYPE_GEN_LWRITE, 1, 0, 0, OTM_CMD_DELAY, sizeof(cmd29), cmd29},
	{DTYPE_GEN_LWRITE, 1, 0, 0, OTM_CMD_DELAY, sizeof(cmd30), cmd30},
	{DTYPE_GEN_LWRITE, 1, 0, 0, OTM_CMD_DELAY, sizeof(cmd31), cmd31},
	{DTYPE_GEN_LWRITE, 1, 0, 0, OTM_CMD_DELAY, sizeof(cmd32), cmd32},
	{DTYPE_GEN_LWRITE, 1, 0, 0, OTM_CMD_DELAY, sizeof(cmd33), cmd33},
	{DTYPE_GEN_LWRITE, 1, 0, 0, OTM_CMD_DELAY, sizeof(cmd34), cmd34},
	{DTYPE_GEN_LWRITE, 1, 0, 0, OTM_CMD_DELAY, sizeof(cmd35), cmd35},
	{DTYPE_GEN_LWRITE, 1, 0, 0, OTM_CMD_DELAY, sizeof(cmd36), cmd36},
	{DTYPE_GEN_LWRITE, 1, 0, 0, OTM_CMD_DELAY, sizeof(cmd37), cmd37},
	{DTYPE_GEN_LWRITE, 1, 0, 0, OTM_CMD_DELAY, sizeof(cmd38), cmd38},
	{DTYPE_GEN_LWRITE, 1, 0, 0, OTM_CMD_DELAY, sizeof(cmd39), cmd39},
	{DTYPE_GEN_LWRITE, 1, 0, 0, OTM_CMD_DELAY, sizeof(cmd40), cmd40},
	{DTYPE_GEN_LWRITE, 1, 0, 0, OTM_CMD_DELAY, sizeof(cmd41), cmd41},
	{DTYPE_GEN_LWRITE, 1, 0, 0, OTM_CMD_DELAY, sizeof(cmd42), cmd42},
	{DTYPE_GEN_LWRITE, 1, 0, 0, OTM_CMD_DELAY, sizeof(cmd43), cmd43},
	{DTYPE_GEN_LWRITE, 1, 0, 0, OTM_CMD_DELAY, sizeof(cmd44), cmd44},
	{DTYPE_GEN_LWRITE, 1, 0, 0, OTM_CMD_DELAY, sizeof(cmd45), cmd45},
	{DTYPE_GEN_LWRITE, 1, 0, 0, OTM_CMD_DELAY, sizeof(cmd46), cmd46},
	{DTYPE_GEN_LWRITE, 1, 0, 0, OTM_CMD_DELAY, sizeof(cmd47), cmd47},
	{DTYPE_GEN_LWRITE, 1, 0, 0, OTM_CMD_DELAY, sizeof(cmd48), cmd48},
	{DTYPE_GEN_LWRITE, 1, 0, 0, OTM_CMD_DELAY, sizeof(cmd49), cmd49},
	{DTYPE_GEN_LWRITE, 1, 0, 0, OTM_CMD_DELAY, sizeof(cmd50), cmd50},
	{DTYPE_GEN_LWRITE, 1, 0, 0, OTM_CMD_DELAY, sizeof(cmd51), cmd51},
	{DTYPE_GEN_LWRITE, 1, 0, 0, OTM_CMD_DELAY, sizeof(cmd52), cmd52},
	{DTYPE_GEN_LWRITE, 1, 0, 0, OTM_CMD_DELAY, sizeof(cmd53), cmd53},
	{DTYPE_GEN_LWRITE, 1, 0, 0, OTM_CMD_DELAY, sizeof(cmd54), cmd54},
	{DTYPE_GEN_LWRITE, 1, 0, 0, OTM_CMD_DELAY, sizeof(cmd55), cmd55},
	{DTYPE_GEN_LWRITE, 1, 0, 0, OTM_CMD_DELAY, sizeof(cmd56), cmd56},
	{DTYPE_GEN_LWRITE, 1, 0, 0, OTM_CMD_DELAY, sizeof(cmd57), cmd57},
	{DTYPE_GEN_LWRITE, 1, 0, 0, OTM_CMD_DELAY, sizeof(cmd58), cmd58},
	{DTYPE_GEN_LWRITE, 1, 0, 0, OTM_CMD_DELAY, sizeof(cmd59), cmd59},
	{DTYPE_GEN_LWRITE, 1, 0, 0, OTM_CMD_DELAY, sizeof(cmd60), cmd60},
	{DTYPE_GEN_LWRITE, 1, 0, 0, OTM_CMD_DELAY, sizeof(cmd61), cmd61},
	{DTYPE_GEN_LWRITE, 1, 0, 0, OTM_CMD_DELAY, sizeof(cmd62), cmd62},
	{DTYPE_GEN_LWRITE, 1, 0, 0, OTM_CMD_DELAY, sizeof(cmd63), cmd63},
	{DTYPE_GEN_LWRITE, 1, 0, 0, OTM_CMD_DELAY, sizeof(cmd64), cmd64},
	{DTYPE_GEN_LWRITE, 1, 0, 0, OTM_CMD_DELAY, sizeof(cmd65), cmd65},
	{DTYPE_GEN_LWRITE, 1, 0, 0, OTM_CMD_DELAY, sizeof(cmd66), cmd66},
	{DTYPE_GEN_LWRITE, 1, 0, 0, OTM_CMD_DELAY, sizeof(cmd67), cmd67},
	{DTYPE_GEN_LWRITE, 1, 0, 0, OTM_CMD_DELAY, sizeof(cmd68), cmd68},
	{DTYPE_GEN_LWRITE, 1, 0, 0, OTM_CMD_DELAY, sizeof(cmd69), cmd69},
	{DTYPE_GEN_LWRITE, 1, 0, 0, OTM_CMD_DELAY, sizeof(cmd70), cmd70},
	{DTYPE_GEN_LWRITE, 1, 0, 0, OTM_CMD_DELAY, sizeof(cmd71), cmd71},
	{DTYPE_GEN_LWRITE, 1, 0, 0, OTM_CMD_DELAY, sizeof(cmd72), cmd72},
	{DTYPE_GEN_LWRITE, 1, 0, 0, OTM_CMD_DELAY, sizeof(cmd73), cmd73},
	{DTYPE_GEN_LWRITE, 1, 0, 0, OTM_CMD_DELAY, sizeof(cmd74), cmd74},
	{DTYPE_GEN_LWRITE, 1, 0, 0, OTM_CMD_DELAY, sizeof(cmd75), cmd75},
	{DTYPE_GEN_LWRITE, 1, 0, 0, OTM_CMD_DELAY, sizeof(cmd76), cmd76},
	{DTYPE_GEN_LWRITE, 1, 0, 0, OTM_CMD_DELAY, sizeof(cmd77), cmd77},
	{DTYPE_GEN_LWRITE, 1, 0, 0, OTM_CMD_DELAY, sizeof(cmd78), cmd78},
	{DTYPE_GEN_LWRITE, 1, 0, 0, OTM_CMD_DELAY, sizeof(cmd79), cmd79},
	{DTYPE_GEN_LWRITE, 1, 0, 0, OTM_CMD_DELAY, sizeof(cmd80), cmd80},

//	{DTYPE_DCS_WRITE1, 1, 0, 0, OTM_CMD_DELAY, sizeof(swap_updown), swap_updown},
	{DTYPE_DCS_WRITE, 1, 0, 0, OTM_SLEEP_OFF_DELAY, sizeof(exit_sleep), exit_sleep},
	{DTYPE_DCS_WRITE, 1, 0, 0, OTM_DISPLAY_ON_DELAY, sizeof(display_on), display_on},
};

static int mipi_otm_lcd_on(struct platform_device *pdev)
{
	struct msm_fb_data_type *mfd;
	struct mipi_panel_info *mipi;

	pr_debug("mipi_otm_lcd_on hsd_fwvga E\n");

	mfd = platform_get_drvdata(pdev);

	if (!mfd) {
		return -ENODEV;
	}

	if (mfd->key != MFD_KEY) {
		return -EINVAL;
	}

	mipi  = &mfd->panel_info.mipi;
	pr_debug("%s: mode = %d\n", __func__, mipi->mode);
	/* select DSI mode */
	if (!mfd->cont_splash_done) {
		mfd->cont_splash_done = 1;
		gpio_set_value_cansleep(PWM_GPIO_EN, 0);
		/* gpio-high:dijing, gpio-low:dezhixin*/
		gpio_lcd_id = gpio_get_value(77);
		return 0;
	}

	if (mipi->mode == DSI_VIDEO_MODE) {
		if (gpio_lcd_id)
			mipi_dsi_cmds_tx(&otm_tx_buf, otm_video_display_on_cmds,
				ARRAY_SIZE(otm_video_display_on_cmds));
		else
			mipi_dsi_cmds_tx(&otm_tx_buf, otm_video_dzx_display_on_cmds,
				ARRAY_SIZE(otm_video_dzx_display_on_cmds));

	} else if (mipi->mode == DSI_CMD_MODE) {
		mipi_dsi_cmds_tx(&otm_tx_buf, otm_cmd_display_on_cmds,
						 ARRAY_SIZE(otm_cmd_display_on_cmds));
	}

	pr_debug("mipi_otm_lcd_on hsd_fwvga X\n");

	return 0;
}

static int mipi_otm_lcd_off(struct platform_device *pdev)
{
	struct msm_fb_data_type *mfd;

	pr_debug("mipi_otm_lcd_off E\n");

	mfd = platform_get_drvdata(pdev);

	if (!mfd)
		return -ENODEV;
	if (mfd->key != MFD_KEY)
		return -EINVAL;

//#ifndef MIPI_OTM_FAKE_PANEL
	mipi_dsi_cmds_tx(&otm_tx_buf, otm_display_off_cmds,
					 ARRAY_SIZE(otm_display_off_cmds));
//#endif

	pr_debug("mipi_otm_lcd_off X\n");

	return 0;
}

static int __devinit mipi_otm_lcd_probe(struct platform_device *pdev)
{
	int rc = 0;
	pr_debug("%s\n", __func__);

	if (pdev->id == 0) {
		mipi_otm_pdata = pdev->dev.platform_data;
		if (mipi_otm_pdata) {
			gpio_backlight_en = mipi_otm_pdata->gpio;
		}

		return rc;
	}

	msm_fb_add_device(pdev);

	return rc;
}

static struct platform_driver this_driver = {
	.probe  = mipi_otm_lcd_probe,
	.driver = {
		.name   = "mipi_otm",
	},
};

static void mipi_otm_set_backlight(struct msm_fb_data_type *mfd)
{
	int bl_level;

#ifndef CONFIG_FB_MSM_PWM_BACKLIGHT
	int step = 0, i = 0;
	unsigned long flags;
	int value;
#else
	int ret;
#endif

#ifdef CONFIG_FB_MSM_PWM_BACKLIGHT
	bl_level = mfd->bl_level;
	if (mipi_otm_pdata && mipi_otm_pdata->pmic_backlight)
		ret = mipi_otm_pdata->pmic_backlight(bl_level);
	else
		pr_err("%s(): Backlight level set failed", __func__);
#else

	value = mfd->bl_level;
	if (value > MAX_BACKLIGHT_BRIGHTNESS)
		value = MAX_BACKLIGHT_BRIGHTNESS;

	/* This maps android backlight level 0 to 255 into
	   driver backlight level 0 to bl_max with rounding */
	bl_level = (2 * value * MY_BACKLIGHT_MAX + MAX_BACKLIGHT_BRIGHTNESS)
			   / (2 * MAX_BACKLIGHT_BRIGHTNESS);

	if (!bl_level && value)
		bl_level = 1;
	bl_level = PREV_BK_L - bl_level;
	//printk("%s: prev_bl = %d, bl_level = %d\n", __func__, prev_bl, bl_level);
	if (bl_level > prev_bl) {
		step = bl_level - prev_bl;
		if (bl_level == PREV_BK_L) {
			step--;
		}
	} else if (bl_level < prev_bl) {
		step = bl_level + MY_BACKLIGHT_MAX - prev_bl;
	} else {
		step = bl_level + MY_BACKLIGHT_MAX - prev_bl;
		return;
	}

	if (bl_level == PREV_BK_L) {
		/* turn off backlight */
		printk("%s: turn off backlight\n", __func__);
		gpio_set_value_cansleep(PWM_GPIO_EN, 0);
	} else {
		local_irq_save(flags);

		if (prev_bl == PREV_BK_L) {
			/* turn on backlight */
			gpio_set_value_cansleep(PWM_GPIO_EN, 1);
			udelay(40);
		}
		/* adjust backlight level */
		for (i = 0; i < step; i++) {
			gpio_set_value_cansleep(PWM_GPIO_EN, 0);
			udelay(1);
			gpio_set_value_cansleep(PWM_GPIO_EN, 1);
			udelay(1);
		}

		local_irq_restore(flags);
	}
	msleep(1);
	prev_bl = bl_level;

#endif
	return;
}

static struct msm_fb_panel_data otm_panel_data = {
	.on	= mipi_otm_lcd_on,
	.off = mipi_otm_lcd_off,
	.set_backlight = mipi_otm_set_backlight,
};
static int mipi_otm_lcd_init(void)
{
	pr_debug("mipi_otm_lcd_init\n");

	mipi_dsi_buf_alloc(&otm_tx_buf, DSI_BUF_SIZE);
	mipi_dsi_buf_alloc(&otm_rx_buf, DSI_BUF_SIZE);

	return platform_driver_register(&this_driver);
}

static int ch_used[3];

int mipi_otm_device_register(struct msm_panel_info *pinfo,
							 u32 channel, u32 panel)
{
	struct platform_device *pdev = NULL;
	int ret;

	pr_debug("mipi_otm_device_register\n");

	if ((channel >= 3) || ch_used[channel])
		return -ENODEV;

	ch_used[channel] = TRUE;
	ret = mipi_otm_lcd_init();
	if (ret) {
		pr_err("mipi_otm_lcd_init() failed with ret %u\n", ret);
		return ret;
	}

	pdev = platform_device_alloc("mipi_otm", (panel << 8) | channel);
	if (!pdev)
		return -ENOMEM;

	otm_panel_data.panel_info = *pinfo;

	ret = platform_device_add_data(pdev, &otm_panel_data,
								   sizeof(otm_panel_data));
	if (ret) {
		pr_debug("%s: platform_device_add_data failed!\n", __func__);
		goto err_device_put;
	}

	ret = platform_device_add(pdev);
	if (ret) {
		pr_debug("%s: platform_device_register failed!\n", __func__);
		goto err_device_put;
	}

	return 0;

err_device_put:
	platform_device_put(pdev);
	return ret;
}

