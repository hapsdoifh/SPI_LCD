/* 
 * File:   lcd_ili9341.h
 * Author: wyy
 *
 * Created on 2016年2月19日, 下午3:40
 */

#ifndef LCD_ILI9341_H
#define	LCD_ILI9341_H

#include "lcd_spi.h"
#ifdef	__cplusplus
extern "C" {
#endif


//////////////////////////////////////////////////////////////////////////
/* 
代码测试环境:单片机STC12LE5A60S2,晶振33M  单片机工作电压3.3V
程序默认IO连接方式：
控制线：RS-P3^5;    WR-P3^6;   RD-P3^7;   CS-P1^0;   REST-P1^2;
数据线: DB0-DB7依次连接P0^0-P0^7;  DB8-DB15依次连接P2^0-P2^7;（8位模式下DB0-DB7可以不连接）
触摸功能连接方式：(不使用触摸可不连接)
T_CLK-P1^7;  T_CS-P1^4;  T_DIN-P3^0;  T_OUT-P3^1;  T_IRQ-P3^4;
*/	

#define CMD_SOFTWARE_RST    0x01
#define CMD_PWR_CONTRL_A    0xCB
#define CMD_PWR_CONTRL_B    0xCF
#define CMD_TIM_CONTRL_A    0xE8
#define CMD_TIM_CONTRL_B    0xEA
#define CMD_PWR_SEQ_CT_A    0xED
#define CMD_PMP_RATIO_CT    0xF7
#define CMD_PWR_CONTRL_1    0xC0
#define CMD_PWR_CONTRL_2    0xC1
#define CMD_VCM_CONTRL_1    0xC5
#define CMD_VCM_CONTRL_2    0xC7
#define CMD_MEM_ACCESS_C    0x36
#define CMD_PIXEL_FORMAT    0x3A
#define CMD_FRAME_RATE_C    0xB1
#define CMD_DISPLAY_FUNC    0xB6
#define CMD_ENABL_3GAMMA    0xF2
#define CMD_GAMMA_CURVES    0x26
#define CMD_POS_GAMMA_CR    0xE0
#define CMD_NEG_GAMMA_CR    0xE1
#define CMD_SLEEP_MODE_I    0x10
#define CMD_SLEEP_MODE_O    0x11
#define CMD_SET_DISP__ON    0x29
#define CMD_WRITE_MEMORY    0x2C

//定义LCD的尺寸	
#define LCD_ILI9341_WIDE        320
#define LCD_ILI9341_HIGH        240

#define LCD_DISPLAY_LIMIT_TEXT  240
#define LCD_DISPLAY_LIMIT_IMG   254
    
extern  uint16 lcd_ili9341_BACK_COLOR, lcd_ili9341_POINT_COLOR;   //背景色，画笔色

typedef struct __CHN_Word_Template_32 {
    unsigned char m_lattice[32][4];
} ChineseLattice;

typedef struct __CHN_Words_32 {
    int             m_nWord;
    ChineseLattice* m_words;
} ChineseWords;

typedef struct  __LCD_IMAGE {
    int m_width;
    int m_hight;
    unsigned char* m_image;
} LCD_Image;

void Lcd_ili9341_Init(int fd); 
void Lcd_ili9341_Clear(int fd,  uint16 Color);

void Lcd_ili9341_DrawPoint(int fd,  uint16 x,uint16 y);//画点
void Lcd_ili9341_DrawPoint_big(int fd,  uint16 x,uint16 y);//画一个大点
uint16  Lcd_ili9341_ReadPoint(int fd,  uint16 x,uint16 y); //读点
void Lcd_ili9341_Draw_Circle(int fd,  uint16 x0,uint16 y0,uchar r);
void Lcd_ili9341_DrawLine(int fd,  uint16 x1, uint16 y1, uint16 x2, uint16 y2);
void Lcd_ili9341_DrawRectangle(int fd,  uint16 x1, uint16 y1, uint16 x2, uint16 y2);		   
void Lcd_ili9341_Fill(int fd,  uint16 xsta,uint16 ysta,uint16 xend,uint16 yend,uint16 color);
void Lcd_ili9341_ShowChar(int fd, uint16 x, uint16 y, uchar num, uchar hight,uchar width,uchar mode);//显示一个字符
void Lcd_ili9341_ShowNum(int fd, uint16 x, uint16 y, uint num, uchar len);//显示数字
void Lcd_ili9341_Show2Num(int fd, uint16 x, uint16 y, uint16 num, uchar len);//显示2个数字
void Lcd_ili9341_ShowString(int fd, uint16 x, uint16 y, uint16 hight, uint16 width, const uchar *p);//显示一个字符串,16字体
void Lcd_ili9341_ShowChinese(int fd, unsigned int x, unsigned int y, ChineseWords *words, bool draw_bk);
void Lcd_ili9341_ShowIcon(int fd, uint16 left, uint16 top, uint16 right, uint16 bottom, unsigned char *image);
void Lcd_ili9341_ShowIcons(int fd, int r0, int r1, int c0, int c1, unsigned char *image);
void Lcd_ili9341_ClearIcons(int fd, int r0, int r1, int c0, int c1, uint16 color);



//画笔颜色
#define LCD_ILI9341_WHITE       0xFFFF
#define LCD_ILI9341_BLACK       0x0000	  
#define LCD_ILI9341_BLUE        0x001F  
#define LCD_ILI9341_BRED        0XF81F
#define LCD_ILI9341_GRED        0XFFE0
#define LCD_ILI9341_GBLUE       0X07FF
#define LCD_ILI9341_RED         0xF800
#define LCD_ILI9341_MAGENTA     0xF81F
#define LCD_ILI9341_GREEN       0x07E0
#define LCD_ILI9341_CYAN        0x7FFF
#define LCD_ILI9341_YELLOW      0xFFE0
#define LCD_ILI9341_BROWN       0XBC40 //棕色
#define LCD_ILI9341_BRRED       0XFC07 //棕红色
#define LCD_ILI9341_GRAY        0X8430 //灰色
//GUI颜色

#define LCD_ILI9341_DARKBLUE    0X01CF	//深蓝色
#define LCD_ILI9341_LIGHTBLUE   0X7D7C	//浅蓝色  
#define LCD_ILI9341_GRAYBLUE    0X5458 //灰蓝色
//以上三色为PANEL的颜色 
 
#define LCD_ILI9341_LIGHTGREEN  0X841F //浅绿色
#define LCD_ILI9341_LGRAY       0XC618 //浅灰色(PANNEL),窗体背景色

#define LCD_ILI9341_LGRAYBLUE   0XA651 //浅灰蓝色(中间层颜色)
#define LCD_ILI9341_LBBLUE      0X2B12 //浅棕蓝色(选择条目的反色)

#ifdef	__cplusplus
}
#endif

#endif	/* LCD_ILI9341_H */

