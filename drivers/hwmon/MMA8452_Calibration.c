//#include "MMA8452_Calibration.h"
#include <linux/i2c.h>
#include <linux/delay.h>

#define MMA8452_1G_Value_At_8G 256
#define MMA8452_1G_Value_At_4G 512
#define MMA8452_1G_Value_At_2G 1024
#define CalibrationTimes 3
#define Cal_Tolerance 4

unsigned char Rx_Buffer[6]; //IIC receiving buffer
//unsigned char Tx_Buffer[]; //IIC trasmitting buffer, first byte is Command

extern struct i2c_client *mma8452_i2c_client;
typedef struct {
	signed short  X;
	signed short  Y;
	signed short  Z;
} SPARAMETERS;


unsigned char Calibration_MMA8452(void);
unsigned char MMA8452_Sample_n_Times(unsigned char n, signed short *pX, signed short *pY, signed short *pZ);

unsigned char MMA8452_Read_XYZ8(signed char *x, signed char *y, signed char *z);
unsigned char MMA8452_Data_Is_Ready(void);
unsigned char MMA8452_Read_XYZ12(signed short *x, signed short *y, signed short *z);
enum {
	MMA8452_STATUS = 0x00,
	MMA8452_OUT_X_MSB,
	MMA8452_OUT_X_LSB,
	MMA8452_OUT_Y_MSB,
	MMA8452_OUT_Y_LSB,
	MMA8452_OUT_Z_MSB,
	MMA8452_OUT_Z_LSB,

	MMA8452_F_SETUP = 0x09,
	MMA8452_TRIG_CFG,
	MMA8452_SYSMOD,
	MMA8452_INT_SOURCE,
	MMA8452_WHO_AM_I,
	MMA8452_XYZ_DATA_CFG,
	MMA8452_HP_FILTER_CUTOFF,

	MMA8452_PL_STATUS,
	MMA8452_PL_CFG,
	MMA8452_PL_COUNT,
	MMA8452_PL_BF_ZCOMP,
	MMA8452_P_L_THS_REG,

	MMA8452_FF_MT_CFG,
	MMA8452_FF_MT_SRC,
	MMA8452_FF_MT_THS,
	MMA8452_FF_MT_COUNT,

	MMA8452_TRANSIENT_CFG = 0x1D,
	MMA8452_TRANSIENT_SRC,
	MMA8452_TRANSIENT_THS,
	MMA8452_TRANSIENT_COUNT,

	MMA8452_PULSE_CFG,
	MMA8452_PULSE_SRC,
	MMA8452_PULSE_THSX,
	MMA8452_PULSE_THSY,
	MMA8452_PULSE_THSZ,
	MMA8452_PULSE_TMLT,
	MMA8452_PULSE_LTCY,
	MMA8452_PULSE_WIND,

	MMA8452_ASLP_COUNT,
	MMA8452_CTRL_REG1,
	MMA8452_CTRL_REG2,
	MMA8452_CTRL_REG3,
	MMA8452_CTRL_REG4,
	MMA8452_CTRL_REG5,

	MMA8452_OFF_X,
	MMA8452_OFF_Y,
	MMA8452_OFF_Z,

	MMA8452_REG_END,
};
s32 i2c_smbus_read_byte_data1(struct i2c_client *client, u8 command)
{

	Rx_Buffer[0] = i2c_smbus_read_byte_data(mma8452_i2c_client, command);
	return Rx_Buffer[0];

}
unsigned char Calibration_MMA8452(void)
{
	SPARAMETERS gValue;
	unsigned char temp8u1, temp8u2, i;
	unsigned short temp16u1;
	signed short x1, y1, z1;

#ifdef _FOR_DEBUG_CALIBRATION_
	unsigned char temp8u3;
#endif

	if (!i2c_smbus_read_byte_data1(mma8452_i2c_client, MMA8452_CTRL_REG1)); // return 0;
	temp8u1 = Rx_Buffer[0];
	if (!i2c_smbus_read_byte_data1(mma8452_i2c_client, MMA8452_XYZ_DATA_CFG)); // return 0;//i2c_smbus_read_byte_data(mma8452_i2c_client,
	temp8u2 = Rx_Buffer[0];

	(void)i2c_smbus_write_byte_data(mma8452_i2c_client, MMA8452_CTRL_REG1, 0x00);
	(void)i2c_smbus_write_byte_data(mma8452_i2c_client, MMA8452_XYZ_DATA_CFG,  0b00000000);   //2g mode

	(void)i2c_smbus_write_byte_data(mma8452_i2c_client, MMA8452_OFF_X, 0x00);
	(void)i2c_smbus_write_byte_data(mma8452_i2c_client, MMA8452_OFF_Y, 0x00);
	(void)i2c_smbus_write_byte_data(mma8452_i2c_client, MMA8452_OFF_Z, 0x00);
	(void)i2c_smbus_write_byte_data(mma8452_i2c_client, MMA8452_CTRL_REG1, 0b00100101);
	printk("%s>x_off = %d y_off = %d z_off = %d>>\n", __func__, MMA8452_OFF_X, MMA8452_OFF_Y, MMA8452_OFF_Z);

	//00000000
	//   |  ||
	//   |  ++- FS: 00 - 2g
	//   |          01 - 4g
	//   |          10 - 8g
	//   |          11 - Reserved
	//   +----- HPF_OUT:  0 - Output data without high pass filter
	//                    1 - Output data with high pass filtered
	//
//  (void)i2c_smbus_write_byte_data(mma8452_i2c_client,MMA8452_CTRL_REG1,  0b00010101);
	(void)i2c_smbus_write_byte_data(mma8452_i2c_client, MMA8452_CTRL_REG1,  0b00100101);
	//00000000
	//||||||||
	//|||||||+- ACTIVE:  0 - Standby Mode
	//|||||||            1 - Active Mode
	//||||||+- F_READ:   0 - Normal Read Mode
	//||||||             1 - Fast Read Mode
	//|||||+- LNOISE:    0 - Normal Noise Mode
	//|||||              1 - Reduced Noise Mode
	//||+++--- DR[2:0]:  000 - 800Hz
	//||                 001 - 400Hz
	//||                 010 - 200Hz
	//||                 011 - 100Hz
	//||                 100 - 50Hz
	//||                 101 - 12.5Hz
	//||                 110 - 6.25Hz
	//||                 111 - 1.56Hz
	//++------ ASLP_RATE[1:0]: 00 - 50Hz
	//                         01 - 12.5Hz
	//                         10 - 6.25Hz
	//                         11 - 1.56Hz
	if (!MMA8452_Read_XYZ12(&gValue.X, &gValue.Y, &gValue.Z)) return 0;     //Redundant reading to clear data ready flags
	//exit if reading data failed

	for (i = 0; i < 3; i++) {
		if (!MMA8452_Sample_n_Times(4, &gValue.X, &gValue.Y, &gValue.Z)) return 0; //Reading data failed

		gValue.X *= -1;
		if (!i2c_smbus_read_byte_data1(mma8452_i2c_client, MMA8452_OFF_X)); // return 0;
		temp16u1 = ((unsigned short)Rx_Buffer[0]) & 0x00FF;
		if (temp16u1 & 0x0080) temp16u1 |= 0xFF80;
		gValue.X += (signed short)temp16u1;
		if (abs(gValue.X) > 127) {
			(void)i2c_smbus_write_byte_data(mma8452_i2c_client, MMA8452_CTRL_REG1, 0x00);
			(void)i2c_smbus_write_byte_data(mma8452_i2c_client, MMA8452_XYZ_DATA_CFG, temp8u2);
			(void)i2c_smbus_write_byte_data(mma8452_i2c_client, MMA8452_CTRL_REG1, temp8u1);
			return 0; //Calibration X failed
		}

		gValue.Y *= -1;
		if (!i2c_smbus_read_byte_data1(mma8452_i2c_client, MMA8452_OFF_Y)); // return 0;
		temp16u1 = ((unsigned short)Rx_Buffer[0]) & 0x00FF;
		if (temp16u1 & 0x0080) temp16u1 |= 0xFF80;
		gValue.Y += (signed short)temp16u1;
		if (abs(gValue.Y) > 127) {
			(void)i2c_smbus_write_byte_data(mma8452_i2c_client, MMA8452_CTRL_REG1, 0x00);
			(void)i2c_smbus_write_byte_data(mma8452_i2c_client, MMA8452_XYZ_DATA_CFG, temp8u2);
			(void)i2c_smbus_write_byte_data(mma8452_i2c_client, MMA8452_CTRL_REG1, temp8u1);
			return 0; //Calibration Y failed
		}

		if (gValue.Z >= 0) gValue.Z -= MMA8452_1G_Value_At_2G;
		else gValue.Z += MMA8452_1G_Value_At_2G;
		gValue.Z *= -1;
		if (!i2c_smbus_read_byte_data1(mma8452_i2c_client, MMA8452_OFF_Z)); // return 0;
		temp16u1 = ((unsigned short)Rx_Buffer[0]) & 0x00FF;
		if (temp16u1 & 0x0080) temp16u1 |= 0xFF80;
		gValue.Z += (signed short)temp16u1;
		if (abs(gValue.Z) > 127) {
			(void)i2c_smbus_write_byte_data(mma8452_i2c_client, MMA8452_CTRL_REG1, 0x00);
			(void)i2c_smbus_write_byte_data(mma8452_i2c_client, MMA8452_XYZ_DATA_CFG, temp8u2);
			(void)i2c_smbus_write_byte_data(mma8452_i2c_client, MMA8452_CTRL_REG1, temp8u1);
			return 0; //Calibration Z failed
		}

		(void)i2c_smbus_write_byte_data(mma8452_i2c_client, MMA8452_CTRL_REG1, 0x00);
		(void)i2c_smbus_write_byte_data(mma8452_i2c_client, MMA8452_OFF_X, (unsigned char)(gValue.X & 0x00FF));
		(void)i2c_smbus_write_byte_data(mma8452_i2c_client, MMA8452_OFF_Y, (unsigned char)(gValue.Y & 0x00FF));
		(void)i2c_smbus_write_byte_data(mma8452_i2c_client, MMA8452_OFF_Z, (unsigned char)(gValue.Z & 0x00FF));
		(void)i2c_smbus_write_byte_data(mma8452_i2c_client, MMA8452_CTRL_REG1, 0b00100101);

		Rx_Buffer[0] = i2c_smbus_read_byte_data(mma8452_i2c_client, MMA8452_OFF_X);
		Rx_Buffer[1] = i2c_smbus_read_byte_data(mma8452_i2c_client, MMA8452_OFF_Y);
		Rx_Buffer[2] = i2c_smbus_read_byte_data(mma8452_i2c_client, MMA8452_OFF_Z);

		if (!MMA8452_Read_XYZ12(&gValue.X, &gValue.Y, &gValue.Z)) return 0;     //Redundant reading to clear data ready flags
	}

	i = 1;
	(void)MMA8452_Sample_n_Times(16, &gValue.X, &gValue.Y, &gValue.Z);
	if (abs(gValue.X) > Cal_Tolerance) i = 0;
	if (abs(gValue.Y) > Cal_Tolerance) i = 0;
	if (abs(abs(gValue.Z) - MMA8452_1G_Value_At_2G) > Cal_Tolerance) i = 0;

	(void)i2c_smbus_write_byte_data(mma8452_i2c_client, MMA8452_CTRL_REG1, 0x00);
	mdelay(10);
	(void)i2c_smbus_write_byte_data(mma8452_i2c_client, MMA8452_XYZ_DATA_CFG, temp8u2);
	mdelay(10);
	(void)i2c_smbus_write_byte_data(mma8452_i2c_client, MMA8452_CTRL_REG1, temp8u1);
	mdelay(10);

	MMA8452_Read_XYZ12(&x1, &y1, &z1);
	return i;               //1: Calibration succeeded; 0: Calibration failed.
}

/********************************************************************************/
//	ret = i2c_smbus_read_i2c_block_data(mma8452_i2c_client,MMA8452_OUT_X_MSB, 7, tmp_data);
unsigned char MMA8452_Read_XYZ8(signed char *x, signed char *y, signed char *z)
{
	//if(!FSL_MMA_IIC_ReadDataBlock(MMA8452_CTRL_REG1, &Rx_Buffer[0], 1)) return 0;     //IIC failed
	if (!i2c_smbus_read_i2c_block_data(mma8452_i2c_client, MMA8452_CTRL_REG1, 1, &Rx_Buffer[0])) return 0;   //IIC failed

	if (Rx_Buffer[0] & 0x02) {
		//if(!FSL_MMA_IIC_ReadDataBlock(MMA8452_X_MSB, &Rx_Buffer[0], 3)) return 0;     //IIC failed
		if (!i2c_smbus_read_i2c_block_data(mma8452_i2c_client, MMA8452_OUT_X_MSB, 3, &Rx_Buffer[0])) return 0;   //IIC failed
		*x = (signed char)Rx_Buffer[0];
		*y = (signed char)Rx_Buffer[1];
		*z = (signed char)Rx_Buffer[2];
	} else {
		//if(!FSL_MMA_IIC_ReadDataBlock(MMA8452_X_MSB, &Rx_Buffer[0], 6)) return 0;     //IIC failed
		if (!i2c_smbus_read_i2c_block_data(mma8452_i2c_client, MMA8452_OUT_X_MSB, 6, &Rx_Buffer[0])) return 0;   //IIC failed
		*x = (signed char)Rx_Buffer[0];
		*y = (signed char)Rx_Buffer[2];
		*z = (signed char)Rx_Buffer[4];
	}
	return 1;
}


unsigned char MMA8452_Read_XYZ12(signed short *x, signed short *y, signed short *z)
{

	//if(!FSL_MMA_IIC_ReadDataBlock(MMA8452_CTRL_REG1, &Rx_Buffer[0], 1)) return 0;     //IIC failed
	if (!i2c_smbus_read_i2c_block_data(mma8452_i2c_client, MMA8452_CTRL_REG1, 1, &Rx_Buffer[0])) return 0;   //IIC failed
	if (!(Rx_Buffer[0] & 0x02)) {
		//if(!FSL_MMA_IIC_ReadDataBlock(MMA8452_X_MSB, &Rx_Buffer[0], 6)) return 0;    //IIC failed
		if (!i2c_smbus_read_i2c_block_data(mma8452_i2c_client, MMA8452_OUT_X_MSB, 6, &Rx_Buffer[0])) return 0;  //IIC failed
		*x = ((((signed short)Rx_Buffer[0]) << 4) & 0x0FF0) | ((((signed short)Rx_Buffer[1]) >> 4) & 0x000F);
		if (*x & 0x0800) *x |= 0xF000;
		*y = ((((signed short)Rx_Buffer[2]) << 4) & 0x0FF0) | ((((signed short)Rx_Buffer[3]) >> 4) & 0x000F);
		if (*y & 0x0800) *y |= 0xF000;
		*z = ((((signed short)Rx_Buffer[4]) << 4) & 0x0FF0) | ((((signed short)Rx_Buffer[5]) >> 4) & 0x000F);
		if (*z & 0x0800) *z |= 0xF000;
	} else {
		//if(!FSL_MMA_IIC_ReadDataBlock(MMA8452_X_MSB, &Rx_Buffer[0], 3)) return 0;    //IIC failed
		if (!i2c_smbus_read_i2c_block_data(mma8452_i2c_client, MMA8452_OUT_X_MSB, 3, &Rx_Buffer[0])) return 0;
		*x = ((((signed short)Rx_Buffer[0]) << 4) & 0x0FF0);
		*y = ((((signed short)Rx_Buffer[1]) << 4) & 0x0FF0);
		*z = ((((signed short)Rx_Buffer[2]) << 4) & 0x0FF0);
		//if(!FSL_MMA_IIC_ReadDataBlock(MMA8452_X_LSB, &Rx_Buffer[0], 1)) return 0;    //IIC failed
		if (!i2c_smbus_read_i2c_block_data(mma8452_i2c_client, MMA8452_OUT_X_LSB, 1, &Rx_Buffer[0])) return 0;
		*x |= (((signed short)Rx_Buffer[0]) >> 4) & 0x000F;
		if (*x & 0x0800) *x |= 0xF000;
		//if(!FSL_MMA_IIC_ReadDataBlock(MMA8452_Y_LSB, &Rx_Buffer[0], 1)) return 0;    //IIC failed
		if (!i2c_smbus_read_i2c_block_data(mma8452_i2c_client, MMA8452_OUT_Y_LSB, 1, &Rx_Buffer[0])) return 0;
		*y |= (((signed short)Rx_Buffer[0]) >> 4) & 0x000F;
		if (*y & 0x0800) *y |= 0xF000;
		//if(!FSL_MMA_IIC_ReadDataBlock(MMA8452_Z_LSB, &Rx_Buffer[0], 1)) return 0;    //IIC failed
		if (!i2c_smbus_read_i2c_block_data(mma8452_i2c_client, MMA8452_OUT_Z_LSB, 1, &Rx_Buffer[0])) return 0;
		*z |= (((signed short)Rx_Buffer[0]) >> 4) & 0x000F;
		if (*z & 0x0800) *z |= 0xF000;
	}
	return 1;
}

unsigned char MMA8452_Data_Is_Ready(void)
{
	if (!i2c_smbus_read_i2c_block_data(mma8452_i2c_client, MMA8452_STATUS, 1, &Rx_Buffer[0])) return 0;
// if(!FSL_MMA_IIC_ReadDataBlock(MMA8452_STATUS1, &Rx_Buffer[0], 1)) return 0;    //IIC failed
	if ((Rx_Buffer[0] & 0x07) == 0x07) return 1;
	return 0;
}


unsigned char MMA8452_Sample_n_Times(unsigned char n, signed short *pX, signed short *pY, signed short *pZ)
{
	unsigned char i;
	signed short TmpX, TmpY, TmpZ;
	//unsigned short temp16s1;

	*pX = 0;
	*pY = 0;
	*pZ = 0;

	//for (i=0, temp16s1=0;i<n;i++)
	for (i = 0; i < n; i++) {
		while (!MMA8452_Data_Is_Ready());/*
      {
        if((++temp16s1>= 0xF000)) return 0;   //Reading data failed
      }*/
		if (!MMA8452_Read_XYZ12(&TmpX, &TmpY, &TmpZ)) return 0;  //Reading data failed
		*pX += TmpX;
		*pY += TmpY;
		*pZ += TmpZ;
	}
	*pX /= n;
	*pY /= n;
	*pZ /= n;

	return 1;
}
