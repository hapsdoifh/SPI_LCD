/* 
 * File:   touch_ads7843.h
 * Author: wyy
 *
 * Created on 2016��2��19��, ����5:54
 */

#ifndef TOUCH_ADS7843_H
#define	TOUCH_ADS7843_H

#include "lcd_ili9341.h"
#ifdef	__cplusplus
extern "C" {
#endif

/* touch panel interface define */
#define  CMD_RDX  0xD0	 //����IC�����������
#define  CMD_RDY  0x90
//IO����
//sbit DCLK	   =    P1^6;   
//sbit CS        =    P1^4;
//sbit DIN       =    P3^0;	
//sbit DOUT      =    P3^1;																						   
//sbit Penirq    =    P3^4;   //��ⴥ������Ӧ�ź�
#define SPI_DEBUG_CH2_BUSY      0x00000040

struct ADS7843_TP_PIX_
{
	uint16 x;
	uint16 y;
};
struct ADS7843_TP_PIX32_
{
	uint x;
	uint y;
};
extern struct ADS7843_TP_PIX_  ads7843_tp_pixad,ads7843_tp_pixlcd;	 //��ǰ���������ADֵ,ǰ�������������ֵ   
extern uint16 ads7843_vx,ads7843_vy;  //�������ӣ���ֵ����1000֮���ʾ���ٸ�ADֵ����һ�����ص�
extern uint16 ads7843_chx,ads7843_chy;//Ĭ�����ص�����Ϊ0ʱ��AD��ʼֵ

uint16 Touch_ads7843_ReadByte(int fd_touch);         //SPI ������
uchar Touch_ads7843_tpstate(int fd_touch);
void Touch_ads7843_spistar(int fd_touch);  
void Touch_ads7843_Adjust(int fd_lcd, int fd_touch);
void Touch_ads7843_point(int fd_lcd, int fd_touch); //��ͼ����

unsigned char Touch_ads7843_wait_press(int fd_tch, uint16 *x, uint16 *y);
unsigned char Touch_ads7843_wait_click(int fd_tch, uint16 *x, uint16 *y);

#ifdef	__cplusplus
}
#endif

#endif	/* TOUCH_ADS7843_H */

