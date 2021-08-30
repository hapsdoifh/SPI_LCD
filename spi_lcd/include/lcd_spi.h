/* 
 * File:   lcd_spi.h
 * Author: wyy
 *
 * Created on 2016�?1??29??, �???3:03
 */

#ifndef LCD_SPI_H
#define	LCD_SPI_H

#ifdef	__cplusplus
extern "C" {
#endif

#include <unistd.h>

#define bool    unsigned char
#define uint    unsigned int
#define uint16  unsigned short
#define uchar   unsigned char
#define LCD_X1  0x80
#define LCD_X2  0x88
#define LCD_Y   0x80
#define RS_DATA 1
#define RS_COMD 0
#define TRUE    1
#define FALSE   0
#define code
#define ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))
#define delayms(ms)         usleep(ms*1000)

#define MACH_AR933X         0
#define MACH_AR934X         0
#define MACH_AR953X         0
#define MACH_QCA956X        1

#if (MACH_AR934X)
//#define PLATFORM        "/sys/devices/platform/"
//#define IF_LED          PLATFORM "leds-gpio/leds/gh0228:"
//#define DEV_NAME_BRN    "/brightness"
//#define SPI_LCD_DCX     IF_LED "dcx" DEV_NAME_BRN
//#define SPI_TOUCH_BUSY  IF_LED "busy" DEV_NAME_BRN
//
#define GPIO_SPI_LCD_CS       0   //AR9344_AE4_PCB_J27_PIN_02   default JTAG_TDI
#define GPIO_SPI_LCD_RST      1   //AR9344_R27_PCB_J27_PIN_03   default JTAG_TCK
#define GPIO_SPI_LCD_DCX      2   //AR9344_R27_PCB_J27_PIN_04   default JTAG_TDO
#define GPIO_SPI_MOSI         3   //AR9344_AE4_PCB_J27_PIN_05   default JTAG_TMS
#define GPIO_SPI_MISO         4   //AR9344_AG5_PCB_J27_PIN_06   Input only
#define GPIO_SPI_SCK          12  //AR9344_N27_PCB_J27_PIN_09
#define GPIO_SPI_TCH_CS       14  //AR9344_N25_PCB_J27_PIN_10
#define GPIO_SPI_TCH_BUSY     18  //AR9344_M26_PCB_J27_PIN_11   Input only
#define AR934X_GPIO_COUNT           23
bool GPIO_GET(int fd, uint pin);
void GPIO_SET(int fd, uint pin, uint val);
#elif (MACH_QCA956X)
#define GPIO_SPI_MISO           17	//by default JTAG_TDO
#define GPIO_SPI_SCK            16	//by default JTAG_TCK
#define GPIO_SPI_LCD_CS         15	//by default JTAG_TMS
#define GPIO_SPI_MOSI           14	//by default JTAG_TDI
#define GPIO_SPI_LCD_DCX	7	//
#define GPIO_SPI_LCD_RST	6	//
#define GPIO_SPI_TCH_CS         5	//
#define GPIO_SPI_TCH_BUSY	1	//
#define QCA956X_GPIO_COUNT          23
bool GPIO_GET(int fd, uint pin);
void GPIO_SET(int fd, uint pin, uint val);
#else   //NO GWS_LEDS
#define AR933X_GPIO_COUNT           30
#define GPIO_SET(int fd, name,val)      do {} while(0)
#define GPIO_GET(name)          (0)
#endif  //IS_SERV_AR9344

#define SPI_DCX_NONE            -1
#define SPI_DCX_DATA            1
#define SPI_DCX_COMD            0

//===============================================ANSI控制码的说明
#define VT100_FLAG0         "\x1b"          //ESC
#define VT100_FLAG1         "["
#define VT100_FLAG          VT100_FLAG0 VT100_FLAG1
#define VT100_HEAD          VT100_FLAG
#define VT100_UP(n)         VT100_HEAD n "A"    //ESC[nA 光标上移n行
#define VT100_DOWN(n)       VT100_HEAD n "B"    //ESC[nB 光标下移n行
#define VT100_RIGHT(n)      VT100_HEAD n "C"    //ESC[nC 光标右移n行
#define VT100_LEFT(n)       VT100_HEAD n "D"    //ESC[nD 光标左移n行
#define VT100_CLR2EOF       VT100_HEAD "J"      //
#define VT100_CLR_EOL       VT100_HEAD "K"      //ESC[K 清除从光标到行尾的内容
#define VT100_CLEAR         VT100_HEAD "2J"     //ESC[2J 清屏
#define VT100_SAVE_CURSOR   VT100_HEAD "s"      //ESC[s 保存光标位置
#define VT100_LOAD_CURSOR   VT100_HEAD "u"      //ESC[u 恢复光标位置
#define VT100_HIDE_CURSOR   VT100_HEAD "?25l"   //ESC[?25l 隐藏光标
#define VT100_SHOW_CURSOR   VT100_HEAD "?25h"   //ESC[?25h 显示光标
#define VT100_RESET         VT100_HEAD "0m"     //ESC[0m 关闭所有属性
#define VT100_HIGHT_LIGHT   VT100_HEAD "1m"     //ESC[1m 设置高亮度
#define VT100_UNDER_LINE    VT100_HEAD "4m"     //ESC[4m 下划线
#define VT100_FLASH         VT100_HEAD "5m"     //ESC[5m 闪烁
#define VT100_INVERT        VT100_HEAD "7m"     //ESC[7m 反显
#define VT100_INVISIBLE     VT100_HEAD "8m"     //ESC[8m 消隐
#define VT100_GOTO(x,y)     VT100_HEAD x ";" y "H" VT100_CLR_EOL    //ESC[y;xH设置光标位置
#define VT100_COLOR(b,f)    VT100_HEAD b ";" f "m"                  //
//ESC[30m&nbsp-- ESC[37m 设置前景色
//ESC[40m&nbsp-- ESC[47m 设置背景色

//字颜色:30-----------39
#define VT100_F_BLACK       "30"    //黑
#define VT100_F_RED         "31"    //红
#define VT100_F_GREEN       "32"    //绿
#define VT100_F_YELLOW      "33"    //黄
#define VT100_F_BLUE        "34"    //蓝色
#define VT100_F_PINK        "35"    //紫色
#define VT100_F_DARK_G      "36"    //深绿
#define VT100_F_WHITE       "37"    //白色

//字背景颜色范围:40----49
#define VT100_B_BLACK       "40"    //黑
#define VT100_B_RED         "41"    //深红
#define VT100_B_GREEN       "42"    //绿
#define VT100_B_YELLOW      "43"    //黄色
#define VT100_B_BLUE        "44"    //蓝色
#define VT100_B_PINK        "45"    //紫色
#define VT100_B_DARK_G      "46"    //深绿
#define VT100_B_WHITE       "47"    //白色

//#define VT100_STYLE0        VT100_RESET
#define VT100_STYLE0        VT100_COLOR(VT100_B_BLACK,VT100_F_GREEN)
#define VT100_STYLE1        VT100_COLOR(VT100_B_BLACK,VT100_F_WHITE) VT100_HIGHT_LIGHT
#define VT100_STYLE2        VT100_COLOR(VT100_B_DARK_G,VT100_F_WHITE) VT100_HIGHT_LIGHT
#define VT100_STYLE_BLUE    VT100_COLOR(VT100_B_BLUE,VT100_F_WHITE) VT100_HIGHT_LIGHT
#define VT100_STYLE_ALERT   VT100_COLOR(VT100_B_RED,VT100_F_YELLOW) VT100_HIGHT_LIGHT
#define VT100_STYLE_NORMAL  VT100_COLOR(VT100_B_GREEN,VT100_F_WHITE) VT100_HIGHT_LIGHT
#define VT100_STYLE_MENU    VT100_RESET VT100_COLOR(VT100_B_BLACK,VT100_F_GREEN) VT100_HIGHT_LIGHT
#define VT100_STYLE_KEY     VT100_COLOR(VT100_B_BLACK,VT100_F_YELLOW) VT100_HIGHT_LIGHT VT100_UNDER_LINE
#define VT100_STYLE_HOT     VT100_RESET VT100_COLOR(VT100_B_BLACK,VT100_F_RED) VT100_HIGHT_LIGHT


#define SPI_DEBUG_RESET         0x40000000
#define SPI_DEBUG_CH0_RECV      0x00000001
#define SPI_DEBUG_CH0_SEND      0x00000002
#define SPI_DEBUG_CH1_RECV      0x00000004
#define SPI_DEBUG_CH1_SEND      0x00000008
#define SPI_DEBUG_CH2_RECV      0x00000010
#define SPI_DEBUG_CH2_SEND      0x00000020

void pabort(const char *s);
void Spi_Set_DCX(int fd, uchar dcx);
uint Spi_transfer(int fd, uchar *tx_buffer, uchar len, uint debug);  //dcx : (-1 = no dcx, 0 = data, 1 = cmd) 
uint Spi_write_reg(int fd);
bool Spi_Is_From_File(void);
void Spi_enable_debug(char* optarg);
void Spi_set_tx_file(char* optarg);
void Spi_set_speed(char* optarg);
void Spi_set_delay(char* optarg);
void Spi_set_bits(char* optarg);
void Spi_set_mode_loop(void);
void Spi_set_mode_cpha(void);
void Spi_set_mode_cpol(void);
void Spi_set_mode_lsb_first(void);
void Spi_set_mode_cs_high(void);
void Spi_set_mode_3wire(void);
void Spi_set_mode_no_cs(void);
void Spi_set_mode_ready(void);
int Spi_setting(int fd);
uchar* Spi_get_rx_buff();
uint Spi_get_debug();

#ifdef	__cplusplus
}
#endif

#endif	/* LCD_SPI_H */

