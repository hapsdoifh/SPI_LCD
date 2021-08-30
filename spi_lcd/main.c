
/*
 * SPI testing utility (using spidev driver)
 *
 * Copyright (c) 2007  MontaVista Software, Inc.
 * Copyright (c) 2007  Anton Vorontsov <avorontsov@ru.mvista.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License.
 *
 * Cross-compile with cross-gcc -I/path/to/cross-kernel/include
 */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>

#include "lcd_omcj4x8c_16.h"
#include "lcd_omcj4x8c_font.h"
#include "lcd_ili9341.h"
#include "lcd_ili9341_font.h"
#include "touch_ads7843.h"

static const char *static_device_lcd = "/dev/spidev0.0";
static const char *static_device_touch = "/dev/spidev0.1";

#define LCD_TYPE_NONE       0x00
#define LCD_TYPE_OMCJ4X8C   0x01
#define LCD_TYPE_ILI9341    0x02

//#define LCD_TYPE    LCD_TYPE_OMCJ4X8C
#define LCD_TYPE    LCD_TYPE_ILI9341
//#define LCD_TYPE    LCD_TYPE_NONE

#if (LCD_TYPE & LCD_TYPE_OMCJ4X8C)
#include "lcd_omcj4x8c_16.h"

static void Lcd_omcj4x8c_demo(int fd_lcd) {
    Lcd_omcj4x8c_lat_disp(fd_lcd, 0x00, 0x00);
    Lcd_omcj4x8c_chn_disp(fd_lcd, tab1);
    usleep(3000000);
    Lcd_omcj4x8c_con_disp(fd_lcd, 0xff, 0xff, 0x8c, 0x80, 2, 16);
    usleep(3000000);
    Lcd_omcj4x8c_clrram(fd_lcd);
    Lcd_omcj4x8c_lat_disp(fd_lcd, 0xcc, 0xcc);
    usleep(3000000);
    Lcd_omcj4x8c_lat_disp(fd_lcd, 0x00, 0x00);
    Lcd_omcj4x8c_chn_disp1(fd_lcd, tab31);
    Lcd_omcj4x8c_img_disp1(fd_lcd, tab32);
    usleep(3000000);
    Lcd_omcj4x8c_clrram(fd_lcd);
    Lcd_omcj4x8c_lat_disp(fd_lcd, 0xff, 0x00);
    usleep(1000000);
    Lcd_omcj4x8c_img_disp(fd_lcd, tab5);
}
#elif (LCD_TYPE & LCD_TYPE_ILI9341)
#include "lcd_ili9341.h"

static int static_icon_row = 1;
static int static_icon_col;
//static void Lcd_ili9341_demo(int fd_lcd) //显示40*40图片
//{
//    //Lcd_ili9341_ShowHanzi(fd_lcd, 10, 0, 0); //晶
//    //Lcd_ili9341_ShowHanzi(fd_lcd, 45, 0, 1); //耀
//    Lcd_ili9341_ShowChinese(fd_lcd, 20, 100, &lcd_brandname_a,FALSE);
//    Lcd_ili9341_ShowString(fd_lcd, 10, 35, "2.4 TFT SPI 240*320");
//    Lcd_ili9341_ShowString(fd_lcd, 10, 55, "LCD_W:");
//    Lcd_ili9341_ShowNum(fd_lcd, 70, 55, LCD_ILI9341_WIDE, 3);
//    Lcd_ili9341_ShowString(fd_lcd, 110, 55, "LCD_H:");
//    Lcd_ili9341_ShowNum(fd_lcd, 160, 55, LCD_ILI9341_HIGH, 3);
////    usleep(1000000);    //显示信息
//    int row = static_icon_row % 6;
//    int col = static_icon_col % 6;
//    Lcd_ili9341_ClearIcons(fd_lcd, row + 2 , row + 3, col, col + 1,LCD_ILI9341_WHITE);
//    static_icon_row ++;
//    static_icon_col ++;
//    row = static_icon_row % 6;
//    col = static_icon_col % 6;
//    Lcd_ili9341_ShowIcons(fd_lcd, row + 2 , row + 3, col, col + 1,lcd_ili9341_image);
//}
#else
#define LCD_TYPE    LCD_TYPE_NONE
#endif  //LCD_TYPE

static void print_usage(const char *prog) {
    printf("Usage: %s [-DFsbdlHOLCg3]\n", prog);
    puts("  -D --device   device to use (default /dev/spidev1.1)\n"
            "  -F --txfile   text file contain Hex string to be sent\n"
            "  -s --speed    max speed (Hz)\n"
            "  -d --usleep    usleep (usec)\n"
            "  -b --bpw      bits per word \n"
            "  -l --loop     loopback\n"
            "  -H --cpha     clock phase\n"
            "  -O --cpol     clock polarity\n"
            "  -L --lsb      least significant bit first\n"
            "  -C --cs-high  chip select active high\n"
            "  -3 --3wire    SI/SO signals shared\n");
    exit(1);
}

static void parse_opts(int argc, char *argv[]) {
    while (1) {
        static const struct option lopts[] = {
            { "device", 1, 0, 'D'},
            { "txfile", 1, 0, 'F'},
            { "speed", 1, 0, 's'},
            { "usleep", 1, 0, 'd'},
            { "bpw", 1, 0, 'b'},
            { "loop", 0, 0, 'l'},
            { "cpha", 0, 0, 'H'},
            { "cpol", 0, 0, 'O'},
            { "lsb", 0, 0, 'L'},
            { "cs-high", 0, 0, 'C'},
            { "3wire", 0, 0, '3'},
            { "no-cs", 0, 0, 'N'},
            { "ready", 0, 0, 'R'},
            { "debug", 1, 0, 'g'},
            { NULL, 0, 0, 0},
        };
        int c;

        c = getopt_long(argc, argv, "D:F:s:d:b:g:lHOLC3NR", lopts, NULL);

        if (c == -1) break;
        switch (c) {
            case 'D': static_device_lcd = optarg;   break;
            case 'F': Spi_set_tx_file(optarg);      break;
            case 's': Spi_set_speed(optarg);        break;
            case 'd': Spi_set_delay(optarg);        break;
            case 'b': Spi_set_bits(optarg);         break;
            case 'l': Spi_set_mode_loop();          break;
            case 'H': Spi_set_mode_cpha();          break;
            case 'O': Spi_set_mode_cpol();          break;
            case 'L': Spi_set_mode_lsb_first();     break;
            case 'C': Spi_set_mode_cs_high();       break;
            case '3': Spi_set_mode_3wire();         break;
            case 'N': Spi_set_mode_no_cs();         break;
            case 'R': Spi_set_mode_ready();         break;
            case 'g': Spi_enable_debug(optarg);     break;
            default: print_usage(argv[0]);          break;
        }
    }
}

#define lcd_printf32(col,row,width,fore_color,back_color,fmt,...) do {\
            if (static_fd_lcd >= 0) {\
                int     i;\
                int     w = (width < 20) ? width : 20;\
                char    msg[64];\
                msg[w] = 0;\
                snprintf(msg,w,fmt,##__VA_ARGS__); \
                for (i = 0; i < w; i ++) if (msg[i] == 0) msg[i] = 0;\
                lcd_ili9341_BACK_COLOR = back_color;\
                lcd_ili9341_POINT_COLOR = fore_color;\
                Lcd_ili9341_ShowString(static_fd_lcd,col,row,32,16,msg);\
            }\
        } while (0)

#define lcd_printf(col,row,width,fore_color,back_color,fmt,...) do {\
            if (static_fd_lcd >= 0) {\
                int     i;\
                int     w = (width < 40) ? width : 40;\
                char    msg[64];\
                msg[w] = 0;\
                snprintf(msg,w,fmt,##__VA_ARGS__); \
                for (i = 0; i < w; i ++) if (msg[i] == 0) msg[i] = 0;\
                lcd_ili9341_BACK_COLOR = back_color;\
                lcd_ili9341_POINT_COLOR = fore_color;\
                Lcd_ili9341_ShowString(static_fd_lcd,col,row,16,8,msg);\
            }\
        } while (0)

                
#define LCD_ShowStatusBar(fmt,...)        do {\
            lcd_printf(1,220,40,LCD_ILI9341_WHITE,LCD_ILI9341_BLACK,fmt,##__VA_ARGS__);\
        } while (0)


int main(int argc, char *argv[]) {
    int ret = 0;
    static int static_fd_lcd = -1;
    static int static_fd_tch = -1;
    uint8_t mode = 0;

    parse_opts(argc, argv);

    static_fd_lcd = open(static_device_lcd, O_RDWR);
    if (static_fd_lcd < 0)
        pabort("can't open device");

    ret = Spi_setting(static_fd_lcd);

    if (ret >= 0) {
        if (Spi_Is_From_File()) {
            Spi_write_reg(static_fd_lcd);
        } else {
#if (LCD_TYPE == LCD_TYPE_OMCJ4X8C)
            Lcd_omcj4x8c_init_lcd(fd_lcd);
            while (1) {
                Lcd_omcj4x8c_demo(fd_lcd);
                usleep(3000000);
            }
#elif (LCD_TYPE == LCD_TYPE_ILI9341)
            static_fd_tch = open(static_device_touch, O_RDWR);
            lcd_ili9341_BACK_COLOR = LCD_ILI9341_BLACK;
            lcd_ili9341_POINT_COLOR = LCD_ILI9341_WHITE;
            Lcd_ili9341_Init(static_fd_lcd); //tft初始化
            Lcd_ili9341_Clear(static_fd_lcd, LCD_ILI9341_BLACK); //清屏
            Lcd_ili9341_ShowChinese(static_fd_lcd, 120, 10, &lcd_author_name, 0);
            LCD_ShowStatusBar("%s", "Yaoyang Wang");
//            if (static_fd_tch >= 0) {
//                printf("Touch Calibration!\n");
//                Touch_ads7843_Adjust(static_fd_lcd, static_fd_tch);
//            }
//            Lcd_ili9341_ShowIcons(static_fd_lcd, 0, 8, 0, 6,lcd_ili9341_image);
//            while (1) {
//                Lcd_ili9341_demo(static_fd_lcd); //显示40*40图片
//                usleep(100000);
//            }
            if (static_fd_tch >= 0)
                close(static_fd_tch);
#else
            Spi_write_reg(static_fd_lcd);
#endif
        }
        close(static_fd_lcd);
    }

    return ret;
}
