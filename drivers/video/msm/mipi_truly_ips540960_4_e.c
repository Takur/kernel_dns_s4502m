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
#define DEBUG

#include <mach/socinfo.h>

#include "msm_fb.h"
#include "mipi_dsi.h"
#include "mipi_truly_ips540960_4_e.h"

static struct msm_panel_common_pdata *mipi_truly_ips540960_4_e_pdata;
static struct dsi_buf truly_ips540960_4_e_tx_buf;
static struct dsi_buf truly_ips540960_4_e_rx_buf;

static int mipi_truly_ips540960_4_e_bl_ctrl = 0;

/* common setting */
static char exit_sleep[2] = {0x11, 0x00};
static char display_on[2] = {0x29, 0x00};
static char display_off[2] = {0x28, 0x00};
static char enter_sleep[2] = {0x10, 0x00};
static char write_ram[2] = {0x2c, 0x00}; /* write ram */

static struct dsi_cmd_desc truly_ips540960_4_e_display_off_cmds[] = {
	{DTYPE_DCS_WRITE, 1, 0, 0, 150, sizeof(display_off), display_off},
	{DTYPE_DCS_WRITE, 1, 0, 0, 150, sizeof(enter_sleep), enter_sleep}
};

static char para1[6]={0xFF,0xAA,0x55,0x25,0x01,0x01};
static char para2[31]={0xF2,0x00,0x00,0x4A,0x0A,0xA8,
	0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,
	0x0B,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x40,
	0x01};
static char para3[8]={0xF3,0x02,0x03,0x07,0x45,0x88,0xD1,0x0D};

static char para4[6]={0xF0,0x55,0xAA,0x52,0x08,0x00};

static char para5[4]={0xB1,0xFC,0x06,0x00};
static char para6[5]={0xB8,0x01,0x02,0x02,0x02};
static char para7[7]={0xC9,0x63,0x06,0x0D,0x1A,0x17,0x00};

static char para8[6]={0xF0,0x55,0xAA,0x52,0x08,0x01};
static char para9[4]={0xB0,0x05,0x05,0x05};
static char para10[4]={0xB1,0x05,0x05,0x05};
static char para11[4]={0xB2,0x01,0x01,0x01};
static char para12[4]={0xB3,0x0E,0x0E,0x0E};
static char para13[4]={0xB4,0x08,0x08,0x08};
static char para14[4]={0xB6,0x44,0x44,0x44};
static char para15[4]={0xB7,0x34,0x34,0x34};
static char para16[4]={0xB8,0x10,0x10,0x10};
static char para17[4]={0xB9,0x26,0x26,0x26};
static char para18[4]={0xBA,0x24,0x24,0x24};
static char para19[4]={0xBC,0x00,0xC8,0x00};
static char para20[4]={0xBD,0x00,0xC8,0x00};
static char para21[2]={0xBE,0x7b};
static char para22[3]={0xC0,0x04,0x00};
static char para23[2]={0xCA,0x00};
static char para24[5]={0xD0,0x0F, 0x0F, 0x10,0x10};
static char para25[17]={0xD1,0x00,0x70,0x00,0x83,
	0x00,0xA0,0x00,0xB9,0x00,
	0xCC,0x00,0xED,0x01,0x0C,
	0x01,0x3C};
static char para26[17]={0xD2,0x01,0x62,0x01,0x9F,
	0x01,0xCE,0x02,0x14,0x02,
	0x50,0x02,0x51,0x02,0x88,
	0x02,0xC6};
static char para27[17]={0xD3,0x02,0xED,0x03,0x20,
	0x03,0x41,0x03,0x67,0x03,
	0x80,0x03,0x9F,0x03,0xAF,
	0x03,0xEE};
static char para28[5]={0xD4,0x03,0xFF,0x03,0xFF};
static char para29[17]={0xD5,0x00,0x70,0x00,0x83,
	0x00,0xA0,0x00,0xB9,0x00,
	0xCC,0x00,0xED,0x01,0x0C,
	0x01,0x3C};
static char para30[17]={0xD6,0x01,0x62,0x01,0x9F,
	0x01,0xCE,0x02,0x14,0x02,
	0x50,0x02,0x51,0x02,0x88,
	0x02,0xC6};
static char para31[17]={0xD7,0x02,0xED,0x03,0x20,
	0x03,0x41,0x03,0x67,0x03,
	0x80,0x03,0x9F,0x03,0xAF,
	0x03,0xEE};
static char para32[5]={0xD8, 0x03,0xFF,0x03,0xFF};
static char para33[17]={0xD9,0x00,0x70,0x00,0x83,
	0x00,0xA0,0x00,0xB9,0x00,
	0xCC,0x00,0xED,0x01,0x0C,
	0x01,0x3C};
static char para34[17]={0xDD,0x01,0x62,0x01,0x9F,
	0x01,0xCE,0x02,0x14,0x02,
	0x50,0x02,0x51,0x02,0x88,
	0x02,0xC6};
static char para35[17]={0xDE, 0x02,0xED,0x03,0x20,
	0x03,0x41,0x03,0x67,0x03,
	0x80,0x03,0x9F,0x03,0xAF,
	0x03,0xEE};
static char para36[5]={0xDF,0x03,0xFF,0x03,0xFF};
static char para37[17]={0xE0,0x00,0x70,0x00,0x83,
	0x00,0xA0,0x00,0xB9,0x00,
	0xCC,0x00,0xED,0x01,0x0C,
	0x01,0x3C};
static char para38[17]={0xE1,0x01,0x62,0x01,0x9F,
	0x01,0xCE,0x02,0x14,0x02,
	0x50,0x02,0x51,0x02,0x88,
	0x02,0xC6};
static char para39[17]={0xE2, 0x02,0xED,0x03,0x20,
	0x03,0x41,0x03,0x67,0x03,
	0x80,0x03,0x9F,0x03,0xAF,
	0x03,0xEE};
static char para40[5]={ 0xE3,0x03,0xFF,0x03,0xFF};
static char para41[17]={0xE4,0x00,0x70,0x00,0x83,
	0x00,0xA0,0x00,0xB9,0x00,
	0xCC,0x00,0xED,0x01,0x0C,
	0x01,0x3C};
static char para42[17]={0xE5,0x01,0x62,0x01,0x9F,
	0x01,0xCE,0x02,0x14,0x02,
	0x50,0x02,0x51,0x02,0x88,
	0x02,0xC6};
static char para43[17]={0xE6,0x02,0xED,0x03,0x20,
	0x03,0x41,0x03,0x67,0x03,
	0x80,0x03,0x9F,0x03,0xAF,
	0x03,0xEE};
static char para44[5]={ 0xE7, 0x03,0xFF,0x03,0xFF};
static char para45[17]={0xE8,0x00,0x70,0x00,0x83,
	0x00,0xA0,0x00,0xB9,0x00,
	0xCC,0x00,0xED,0x01,0x0C,
	0x01,0x3C};
static char para46[17]={0xE9,0x01,0x62,0x01,0x9F,
	0x01,0xCE,0x02,0x14,0x02,
	0x50,0x02,0x51,0x02,0x88,
	0x02,0xC6};
static char para47[17]={0xEA,0x02,0xED,0x03,0x20,
	0x03,0x41,0x03,0x67,0x03,
	0x80,0x03,0x9F,0x03,0xAF,
	0x03,0xEE};
static char para48[5]={0xEB, 0x03,0xFF,0x03,0xFF};   
static char para49[2]={0x2C,0x00};
static char para50[2]={0x13,0x00};

static struct dsi_cmd_desc truly_ips540960_4_e_cmd_display_on_cmds[] = {
	{DTYPE_DCS_LWRITE, 1, 0, 0, 50, sizeof(para1),  para1},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 50, sizeof(para2),  para2},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 50, sizeof(para3),  para3},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 50, sizeof(para4),  para4},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 50, sizeof(para5),  para5},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 50, sizeof(para6),  para6},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 50, sizeof(para7),  para7},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 50, sizeof(para8),  para8},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 50, sizeof(para9),  para9},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 50, sizeof(para10), para10},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 50, sizeof(para11), para11},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 50, sizeof(para12), para12},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 50, sizeof(para13), para13},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 50, sizeof(para14), para14},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 50, sizeof(para15), para15},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 50, sizeof(para16), para16},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 50, sizeof(para17), para17},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 50, sizeof(para18), para18},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 50, sizeof(para19), para19},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 50, sizeof(para20), para20},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 50, sizeof(para21), para21},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 50, sizeof(para22), para22},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 50, sizeof(para23), para23},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 50, sizeof(para24), para24},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 50, sizeof(para25), para25},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 50, sizeof(para26), para26},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 50, sizeof(para27), para27},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 50, sizeof(para28), para28},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 50, sizeof(para29), para29},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 50, sizeof(para30),  para30},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 50, sizeof(para31),  para31},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 50, sizeof(para32),  para32},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 50, sizeof(para33),  para33},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 50, sizeof(para34),  para34},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 50, sizeof(para35),  para35},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 50, sizeof(para36),  para36},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 50, sizeof(para37),  para37},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 50, sizeof(para38),  para38},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 50, sizeof(para39),  para39},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 50, sizeof(para40),  para40},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 50, sizeof(para41),  para41},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 50, sizeof(para42),  para42},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 50, sizeof(para43),  para43},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 50, sizeof(para44),  para44},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 50, sizeof(para45),  para45},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 50, sizeof(para46),  para46},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 50, sizeof(para47),  para47},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 50, sizeof(para48),  para48},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 50, sizeof(para49),  para49},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 50, sizeof(para50),  para50},

	{DTYPE_DCS_WRITE, 1, 0, 0, 150,	sizeof(exit_sleep), exit_sleep},
	{DTYPE_DCS_WRITE, 1, 0, 0, 150,	sizeof(display_on), display_on},

	{DTYPE_DCS_WRITE, 1, 0, 0, 10,  sizeof(write_ram), write_ram},
};

static struct dsi_cmd_desc truly_ips540960_4_e_video_display_on_cmds[] = {
	{DTYPE_DCS_LWRITE, 1, 0, 0, 50, sizeof(para1),  para1},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 50, sizeof(para2),  para2},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 50, sizeof(para3),  para3},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 50, sizeof(para4),  para4},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 50, sizeof(para5),  para5},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 50, sizeof(para6),  para6},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 50, sizeof(para7),  para7},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 50, sizeof(para8),  para8},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 50, sizeof(para9),  para9},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 50, sizeof(para10), para10},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 50, sizeof(para11), para11},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 50, sizeof(para12), para12},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 50, sizeof(para13), para13},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 50, sizeof(para14), para14},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 50, sizeof(para15), para15},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 50, sizeof(para16), para16},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 50, sizeof(para17), para17},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 50, sizeof(para18), para18},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 50, sizeof(para19), para19},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 50, sizeof(para20), para20},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 50, sizeof(para21), para21},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 50, sizeof(para22), para22},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 50, sizeof(para23), para23},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 50, sizeof(para24), para24},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 50, sizeof(para25), para25},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 50, sizeof(para26), para26},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 50, sizeof(para27), para27},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 50, sizeof(para28), para28},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 50, sizeof(para29), para29},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 50, sizeof(para30),  para30},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 50, sizeof(para31),  para31},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 50, sizeof(para32),  para32},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 50, sizeof(para33),  para33},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 50, sizeof(para34),  para34},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 50, sizeof(para35),  para35},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 50, sizeof(para36),  para36},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 50, sizeof(para37),  para37},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 50, sizeof(para38),  para38},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 50, sizeof(para39),  para39},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 50, sizeof(para40),  para40},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 50, sizeof(para41),  para41},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 50, sizeof(para42),  para42},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 50, sizeof(para43),  para43},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 50, sizeof(para44),  para44},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 50, sizeof(para45),  para45},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 50, sizeof(para46),  para46},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 50, sizeof(para47),  para47},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 50, sizeof(para48),  para48},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 50, sizeof(para49),  para49},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 50, sizeof(para50),  para50},

	{DTYPE_DCS_WRITE, 1, 0, 0, 150,	sizeof(exit_sleep), exit_sleep},
	{DTYPE_DCS_WRITE, 1, 0, 0, 130,	sizeof(display_on), display_on},
};

static int mipi_truly_ips540960_4_e_lcd_on(struct platform_device *pdev)
{
	struct msm_fb_data_type *mfd;
	struct mipi_panel_info *mipi;

	pr_debug("mipi_truly_ips540960_4_e_lcd_on E\n");

	mfd = platform_get_drvdata(pdev);
	if (!mfd)
		return -ENODEV;

	if (mfd->key != MFD_KEY)
		return -EINVAL;

	mipi  = &mfd->panel_info.mipi;

	if (mipi->mode == DSI_VIDEO_MODE) {
		mipi_dsi_cmds_tx(&truly_ips540960_4_e_tx_buf,
				truly_ips540960_4_e_video_display_on_cmds,
				ARRAY_SIZE(truly_ips540960_4_e_video_display_on_cmds));
	} else if (mipi->mode == DSI_CMD_MODE) {
		mipi_dsi_cmds_tx(&truly_ips540960_4_e_tx_buf,
				truly_ips540960_4_e_cmd_display_on_cmds,
				ARRAY_SIZE(truly_ips540960_4_e_cmd_display_on_cmds));
	}

	pr_debug("mipi_truly_ips540960_4_e_lcd_on X\n");

	return 0;
}

static int mipi_truly_ips540960_4_e_lcd_off(struct platform_device *pdev)
{
	struct msm_fb_data_type *mfd;

	pr_debug("mipi_truly_ips540960_4_e_lcd_off E\n");

	mfd = platform_get_drvdata(pdev);

	if (!mfd)
		return -ENODEV;
	if (mfd->key != MFD_KEY)
		return -EINVAL;

	mipi_dsi_cmds_tx(&truly_ips540960_4_e_tx_buf, truly_ips540960_4_e_display_off_cmds,
			ARRAY_SIZE(truly_ips540960_4_e_display_off_cmds));

	pr_debug("mipi_truly_ips540960_4_e_lcd_off X\n");
	return 0;
}

static ssize_t mipi_truly_ips540960_4_e_wta_bl_ctrl(struct device *dev,
		struct device_attribute *attr, const char *buf, size_t count)
{
	ssize_t ret = strnlen(buf, PAGE_SIZE);
	int err;

	err =  kstrtoint(buf, 0, &mipi_truly_ips540960_4_e_bl_ctrl);
	if (err)
		return ret;

	pr_info("%s: bl ctrl set to %d\n", __func__, mipi_truly_ips540960_4_e_bl_ctrl);

	return ret;
}

static DEVICE_ATTR(bl_ctrl, S_IWUSR, NULL, mipi_truly_ips540960_4_e_wta_bl_ctrl);

static struct attribute *mipi_truly_ips540960_4_e_fs_attrs[] = {
	&dev_attr_bl_ctrl.attr,
	NULL,
};

static struct attribute_group mipi_truly_ips540960_4_e_fs_attr_group = {
	.attrs = mipi_truly_ips540960_4_e_fs_attrs,
};

static int mipi_truly_ips540960_4_e_create_sysfs(struct platform_device *pdev)
{
	int rc;
	struct msm_fb_data_type *mfd = platform_get_drvdata(pdev);

	if (!mfd) {
		pr_err("%s: mfd not found\n", __func__);
		return -ENODEV;
	}
	if (!mfd->fbi) {
		pr_err("%s: mfd->fbi not found\n", __func__);
		return -ENODEV;
	}
	if (!mfd->fbi->dev) {
		pr_err("%s: mfd->fbi->dev not found\n", __func__);
		return -ENODEV;
	}
	rc = sysfs_create_group(&mfd->fbi->dev->kobj,
			&mipi_truly_ips540960_4_e_fs_attr_group);
	if (rc) {
		pr_err("%s: sysfs group creation failed, rc=%d\n",
				__func__, rc);
		return rc;
	}

	return 0;
}

static int __devinit mipi_truly_ips540960_4_e_lcd_probe(struct platform_device *pdev)
{
	struct platform_device *pthisdev = NULL;
	pr_debug("%s\n", __func__);

	if (pdev->id == 0) {
		mipi_truly_ips540960_4_e_pdata = pdev->dev.platform_data;
		if (mipi_truly_ips540960_4_e_pdata->bl_lock)
			spin_lock_init(&mipi_truly_ips540960_4_e_pdata->bl_spinlock);

		/* PVT use PWM as backlight control method */
		if(machine_is_msm8625_qrd5() && hw_version_is(3, 0))
			mipi_truly_ips540960_4_e_bl_ctrl = 1;

		return 0;
	}

	pthisdev = msm_fb_add_device(pdev);
	mipi_truly_ips540960_4_e_create_sysfs(pthisdev);

	return 0;
}

static struct platform_driver this_driver = {
	.probe  = mipi_truly_ips540960_4_e_lcd_probe,
	.driver = {
		.name   = "mipi_truly_ips540960_4_e",
	},
};

static int old_bl_level;

static void mipi_truly_ips540960_4_e_set_backlight(struct msm_fb_data_type *mfd)
{
	int bl_level;
	unsigned long flags;
	bl_level = mfd->bl_level;

	if (mipi_truly_ips540960_4_e_pdata->bl_lock) {
		if (!mipi_truly_ips540960_4_e_bl_ctrl) {
			/* Level received is of range 1 to bl_max,
			   We need to convert the levels to 1
			   to 31 */
			bl_level = (2 * bl_level * 31 + mfd->panel_info.bl_max)
				/(2 * mfd->panel_info.bl_max);
			if (bl_level == old_bl_level)
				return;

			if (bl_level == 0)
				mipi_truly_ips540960_4_e_pdata->backlight(0, 1);

			if (old_bl_level == 0)
				mipi_truly_ips540960_4_e_pdata->backlight(50, 1);

			spin_lock_irqsave(&mipi_truly_ips540960_4_e_pdata->bl_spinlock,
					flags);
			mipi_truly_ips540960_4_e_pdata->backlight(bl_level, 0);
			spin_unlock_irqrestore(&mipi_truly_ips540960_4_e_pdata->bl_spinlock,
					flags);
			old_bl_level = bl_level;
		} else {
			mipi_truly_ips540960_4_e_pdata->backlight(bl_level, 1);
		}
	} else {
		mipi_truly_ips540960_4_e_pdata->backlight(bl_level, mipi_truly_ips540960_4_e_bl_ctrl);
	}
}

static struct msm_fb_panel_data truly_ips540960_4_e_panel_data = {
	.on	= mipi_truly_ips540960_4_e_lcd_on,
	.off = mipi_truly_ips540960_4_e_lcd_off,
	.set_backlight = mipi_truly_ips540960_4_e_set_backlight,
};

static int ch_used[3];

static int mipi_truly_ips540960_4_e_lcd_init(void)
{
	mipi_dsi_buf_alloc(&truly_ips540960_4_e_tx_buf, DSI_BUF_SIZE);
	mipi_dsi_buf_alloc(&truly_ips540960_4_e_rx_buf, DSI_BUF_SIZE);

	return platform_driver_register(&this_driver);
}
int mipi_truly_ips540960_4_e_device_register(struct msm_panel_info *pinfo,
		u32 channel, u32 panel)
{
	struct platform_device *pdev = NULL;
	int ret;

	if ((channel >= 3) || ch_used[channel])
		return -ENODEV;

	ch_used[channel] = TRUE;

	ret = mipi_truly_ips540960_4_e_lcd_init();
	if (ret) {
		pr_err("mipi_truly_ips540960_4_e_lcd_init() failed with ret %u\n", ret);
		return ret;
	}

	pdev = platform_device_alloc("mipi_truly_ips540960_4_e", (panel << 8)|channel);
	if (!pdev)
		return -ENOMEM;

	truly_ips540960_4_e_panel_data.panel_info = *pinfo;
	ret = platform_device_add_data(pdev, &truly_ips540960_4_e_panel_data,
			sizeof(truly_ips540960_4_e_panel_data));
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
