#include "oled.h"
#include "oledfont.h"

#define OLED_WR_address 0x78

extern I2C_HandleTypeDef hi2c1;
//OLED的显存
//存放格式如下.
//[0]0 1 2 3 ... 127	
//[1]0 1 2 3 ... 127	
//[2]0 1 2 3 ... 127	
//[3]0 1 2 3 ... 127	
//[4]0 1 2 3 ... 127	
//[5]0 1 2 3 ... 127	
//[6]0 1 2 3 ... 127	
//[7]0 1 2 3 ... 127 			   

/**********************************************
// IIC Write Command
**********************************************/
void Write_IIC_Command(unsigned char IIC_Command)
{
	uint8_t tmp[] = {0x00,IIC_Command};//寄存器地址和命令
	HAL_I2C_Master_Transmit(&hi2c1, OLED_WR_address, tmp, 2,100);
	HAL_Delay(0);//只需要点点延时而已
}

/**********************************************
// IIC Write Data
**********************************************/
void Write_IIC_Data(unsigned char IIC_Data)
{

	uint8_t tmp[] = {0x40, IIC_Data};//数据地址和数据
	HAL_I2C_Master_Transmit(&hi2c1, OLED_WR_address, tmp, 2, 20);
}

void OLED_WR_Byte(uint8_t dat, uint8_t cmd)
{
	if(cmd)
	{
		Write_IIC_Data(dat);
	}else{
   Write_IIC_Command(dat);	
	}
}

//坐标设置
void OLED_Set_Pos(unsigned char x, unsigned char y) 
{ 	
	OLED_WR_Byte(0xb0+y, OLED_CMD);
	OLED_WR_Byte(((x&0xf0)>>4)|0x10, OLED_CMD);
	OLED_WR_Byte((x&0x0f), OLED_CMD); 
}   	  

//开启OLED显示    
void OLED_Display_On(void)
{
	OLED_WR_Byte(0X8D, OLED_CMD);  //SET DCDC命令
	OLED_WR_Byte(0X14, OLED_CMD);  //DCDC ON
	OLED_WR_Byte(0XAF, OLED_CMD);  //DISPLAY ON
}

//关闭OLED显示     
void OLED_Display_Off(void)
{
	OLED_WR_Byte(0X8D, OLED_CMD);  //SET DCDC命令
	OLED_WR_Byte(0X10, OLED_CMD);  //DCDC OFF
	OLED_WR_Byte(0XAE, OLED_CMD);  //DISPLAY OFF
}		

//清屏函数,清完屏,整个屏幕是黑色的!和没点亮一样!!!	  
void OLED_Clear(void)  
{  
	uint8_t i, n;		    
	for(i=0; i<8; i++)  
	{  
		OLED_WR_Byte (0xb0+i, OLED_CMD);    //设置页地址（0~7）
		OLED_WR_Byte (0x00, OLED_CMD);      //设置显示位置—列低地址
		OLED_WR_Byte (0x10, OLED_CMD);      //设置显示位置—列高地址   
		for(n=0; n<128; n++)
			OLED_WR_Byte(0, OLED_DATA); 
	} //更新显示
}

void OLED_On(void)  
{  
	uint8_t i, n;		    
	for(i=0; i<8; i++)  
	{  
		OLED_WR_Byte (0xb0+i, OLED_CMD);    //设置页地址（0~7）
		OLED_WR_Byte (0x00, OLED_CMD);      //设置显示位置—列低地址
		OLED_WR_Byte (0x10, OLED_CMD);      //设置显示位置—列高地址   
		for(n=0; n<128; n++)
			OLED_WR_Byte(1, OLED_DATA); 
	} //更新显示
}


//在指定位置显示一个字符,包括部分字符
//x:0~127
//y:0 2 4 6，分别对应第1，2，3，4行
//size:选择字体大小 16或者非16 
void OLED_ShowChar(uint8_t x, uint8_t y, uint8_t chr, uint8_t Char_Size)
{      	
	unsigned char c=0, i=0;	
	c=chr - ' ';//得到偏移后的值			
	if(x > Max_Column - 1){x = 0;y = y+2;}
	if(Char_Size == 16)
	{
		OLED_Set_Pos(x, y);	
		for(i=0; i<8; i++)
			OLED_WR_Byte(F8X16[c*16+i], OLED_DATA);
		OLED_Set_Pos(x , y+1);
		for(i=0; i<8; i++)
			OLED_WR_Byte(F8X16[c*16+i+8], OLED_DATA);
	}else{	
		OLED_Set_Pos(x, y);
		for(i=0; i<6; i++)
			OLED_WR_Byte(F6x8[c][i], OLED_DATA);		
	}
}

void OLED_Show16X32Char(uint8_t x,uint8_t y,uint8_t chr)
{
	unsigned char c=0,i=0;
	c=chr-' ';//得到偏移；测试是显示几个数字
	OLED_Set_Pos(x,y);
	for(i=0;i<16;i++)
	OLED_WR_Byte(F16X32[c*64+i],OLED_DATA);
	OLED_Set_Pos(x,y+1);
	for(i=0;i<16;i++)
	OLED_WR_Byte(F16X32[c*64+i+16],OLED_DATA);
	OLED_Set_Pos(x,y+2);
	for(i=0;i<16;i++)
	OLED_WR_Byte(F16X32[c*64+i+32],OLED_DATA);
	OLED_Set_Pos(x,y+3);
	for(i=0;i<16;i++)
	OLED_WR_Byte(F16X32[c*64+i+48],OLED_DATA);
}

void OLED_ShowString16X32(int x,int y,uint8_t *chr)
{
	unsigned char j=0;
	while (chr[j]!='\0')
	{
			OLED_Show16X32Char(x,y,chr[j]);
			x+=16;
		if(x>120){x=0;y+=2;}
			j++;
	}
}


//显示一个字符串
void OLED_ShowString(uint8_t x, uint8_t y, uint8_t *chr, uint8_t Char_Size)
{
	unsigned char j=0;
	while (chr[j] != '\0')
	{		
		OLED_ShowChar(x, y, chr[j], Char_Size);
		x += 8;
		if(x > 120)
		{
			x = 0;
			y += 2;
		}
			j++;
	}
}

//显示汉字
void OLED_ShowCHinese(uint8_t x, uint8_t y, uint8_t no)
{
	uint8_t t,adder=0;
	OLED_Set_Pos(x,y);	
	for(t=0;t<16;t++)
	{
			OLED_WR_Byte(Hzk[2*no][t],OLED_DATA);
			adder+=1;
	}	
	OLED_Set_Pos(x,y+1);	
	for(t=0;t<16;t++)
	{	
		OLED_WR_Byte(Hzk[2*no+1][t],OLED_DATA);
		adder+=1;
	}		
	/*uint8_t t, adder=0;
	OLED_Set_Pos(x, y);	
  for(t=0; t<16; t++)
	{
		OLED_WR_Byte(Hzk[no][t], OLED_DATA);
		adder+=1;
  }	
	OLED_Set_Pos(x, y+1);	
  for(t=16; t<32; t++)
	{	
		OLED_WR_Byte(Hzk[no][t], OLED_DATA);
		adder+=1;
  }*/					
}

/***********功能描述：显示显示BMP图片128×64起始点坐标(x,y),x的范围0～127，y为页的范围0～7*****************/
void OLED_DrawBMP(unsigned char x0, unsigned char y0, unsigned char x1, unsigned char y1, unsigned char BMP[])
{ 	
 unsigned int j=0;
 unsigned char x,y;
	
	for(y=y0; y<y1; y++)
	{
		OLED_Set_Pos(x0,y);
    for(x=x0; x<x1; x++)
	  {      
			OLED_WR_Byte(BMP[j++], OLED_DATA);	    	
	  }
	}
} 

//初始化SSD1306					    
void OLED_Init(void)
{ 
	OLED_WR_Byte(0xAE, OLED_CMD);//--display off
	OLED_WR_Byte(0x00, OLED_CMD);//---set low column address
	OLED_WR_Byte(0x10, OLED_CMD);//---set high column address
	OLED_WR_Byte(0x40, OLED_CMD);//--set start line address  
	OLED_WR_Byte(0xB0, OLED_CMD);//--set page address
	OLED_WR_Byte(0x81, OLED_CMD); // contract control
	OLED_WR_Byte(0xFF, OLED_CMD);//--128   
	OLED_WR_Byte(0xA1, OLED_CMD);//set segment remap 
	OLED_WR_Byte(0xA6, OLED_CMD);//--normal / reverse
	OLED_WR_Byte(0xA8, OLED_CMD);//--set multiplex ratio(1 to 64)
	OLED_WR_Byte(0x3F, OLED_CMD);//--1/32 duty
	OLED_WR_Byte(0xC8, OLED_CMD);//Com scan direction
	OLED_WR_Byte(0xD3, OLED_CMD);//-set display offset
	OLED_WR_Byte(0x00, OLED_CMD);//
	
	OLED_WR_Byte(0xD5, OLED_CMD);//set osc division
	OLED_WR_Byte(0x80, OLED_CMD);//
	
	OLED_WR_Byte(0xD8, OLED_CMD);//set area color mode off
	OLED_WR_Byte(0x05, OLED_CMD);//
	
	OLED_WR_Byte(0xD9, OLED_CMD);//Set Pre-Charge Period
	OLED_WR_Byte(0xF1, OLED_CMD);//
	
	OLED_WR_Byte(0xDA, OLED_CMD);//set com pin configuartion
	OLED_WR_Byte(0x12, OLED_CMD);//
	
	OLED_WR_Byte(0xDB, OLED_CMD);//set Vcomh
	OLED_WR_Byte(0x30, OLED_CMD);//
	
	OLED_WR_Byte(0x8D, OLED_CMD);//set charge pump enable
	OLED_WR_Byte(0x14, OLED_CMD);//
	
	OLED_WR_Byte(0xAF, OLED_CMD);//--turn on oled panel
}  


























