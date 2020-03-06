#include "MPU6050.h"
#include "stdio.h"
/*******?????????*********/
int16_t offsetAccelX = 0;
int16_t offsetAccelY = 0;
int16_t offsetAccelZ = 0;

int16_t offsetGyroX = 3;
int16_t offsetGyroY = 1;
int16_t offsetGyroZ = 0;

extern I2C_HandleTypeDef hi2c1;

uint16_t AccelX_Vlue,AccelY_Vlue,AccelZ_Vlue;
uint16_t GyroX_Vlue,GyroY_Vlue,GyroZ_Vlue;
//**************************************
//I2C写字节函数
//**************************************
void Single_WriteI2C(uint8_t REG_Address,uint8_t REG_data)
{
    uint8_t rxData[2] = {REG_Address,REG_data};
    while(HAL_I2C_Master_Transmit(&hi2c1,SlaveAddress,rxData,2,5000) != HAL_OK)
    {
        if(HAL_I2C_GetError(&hi2c1) != HAL_I2C_ERROR_AF)
        {}
    }
}
//**************************************
//I2C读字节函数
//**************************************
uint8_t Single_ReadI2C(uint8_t REG_Address)
{
    uint8_t REG_data;
	while(HAL_I2C_Master_Transmit(&hi2c1,SlaveAddress,&REG_Address,1,5000) != HAL_OK)
    {
        if(HAL_I2C_GetError(&hi2c1) != HAL_I2C_ERROR_AF)
        {}
    }
    
    if(HAL_I2C_Master_Receive(&hi2c1,SlaveAddress+1,&REG_data,1,5000) != HAL_OK)
    {
        if(HAL_I2C_GetError(&hi2c1) != HAL_I2C_ERROR_AF)
        {}
    }
    return REG_data;
}
//**************************************
//???MPU6050
//**************************************
void InitMPU6050(void)
{
    Single_WriteI2C(PWR_MGMT_1, 0x00);    

    Single_WriteI2C(SMPLRT_DIV, 0x07);

    Single_WriteI2C(CONFIG, 0x06);

    Single_WriteI2C(GYRO_CONFIG, 0x18);

    Single_WriteI2C(ACCEL_CONFIG, 0x01);
}
//**************************************
//????
//**************************************
int16_t GetMPUOutValue(uint8_t REG_Address)
{
    int16_t result;
    uint8_t H,L;
    H=Single_ReadI2C(REG_Address);
    L=Single_ReadI2C(REG_Address+1);
    result = (H<<8)+L;
    return result;   //????
}

//**************************************
//????????????
//**************************************
int16_t GetAccelValue(char axis)
{
    int16_t result = 0;
    switch(axis)
    {
        case 'x':
        case 'X':
        {
            result = GetMPUOutValue(ACCEL_XOUT_H);
        }
        break;
        case 'y':
        case 'Y':
        {
            result = GetMPUOutValue(ACCEL_YOUT_H);
        }
        break;
        case 'z':
        case 'Z':
        {
            result = GetMPUOutValue(ACCEL_ZOUT_H);
        }
        break;
    }
    return result;
}

//**************************************
//???????????
//**************************************
int16_t GetGyroValue(char axis)
{
    int16_t result = 0;
    switch(axis)
    {
        case 'x':
        case 'X':
        {
            result = GetMPUOutValue(GYRO_XOUT_H);
        }
        break;
        case 'y':
        case 'Y':
        {
            result = GetMPUOutValue(GYRO_YOUT_H);
        }
        break;
        case 'z':
        case 'Z':
        {
            result = GetMPUOutValue(GYRO_ZOUT_H);
        }
        break;
    }
    return result;
}

uint8_t GyroX_Flag,GyroY_Flag,GyroZ_Flag;
/***********角速度处理函数****************/
void GYROData_Process(void)
{  
	GyroX_Vlue=GetGyroValue('x');
	GyroY_Vlue=GetGyroValue('y');
	GyroZ_Vlue=GetGyroValue('z');	
	float tmp;
	if((GyroX_Vlue&0x8000)==0x8000)
	{	
		GyroX_Flag=1;
		GyroX_Vlue--;
		GyroX_Vlue=~GyroX_Vlue;
		tmp=GyroX_Vlue;
		tmp=tmp/32768*2000;
		GyroX_Vlue=(unsigned short)(tmp);
	}
	else
	{
		GyroX_Flag=0;
		tmp=GyroX_Vlue;
		tmp=tmp/32768*2000;
		GyroX_Vlue=(unsigned short)(tmp);
	}
		
	if((GyroY_Vlue&0x8000)==0x8000)
	{
    GyroY_Flag=1;		
		GyroY_Vlue--;
		GyroY_Vlue=~GyroY_Vlue;
		tmp=GyroY_Vlue;
		tmp=tmp/32768*2000;
		GyroY_Vlue=(unsigned short)(tmp);
	}
	else
	{
		GyroY_Flag=0;
		tmp=GyroY_Vlue;
		tmp=tmp/32768*2000;
		GyroY_Vlue=(unsigned short)(tmp);
	}
	if((GyroZ_Vlue&0x8000)==0x8000)
	{
    GyroZ_Flag=1;		
		GyroZ_Vlue--;
		GyroZ_Vlue=~GyroZ_Vlue;
		tmp=GyroZ_Vlue;
		tmp=tmp/32768*2000;
		GyroZ_Vlue=(unsigned short)(tmp);
	}
	else
	{
		GyroZ_Flag=0;
		tmp=GyroZ_Vlue;
		tmp=tmp/32768*2000;
		GyroZ_Vlue=(unsigned short)(tmp);
	}
}
/**********加速度处理函数****************/
void AccelData_Process(void)
{
		float tmp;
		AccelX_Vlue=GetAccelValue('x');
		AccelY_Vlue=GetAccelValue('y');
		AccelZ_Vlue=GetAccelValue('z');		
		if((AccelX_Vlue&0x8000)==0x8000)
		{	
			AccelX_Vlue--;
			AccelX_Vlue=~AccelX_Vlue;
		}
//		AccelX_Vlue*=598;
//		AccelX_Vlue=(uint16_t)(AccelX_Vlue/1000000);
		tmp=AccelX_Vlue;
		tmp=tmp*0.000598;
		AccelX_Vlue=(uint16_t)tmp;

		if((AccelY_Vlue&0x8000)==0x8000)
		{	
			AccelY_Vlue--;
			AccelY_Vlue=~AccelY_Vlue;
		}
//		AccelY_Vlue*=598;
//		AccelY_Vlue=(uint16_t)(AccelY_Vlue/1000000);
		tmp=AccelY_Vlue;
		tmp=tmp*0.000598;
		AccelY_Vlue=(uint16_t)tmp;

		if((AccelZ_Vlue&0x8000)==0x8000)
		{	
			AccelZ_Vlue--;
			AccelZ_Vlue=~AccelZ_Vlue;
		}
//		AccelZ_Vlue*=598;
//		AccelZ_Vlue=(uint16_t)(AccelZ_Vlue/1000000);
		tmp=AccelZ_Vlue;
		tmp=tmp*0.000598;
		AccelZ_Vlue=(uint16_t)tmp;
}
/****************手势判断函数*******************/
uint8_t Gesture_State;
void Gesture_Judge(void)
{
	if(GyroX_Vlue>50)//上下摆动
	{
		Gesture_State=1;
	}
  else if((GyroZ_Vlue>50)&&(AccelY_Vlue>8))//左右摆动
	{
		 Gesture_State=2;
	} 	  
  else if((GyroY_Vlue<5)&&(GyroZ_Vlue<5)&&(AccelX_Vlue<5)&&(AccelZ_Vlue<5))//垂直站立	
	{
		 Gesture_State=3;
	} 	   
	else
	{
		Gesture_State=0;
	}  
}
