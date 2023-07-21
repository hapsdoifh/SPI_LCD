/* 
 * File:   lcd_omcj4x8c_16.h
 * Author: wyy
 *
 * Created on 2016年2月19日, 下午3:28
 */

#ifndef LCD_OMCJ4X8C_16_H
#define	LCD_OMCJ4X8C_16_H

#include "lcd_spi.h"

#ifdef	__cplusplus
extern "C" {
#endif

#define LCD_CTRL_BIT_SYNC   0xF8    //1111 1000
#define LCD_CTRL_BIT_READ   0x04    //0000 0100
#define LCD_CTRL_BIT_DATA   0x02    //0000 0010



/*------------------??�???-----------------*/
void Lcd_omcj4x8c_init_lcd(int fd);
/*---------------?�示�?�???�?�?----------------*/
void Lcd_omcj4x8c_chn_disp(int fd, uchar code *chn);
/*-----------�???�??�示�?�???�?�?------------*/
void Lcd_omcj4x8c_chn_disp1(int fd, uchar code *chn);
/*----------------?�示?�形-----------------*/
void Lcd_omcj4x8c_img_disp(int fd, uchar code *img);
/*-------------�???�??�示?�形--------------*/
void Lcd_omcj4x8c_img_disp1(int fd, uchar code *img);
/*--------------?�示?��??----------------*/
void Lcd_omcj4x8c_lat_disp(int fd, uchar data1, uchar data2);
//�?data1=0xff,data2=0xff??,??x0,y0�????��?�示16xl*yl.
void Lcd_omcj4x8c_con_disp(int fd, uchar data1, uchar data2, uchar x0, uchar y0, uchar xl, uchar yl);
/*--------------�?DDRAM------------------*/
void Lcd_omcj4x8c_clrram(int fd);

#ifdef	__cplusplus
}
#endif

#endif	/* LCD_OMCJ4X8C_16_H */

