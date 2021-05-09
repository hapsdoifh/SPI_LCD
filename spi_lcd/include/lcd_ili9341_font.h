#ifndef __FONT_H
#define __FONT_H		

#include "lcd_spi.h"
#include "lcd_ili9341.h"

extern  unsigned char code lcd_ili9341_image[];
extern unsigned char lcd_ili9341_asc2_1608[1520];
extern unsigned char lcd_ili9341_asc2_3216[6080];
//additional Chinese words
//extern ChineseWords lcd_brandname_a; //Beijing 6Wilink Inc. (in Chinese)
//extern ChineseWords lcd_brandname_b; //Beijing 6Wilink Inc. (in Chinese)
//extern ChineseWords lcd_brandname_c; //Beijing 6Wilink Inc. (in Chinese)
//extern ChineseWords lcd_brandname_d; //Beijing 6Wilink Inc. (in Chinese)
//extern ChineseWords lcd_brandname_eng; //Beijing 6Wilink Inc. (in Chinese)
extern ChineseWords lcd_author_name;
extern LCD_Image    lcd_image_lcd_ili9341_image_logo;

#endif








