#include <stdio.h>
#include <string.h>
#include "lcd_spi.h"
#include "lcd_ili9341.h"
#include "lcd_ili9341_font.h"

/* 
代码测试环境:单片机STC12LE5A60S2,晶振33M  单片机工作电压3.3V
程序默认IO连接方式：
CS     =P0^0;     //片选	
DC     =P0^2;	  //数据/命令切换
SDI    =P1^5;	  //数据
SCK    =P1^7;	  //时钟
REST   =P0^1;	  //复位  
触摸功能连接方式：(不使用触摸可不连接)
T_CLK-P1^7;  T_CS-P1^4;  T_DIN-P3^0;  T_OUT-P3^1;  T_IRQ-P3^4;
 */

uint16 lcd_ili9341_BACK_COLOR = 0;
uint16 lcd_ili9341_POINT_COLOR = 0; //背景色，画笔色

#define SPI_LCD_DEV_NO          0
#define LCD_DISPLAY_MAP_SIZE    4096
static unsigned short static_Lcd_ili9341_display_buffer[LCD_ILI9341_WIDE][LCD_ILI9341_HIGH];
static unsigned char static_Lcd_ili9341_display_map[LCD_DISPLAY_MAP_SIZE];
static unsigned long static_Lcd_ili9341_display_len = 0;

static unsigned int Lcd_ili9341_Writ_Bus(int fd, uchar dcx, char da) //串行数据写入
{
    if (fd >= 0) { //without LCD_CTRL_BIT_READ
        uchar tx[] = {da};

        Spi_Set_DCX(fd, dcx);
        return Spi_transfer(fd, tx, ARRAY_SIZE(tx), SPI_LCD_DEV_NO);
    }
    return 0;
}

static void Lcd_ili9341_WR_REG(int fd, char da) {
    Lcd_ili9341_Writ_Bus(fd, SPI_DCX_COMD, da);
}

static void Lcd_ili9341_WR_BYTE(int fd, char da) //发送数据-8位参数
{
    Lcd_ili9341_Writ_Bus(fd, SPI_DCX_DATA, da);
}

void Lcd_ili9341_WR_MEM(int fd, char *tx, int len) {
    if (fd >= 0 && tx && len > 0) { //without LCD_CTRL_BIT_READ
        Spi_Set_DCX(fd, SPI_DCX_DATA);
        Spi_transfer(fd, tx, len, SPI_LCD_DEV_NO);
    }
}

static void Lcd_ili9341_WR_WORD(int fd, int da) {
    char word[2] = { da >> 8, da };
    Lcd_ili9341_WR_MEM(fd, word, 2);
}

static void Lcd_ili9341_WR_DWORD(int fd, int da) {
    char dword[4] = { da >> 24, da >> 16, da >> 8, da };
    Lcd_ili9341_WR_MEM(fd, dword, 4);
}

static void Lcd_ili9341_WR_REG_WORD(int fd, int reg, int da) {
    Lcd_ili9341_WR_REG(fd, reg);
    Lcd_ili9341_WR_WORD(fd, da);
}

static void Lcd_ili9341_WR_REG_DWORD(int fd, int reg, int da) {
    Lcd_ili9341_WR_REG(fd, reg);
    Lcd_ili9341_WR_DWORD(fd, da);
}

static void Lcd_ili9341_WR_REG_BYTE(int fd, int reg, char da) {
    Lcd_ili9341_WR_REG(fd, reg);
    Lcd_ili9341_WR_BYTE(fd, da);
}

static void Lcd_ili9341_WR_REG_MEM(int fd, int reg, char* tx, int len) {
    Lcd_ili9341_WR_REG(fd, reg);
    Lcd_ili9341_WR_MEM(fd, tx, len);
}

static void Lcd_ili9341_Address_set(int fd, unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2) {
    char addr_x[4] = { x1 >> 8, x1, x2 >> 8, x2 };
    char addr_y[4] = { y1 >> 8, y1, y2 >> 8, y2 };
    Lcd_ili9341_WR_REG_MEM(fd, 0x2a, addr_x, 4);
    Lcd_ili9341_WR_REG_MEM(fd, 0x2b, addr_y, 4);
    Lcd_ili9341_WR_REG(fd, 0x2C);
}

static void Lcd_ili9341_Map_Clear(void) {
    static_Lcd_ili9341_display_len = 0;
}

static void Lcd_ili9341_Map_CommitMem(int fd, char* mem, int len) {
    if (fd && 0 < len) {
        Lcd_ili9341_WR_MEM(fd, mem, len);
    }
}

static void Lcd_ili9341_Map_Commit(int fd) {
//    printf("WYY:    %s(%d)  display memory[%d]\n",__func__,__LINE__,static_Lcd_ili9341_display_len);
    if (fd && 0 < static_Lcd_ili9341_display_len && static_Lcd_ili9341_display_len <= LCD_DISPLAY_MAP_SIZE) {
        Lcd_ili9341_WR_MEM(fd, static_Lcd_ili9341_display_map, static_Lcd_ili9341_display_len);
    }
    static_Lcd_ili9341_display_len = 0;
}

static void Lcd_ili9341_Map_Append_Btye(int fd, unsigned char da, unsigned long limit) {
    if (0 <= static_Lcd_ili9341_display_len && static_Lcd_ili9341_display_len < LCD_DISPLAY_MAP_SIZE) {
        static_Lcd_ili9341_display_map[static_Lcd_ili9341_display_len] = da;
        static_Lcd_ili9341_display_len ++;
    }
    if (limit > 0 && static_Lcd_ili9341_display_len >= limit)
        Lcd_ili9341_Map_Commit(fd);
}

static void Lcd_ili9341_Map_Append_Word(int fd, unsigned int da, unsigned long limit) {
    Lcd_ili9341_Map_Append_Btye(fd, da >> 8, 0);
    Lcd_ili9341_Map_Append_Btye(fd, da, limit);
}

#define LCD_ILI9341_WR_REG_MEMO(fd, cmd, tx) do { Lcd_ili9341_WR_REG_MEM(fd, cmd, tx, sizeof(tx)); } while (0)

static void Lcd_ili9341_Reset(int fd) {
//    if (SPI_DEBUG_RESET & Spi_get_debug()) {
        char p_CMD_PWR_SEQ_CT_A[] = {0x64,0x03,0x12,0x81};
        printf("Reset LCD (ili9341)...");
        GPIO_SET(fd, GPIO_SPI_LCD_RST,0);
        GPIO_SET(fd, GPIO_SPI_LCD_CS,0);
        GPIO_SET(fd, GPIO_SPI_TCH_CS,0);
        GPIO_SET(fd, GPIO_SPI_TCH_BUSY,0);
        delayms(100);
        GPIO_SET(fd, GPIO_SPI_LCD_RST,1);
        GPIO_SET(fd, GPIO_SPI_LCD_CS,1);
        GPIO_SET(fd, GPIO_SPI_TCH_CS,1);
        GPIO_SET(fd, GPIO_SPI_TCH_BUSY,1);
        delayms(100);
        Lcd_ili9341_WR_REG(fd, CMD_SOFTWARE_RST);
        LCD_ILI9341_WR_REG_MEMO(fd, CMD_PWR_SEQ_CT_A, p_CMD_PWR_SEQ_CT_A);
        printf("done\n");
//    }
}

void Lcd_ili9341_Init(int fd) { //调用一次这些函数，免得编译的时候提示警告
    char p_CMD_PWR_CONTRL_A[] = {0x39,0x2c,0x00,0x34,0x02};
    char p_CMD_PWR_CONTRL_B[] = {0x00,0xc1,0x30};
//    char p_CMD_TIM_CONTRL_A[] = {0x85,0x00,0x78};
    char p_CMD_TIM_CONTRL_A[] = {0x85,0x10,0x7A};
    char p_CMD_PWR_SEQ_CT_A[] = {0x64,0x03,0x12,0x81};
    char p_CMD_DISPLAY_FUNC[] = {0x08,0x82,0x27};
    char p_CMD_POS_GAMMA_CR[] = {0x0F,0x31,0x2B,0x0C,0x0E,0x08,0x4E,0xF1,0x37,0x07,0x10,0x03,0x0E,0x09,0x00};
    char p_CMD_NEG_GAMMA_CR[] = {0x00,0x0E,0x14,0x03,0x11,0x07,0x31,0xC1,0x48,0x08,0x0F,0x0C,0x31,0x36,0x0F};
    Lcd_ili9341_Reset(fd);
    LCD_ILI9341_WR_REG_MEMO(fd, CMD_PWR_CONTRL_A, p_CMD_PWR_CONTRL_A);  //Power Control A
    LCD_ILI9341_WR_REG_MEMO(fd, CMD_PWR_CONTRL_B, p_CMD_PWR_CONTRL_B);
    LCD_ILI9341_WR_REG_MEMO(fd, CMD_TIM_CONTRL_A, p_CMD_TIM_CONTRL_A);
    Lcd_ili9341_WR_REG_WORD(fd, CMD_TIM_CONTRL_B, 0x0000);
    LCD_ILI9341_WR_REG_MEMO(fd, CMD_PWR_SEQ_CT_A, p_CMD_PWR_SEQ_CT_A);
    Lcd_ili9341_WR_REG_BYTE(fd, CMD_PMP_RATIO_CT, 0x20);
    Lcd_ili9341_WR_REG_BYTE(fd, CMD_PWR_CONTRL_1, 0x23);                //Power control , VRH[5:0] 
    Lcd_ili9341_WR_REG_BYTE(fd, CMD_PWR_CONTRL_2, 0x10);                //Power control , SAP[2:0];BT[3:0] 
    Lcd_ili9341_WR_REG_WORD(fd, CMD_VCM_CONTRL_1, 0x3e28);              //VCM control , //对比度调节
    Lcd_ili9341_WR_REG_BYTE(fd, CMD_VCM_CONTRL_2, 0x86);                //VCM control2 
//    Lcd_ili9341_WR_REG_BYTE(fd, CMD_MEM_ACCESS_C, 0x48);                // Memory Access Control , //48 68竖屏//28 E8 横屏
    Lcd_ili9341_WR_REG_BYTE(fd, CMD_MEM_ACCESS_C, 0x28);                // Memory Access Control , //48 68竖屏//28 E8 横屏
    Lcd_ili9341_WR_REG_BYTE(fd, CMD_PIXEL_FORMAT, 0x55);
    Lcd_ili9341_WR_REG_WORD(fd, CMD_FRAME_RATE_C, 0x0018);
    LCD_ILI9341_WR_REG_MEMO(fd, CMD_DISPLAY_FUNC, p_CMD_DISPLAY_FUNC);  // Display Function Control 
    Lcd_ili9341_WR_REG_BYTE(fd, CMD_ENABL_3GAMMA, 0x00);                // 3Gamma Function Disable 
    Lcd_ili9341_WR_REG_BYTE(fd, CMD_GAMMA_CURVES, 0x01);                // Gamma curve selected 
    LCD_ILI9341_WR_REG_MEMO(fd, CMD_POS_GAMMA_CR, p_CMD_POS_GAMMA_CR);  //Set Gamma positive correction
    LCD_ILI9341_WR_REG_MEMO(fd, CMD_NEG_GAMMA_CR, p_CMD_NEG_GAMMA_CR);  //Set Gamma negative correction
    Lcd_ili9341_WR_REG(fd, CMD_SLEEP_MODE_O); //Exit Sleep 
    delayms(120);
    Lcd_ili9341_WR_REG(fd, CMD_SET_DISP__ON); //Display on 
    Lcd_ili9341_WR_REG(fd, CMD_WRITE_MEMORY);

}
//清屏函数
//Color:要清屏的填充色

void Lcd_ili9341_Clear(int fd, uint16 Color) {
    unsigned short w, h, pixel = Color;
    Lcd_ili9341_Address_set(fd, 0, 0, LCD_ILI9341_WIDE - 1, LCD_ILI9341_HIGH - 1);
    for (h = 0; h < LCD_ILI9341_HIGH; h ++) {
        for (w = 0; w < LCD_ILI9341_WIDE; w ++) {
//            unsigned char hi = lcd_image_lcd_ili9341_image_logo.m_image[2 * (h*LCD_ILI9341_WIDE + w)];
//            unsigned char lo = lcd_image_lcd_ili9341_image_logo.m_image[2 * (h*LCD_ILI9341_WIDE + w) + 1];
//            static_Lcd_ili9341_display_buffer[w][h] = Color = (hi << 8) | lo;
            pixel = *(uint16*) &lcd_image_lcd_ili9341_image_logo.m_image[2 * (h*LCD_ILI9341_WIDE + w)];
            Lcd_ili9341_Map_Append_Word(fd,pixel,LCD_DISPLAY_LIMIT_IMG);
            static_Lcd_ili9341_display_buffer[w][h] = pixel;
//        Lcd_ili9341_ShowIcon(fd_lcd, x0, y0,
//                img[nImg]->m_width + x0,
//                img[nImg]->m_hight + y0,
//                img[nImg]->m_image);
//            Lcd_ili9341_Map_Append_Word(fd,Color,LCD_DISPLAY_LIMIT_IMG);
        }
    }
    Lcd_ili9341_Map_Commit(fd);
}
//在指定位置显示一个汉字(32*33大小)
//dcolor为内容颜色，gbcolor为背静颜色

static void Lcd_ili9341_ShowChnWord(int fd, unsigned int x, unsigned int y, ChineseLattice word, bool draw_bk) {
    unsigned char i, j, k;
    unsigned char *temp = (unsigned char*) word.m_lattice;
    unsigned short color = 0;

    Lcd_ili9341_Address_set(fd, x, y, x + 31, y + 31); //设置区域      
    Lcd_ili9341_Map_Clear();
    for (i = 0; i < 32; i++) {
        for (j = 0; j < 4; j++) {
            for (k = 0; k < 8; k ++) {
                if (draw_bk) {
                    color = ((word.m_lattice[i][j] & (0x80 >> k)) != 0) ?
                                lcd_ili9341_POINT_COLOR : lcd_ili9341_BACK_COLOR;
                    static_Lcd_ili9341_display_buffer[x + j * 8 + k][y + i] = color;
                    Lcd_ili9341_Map_Append_Word(fd, color, LCD_DISPLAY_LIMIT_TEXT);
                } else {// if ((word.m_lattice[i][j] & (0x80 >> k)) != 0) {
                    color = ((word.m_lattice[i][j] & (0x80 >> k)) != 0) ?
                                lcd_ili9341_POINT_COLOR : static_Lcd_ili9341_display_buffer[x + j * 8 + k][y + i];
                    static_Lcd_ili9341_display_buffer[x + j * 8 + k][y + i] = color;
                    Lcd_ili9341_Map_Append_Word(fd, color, LCD_DISPLAY_LIMIT_TEXT);
                }
            }
        }
    }
    Lcd_ili9341_Map_Commit(fd);
}

void Lcd_ili9341_ShowChinese(int fd, unsigned int x, unsigned int y, ChineseWords *words, bool draw_bk) {
    int i;
    int col = x;
    int row = y;
    for (i = 0; i < words->m_nWord; i ++, col += 32)
        Lcd_ili9341_ShowChnWord(fd,col,row,words->m_words[i],draw_bk);
}

//画点
//POINT_COLOR:此点的颜色

void Lcd_ili9341_DrawPoint(int fd, uint16 x, uint16 y) {
    Lcd_ili9341_Address_set(fd, x, y, x, y); //设置光标位置 
    Lcd_ili9341_WR_WORD(fd, lcd_ili9341_POINT_COLOR);
}
//画一个大点
//POINT_COLOR:此点的颜色

void Lcd_ili9341_DrawPoint_big(int fd, uint16 x, uint16 y) {
    Lcd_ili9341_Fill(fd, x - 1, y - 1, x + 1, y + 1, lcd_ili9341_POINT_COLOR);
}
//在指定区域内填充指定颜色
//区域大小:
//  (xend-xsta)*(yend-ysta)

void Lcd_ili9341_Fill(int fd, uint16 xsta, uint16 ysta, uint16 xend, uint16 yend, uint16 color) {
    uint16 i, j;
    Lcd_ili9341_Address_set(fd, xsta, ysta, xend, yend); //设置光标位置 
    for (i = ysta; i <= yend; i++) {
        for (j = xsta; j <= xend; j++)
            Lcd_ili9341_Map_Append_Word(fd, color, LCD_DISPLAY_LIMIT_IMG); //设置光标位置 	    
    }
    Lcd_ili9341_Map_Commit(fd);
}
//画线
//x1,y1:起点坐标
//x2,y2:终点坐标  

void Lcd_ili9341_DrawLine(int fd, uint16 x1, uint16 y1, uint16 x2, uint16 y2) {
    uint16 t;
    int xerr = 0, yerr = 0, delta_x, delta_y, distance;
    int incx, incy, uRow, uCol;

    delta_x = x2 - x1; //计算坐标增量 
    delta_y = y2 - y1;
    uRow = x1;
    uCol = y1;
    if (delta_x > 0)incx = 1; //设置单步方向 
    else if (delta_x == 0)incx = 0; //垂直线 
    else {
        incx = -1;
        delta_x = -delta_x;
    }
    if (delta_y > 0)incy = 1;
    else if (delta_y == 0)incy = 0; //水平线 
    else {
        incy = -1;
        delta_y = -delta_y;
    }
    if (delta_x > delta_y)distance = delta_x; //选取基本增量坐标轴 
    else distance = delta_y;
    for (t = 0; t <= distance + 1; t++)//画线输出 
    {
        Lcd_ili9341_DrawPoint(fd, uRow, uCol); //画点 
        xerr += delta_x;
        yerr += delta_y;
        if (xerr > distance) {
            xerr -= distance;
            uRow += incx;
        }
        if (yerr > distance) {
            yerr -= distance;
            uCol += incy;
        }
    }
}
//画矩形

void Lcd_ili9341_DrawRectangle(int fd, uint16 x1, uint16 y1, uint16 x2, uint16 y2) {
    Lcd_ili9341_DrawLine(fd, x1, y1, x2, y1);
    Lcd_ili9341_DrawLine(fd, x1, y1, x1, y2);
    Lcd_ili9341_DrawLine(fd, x1, y2, x2, y2);
    Lcd_ili9341_DrawLine(fd, x2, y1, x2, y2);
}
//在指定位置画一个指定大小的圆
//(x,y):中心点
//r    :半径

void Lcd_ili9341_Draw_Circle(int fd, uint16 x0, uint16 y0, uchar r) {
    int a, b;
    int di;
    a = 0;
    b = r;
    di = 3 - (r << 1); //判断下个点位置的标志
    while (a <= b) {
        Lcd_ili9341_DrawPoint(fd, x0 - b, y0 - a); //3           
        Lcd_ili9341_DrawPoint(fd, x0 + b, y0 - a); //0           
        Lcd_ili9341_DrawPoint(fd, x0 - a, y0 + b); //1       
        Lcd_ili9341_DrawPoint(fd, x0 - b, y0 - a); //7           
        Lcd_ili9341_DrawPoint(fd, x0 - a, y0 - b); //2             
        Lcd_ili9341_DrawPoint(fd, x0 + b, y0 + a); //4               
        Lcd_ili9341_DrawPoint(fd, x0 + a, y0 - b); //5
        Lcd_ili9341_DrawPoint(fd, x0 + a, y0 + b); //6 
        Lcd_ili9341_DrawPoint(fd, x0 - b, y0 + a);
        a++;
        //使用Bresenham算法画圆     
        if (di < 0)di += 4 * a + 6;
        else {
            di += 10 + 4 * (a - b);
            b--;
        }
        Lcd_ili9341_DrawPoint(fd, x0 + a, y0 + b);
    }
}
//在指定位置显示一个字符

//num:要显示的字符:" "--->"~"
//mode:叠加方式(1)还是非叠加方式(0)
//在指定位置显示一个字符

//num:要显示的字符:" "--->"~"

//mode:叠加方式(1)还是非叠加方式(0)

void Lcd_ili9341_ShowChar(int fd, uint16 x, uint16 y, uchar num, uchar hight,uchar width,uchar mode) {
    uchar col[2];
    uchar pos, t;
    uint16 x0 = x;
    uint16 colortemp = lcd_ili9341_POINT_COLOR;
    if (x > LCD_ILI9341_WIDE - width || y > LCD_ILI9341_HIGH - hight) return;
    //设置窗口		   
    num = num - ' '; //得到偏移后的值
    Lcd_ili9341_Address_set(fd, x, y, x + width - 1, y + hight - 1); //设置光标位置 
    if (!mode) { //非叠加方式
        for (pos = 0; pos < hight; pos++) {
            if (width == 16) {
                col[0] = lcd_ili9341_asc2_3216[(uint16) num * 64 + pos * 2]; //调用1608字体
                col[1] = lcd_ili9341_asc2_3216[(uint16) num * 64 + pos * 2 + 1]; //调用1608字体
            } else if (width == 8) {
                col[0] = lcd_ili9341_asc2_1608[(uint16) num * 16 + pos]; //调用1608字体
            } else return;
            for (t = 0; t < width; t++) {
                if (col[t/8] & 0x80) lcd_ili9341_POINT_COLOR = colortemp;
                else lcd_ili9341_POINT_COLOR = lcd_ili9341_BACK_COLOR;
                Lcd_ili9341_Map_Append_Word(fd, lcd_ili9341_POINT_COLOR, LCD_DISPLAY_LIMIT_TEXT);
                col[t/8] <<= 1;
                x++;
            }
            x = x0;
            y++;
        }
        Lcd_ili9341_Map_Commit(fd);
    } else {//叠加方式
        for (pos = 0; pos < hight; pos++) {
            if (width == 16) {
                col[0] = lcd_ili9341_asc2_3216[(uint16) num * 64 + pos * 2]; //调用1608字体
                col[1] = lcd_ili9341_asc2_3216[(uint16) num * 64 + pos * 2 + 1]; //调用1608字体
            } else if (width == 8) {
                col[0] = lcd_ili9341_asc2_1608[(uint16) num * 16 + pos]; //调用1608字体
            } else return;
            for (t = 0; t < width; t++) {
                if (col[t/8] & 0x80) Lcd_ili9341_DrawPoint(fd, x + t, y + pos); //画一个点     
                col[0] <<= 1;
            }
        }
    }
    lcd_ili9341_POINT_COLOR = colortemp;
}
//m^n函数

static uint mypow(uchar m, uchar n) {
    uint result = 1;
    while (n--)result *= m;
    return result;
}
//显示2个数字
//x,y :起点坐标	 
//len :数字的位数
//color:颜色
//num:数值(0~4294967295);	

void Lcd_ili9341_ShowNum(int fd, uint16 x, uint16 y, uint num, uchar len) {
    uchar t, temp;
    uchar enshow = 0;
    num = (uint16) num;
    for (t = 0; t < len; t++) {
        temp = (num / mypow(10, len - t - 1)) % 10;
        if (enshow == 0 && t < (len - 1)) {
            if (temp == 0) {
                Lcd_ili9341_ShowChar(fd, x + 8 * t, y, ' ', 16, 8, 0);
                continue;
            } else enshow = 1;

        }
        Lcd_ili9341_ShowChar(fd, x + 8 * t, y, temp + 48, 16, 8, 0);
    }
}
//显示2个数字
//x,y:起点坐标
//num:数值(0~99);	 

void Lcd_ili9341_Show2Num(int fd, uint16 x, uint16 y, uint16 num, uchar len) {
    uchar t, temp;
    for (t = 0; t < len; t++) {
        temp = (num / mypow(10, len - t - 1)) % 10;
        Lcd_ili9341_ShowChar(fd, x + 8 * t, y, temp + '0', 16, 8, 0);
    }
}
//显示字符串
//x,y:起点坐标  
//*p:字符串起始地址
//用16字体

void Lcd_ili9341_ShowString(int fd, uint16 x, uint16 y, uint16 hight, uint16 width, const uchar *p) {
    while (*p != '\0') {
        if (x > LCD_ILI9341_WIDE - width) {
            x = 0;
            y += hight;
        }
        if (y > LCD_ILI9341_HIGH - hight) {
            y = x = 0;
            Lcd_ili9341_Clear(fd, LCD_ILI9341_BLACK);
        }
        Lcd_ili9341_ShowChar(fd, x, y, *p, hight, width, 0);
        x += width;
        p++;
    }
}


void Lcd_ili9341_ShowIcon(int fd, uint16 left, uint16 top, uint16 right, uint16 bottom, unsigned char *image) {
    int h,w;
    int nWidth = right - left;
    int nHight = bottom - top;
//    uint16 **img = image;
    Lcd_ili9341_Map_Clear();
    Lcd_ili9341_Address_set(fd, left, top, right - 1, bottom - 1); //坐标设置
    for (h = 0; h < nHight; h ++) {
        for (w = 0; w < nWidth; w ++) {
            unsigned char hi = image[(h * nWidth + w) * 2];
            unsigned char lo = image[(h * nWidth + w) * 2 + 1];
            unsigned short color = hi << 8 | lo;
            static_Lcd_ili9341_display_buffer[w][h] = color;
            Lcd_ili9341_Map_Append_Word(fd, color, LCD_DISPLAY_LIMIT_IMG);
        }
    }
    Lcd_ili9341_Map_Commit(fd);
}

//Lcd_ili9341_ShowIcons(int fd, 3, 8, 0, 6,lcd_ili9341_image);

void Lcd_ili9341_ShowIcons(int fd, int r0, int r1, int c0, int c1, unsigned char *image) {
    int j, k;
    for (k = r0; k < r1; k++) {
        for (j = c0; j < c1; j++) {
            Lcd_ili9341_ShowIcon(fd, 40 * j, 40 * k, 40 * j + 39, 40 * k + 39,image);
        }
    }
}

void Lcd_ili9341_ClearIcons(int fd, int r0, int r1, int c0, int c1, uint16 color) {
    int j, k;
    for (k = r0; k < r1; k++) {
        for (j = c0; j < c1; j++) {
            Lcd_ili9341_Fill(fd, 40 * j, 40 * k, 40 * j + 39, 40 * k + 39, color);
        }
    }
}

