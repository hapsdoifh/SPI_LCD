#include "led.h"
#include "delay.h"
#include "key.h"
#include "sys.h"
#include "lcd.h"
#include "usart.h"	 
#include "24cxx.h"
#include "flash.h"
#include "touch.h"
#include "flash.h"
#include "spi.h"	  

u8 state=0;
void beepms(u16 va);
void xianshi(void);//显示信息  
void refshow(void);//刷新显示

void Load_Drow_Dialog(void)
{
	LCD_Clear(WHITE);//清屏   
 	POINT_COLOR=BLUE;//设置字体为蓝色 
	LCD_ShowString(lcddev.width-24,0,200,16,16,"RST");//显示清屏区域
  	POINT_COLOR=RED;//设置画笔蓝色 
}
void xianshi()//显示信息
{ 
	BACK_COLOR=WHITE;
	POINT_COLOR=RED;   
	//显示32*32汉字
	showhanzi32(0,0,0);	 //淘
	showhanzi32(40,0,1);	 //晶
	showhanzi32(80,0,2);    //驰
	//显示16*16汉字
	showhanzi16(0,35,0);	  //专
	showhanzi16(20,35,1);	  //注
	showhanzi16(40,35,2);	  //显
	showhanzi16(60,35,3);	  //示
	showhanzi16(80,35,4);	  //方
	showhanzi16(100,35,5);	  //案	   
	LCD_ShowString(0,55,200,16,16,"2.4 TFT SPI 240*320");
}
void showqq()
{ 
	u16 x,y; 
	x=0;
	y=75;
	while(y<lcddev.height-39)
	{
		x=0;
		while(x<lcddev.width-39)
		{
			showimage(x,y);	
			x+=40;
		}
		y+=40;
	 }	  
}
void refshow(void)	 //刷新显示
{
	switch(state)
	{
		case 0:
		LCD_Clear(WHITE);
	    xianshi();
    	showqq();
		break;
		case 1:
		LCD_Clear(BLACK);	
		break;
		case 2:
		LCD_Clear(RED);
		break;
		case 3:
		LCD_Clear(GREEN);
		break;
		case 4:
		LCD_Clear(BLUE);
		break;
	}	
}
void jiance(u8 key) //检测按键
{
	if(key==1)	//KEY_RIGHT按下,则执行校准程序
	{
		LCD_Clear(WHITE);//清屏
	    TP_Adjust();  //屏幕校准 
		TP_Save_Adjdata();	 
		Load_Drow_Dialog();
	}
	if(key==2)
	{
		state++; 	
		if(state==5)
		{
			state=0; 
		}
		refshow();//刷新显示
	}

} 
void beepms(u16 va)
{
	beep=1;
	delay_ms(va);
    beep=0;
}				
int main(void)
 {	 
	u16 i=0;	 
	u8 key=0;	
	delay_init();	    	 //延时函数初始化	  
	NVIC_Configuration(); 	 //设置NVIC中断分组2:2位抢占优先级，2位响应优先级
	uart_init(9600);	 	//串口初始化为9600
 	LED_Init();			     //LED端口初始化
    SPI_Flash_Init();//初始化Flash;
	KEY_Init();	 //按键初始化	   	 
	 	
	SPI1_Init();	//SPI1初始化
	LCD_Init();	
	tp_dev.init();//触摸初始化	 			
 	POINT_COLOR=RED;//设置字体为红色 
	xianshi();	   //显示信息
	showqq();	   //显示QQ
	while(1)
	{ 
	   key=KEY_Scan(0);//扫描按键		   
	   tp_dev.scan(0); //扫描触摸
			
		if(tp_dev.sta&TP_PRES_DOWN)			//触摸屏被按下
		{ 
			if(state!=1)
			{
				LCD_Clear(WHITE);
			} 		
			while(key==0)
			{			
				key=KEY_Scan(0);//扫描按键
		    	tp_dev.scan(0);	
				if(tp_dev.sta&TP_PRES_DOWN)			//触摸屏被按下
				{			
				 	if(tp_dev.x<lcddev.width&&tp_dev.y<lcddev.height)
					{					
						if(tp_dev.x>(lcddev.width-24)&&tp_dev.y<16)Load_Drow_Dialog();//清除
						else TP_Draw_Big_Point(tp_dev.x,tp_dev.y,RED);		//画图	  
						 		   
					}
				}						
			} 			
		}
     

	    jiance(key);//检测按键  

		i++;
		if(i==65000)
		{
			i=0;
			LED0=!LED0;
		}
	}
}


