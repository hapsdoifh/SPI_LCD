#include <stdio.h>
#include <string.h>
#include <time.h>
#include "touch_ads7843.h"
#include "lcd_ili9341.h"

//***因触摸屏批次不同等原因，默认的校准参数值可能会引起触摸识别不准，建议校准后再使用，不建议使用固定的默认校准参数
uint16 ads7843_vx = 15242, ads7843_vy = 11131; //比例因子，此值除以1000之后表示多少个AD值代表一个像素点
uint16 ads7843_chx = 3898, ads7843_chy = 145; //默认像素点坐标为0时的AD起始值
//***因触摸屏批次不同等原因，默认的校准参数值可能会引起触摸识别不准，建议校准后再使用，不建议使用固定的默认校准参数
#define SPI_TCH_DEV_NO          1

#define SPI_TCH_KEY_PRESSED     0
#define SPI_TCH_KEY_RELEASED    1
#define SPI_TCH_KEY_CLICKED     2

struct ADS7843_TP_PIX_ ads7843_tp_pixad, ads7843_tp_pixlcd; //当前触控坐标的AD值,前触控坐标的像素值   
static int static_tch_key_state = SPI_TCH_KEY_RELEASED;
static uchar Touch_ads7843_Key(int fd) {    //return (0--released / 1--pressed / 2--clicked)
    int n, nPressed = 0;
    int pressed = static_tch_key_state;
    
    for (n = 0; n < 50; n ++) { //detect 10 times to ensure the key state
        int v = GPIO_GET(fd, GPIO_SPI_TCH_BUSY);
        
        if (pressed != v) {
            pressed = v;
            nPressed = 0;
            if (Spi_get_debug() & SPI_DEBUG_CH2_BUSY) {
                printf( VT100_GOTO("24","70") VT100_STYLE0 "%s\n" VT100_RESET, v ? "[ ]" : "[_]");
                fflush(stdout);
            }
        } else {
            if (pressed == SPI_TCH_KEY_PRESSED) {
                nPressed ++;
                if (nPressed > 20) {
                    if (static_tch_key_state == SPI_TCH_KEY_RELEASED) {
                        static_tch_key_state = SPI_TCH_KEY_PRESSED;
                        if (Spi_get_debug() & SPI_DEBUG_CH2_BUSY) {
                            printf( VT100_GOTO("24","70") VT100_STYLE0 "[X]\n" VT100_RESET);
                            fflush(stdout);
                        }
                    }
                    return SPI_TCH_KEY_PRESSED;
                }
            }
        }
    }
    if (static_tch_key_state == SPI_TCH_KEY_PRESSED) {
        static_tch_key_state = SPI_TCH_KEY_RELEASED;
        if (Spi_get_debug() & SPI_DEBUG_CH2_BUSY) {
            printf( VT100_GOTO("24","70") VT100_STYLE0 "[O]\n" VT100_RESET);
            fflush(stdout);
        }
        return SPI_TCH_KEY_CLICKED;
    }
    return SPI_TCH_KEY_RELEASED;
}
//**********************************************************

void Touch_ads7843_spistar(int fd_tch) //SPI开始
{
//    CS = 1;
//    DCLK = 1;
//    DIN = 1;
//    DCLK = 1;
}
//**********************************************************

static void Touch_ads7843_WriteBtye(int fd_tch, unsigned char num) //SPI写数据
{
    unsigned char count = 0;
//    DCLK = 0;
//    for (count = 0; count < 8; count++) {
//        num <<= 1;
//        DIN = CY;
//        DCLK = 0;
//        _nop_();
//        _nop_();
//        _nop_(); //上升沿有效
//        DCLK = 1;
//        _nop_();
//        _nop_();
//        _nop_();
//    }
}
//**********************************************************

uint16 Touch_ads7843_ReadByte(int fd_tch) //SPI 读数据
{
    uchar count = 0;
    uint16 Num = 0;
//    for (count = 0; count < 12; count++) {
//        Num <<= 1;
//        DCLK = 1;
//        _nop_();
//        _nop_();
//        _nop_(); //下降沿有效
//        DCLK = 0;
//        _nop_();
//        _nop_();
//        _nop_();
//        if (DOUT) {
//            Num |= 1;
//        }
//
//    }

    return (Num);
}
//从7846/7843/XPT2046/UH7843/UH7846读取adc值	  0x90=y   0xd0-x

#define TCH_SHIFT_H(d)  (0x0FE0 & (d << 5))
#define TCH_SHIFT_L(d)  (0x001F & (d >> 3))
#define TCH_COMBIN(h,l) (TCH_SHIFT_H(h) | TCH_SHIFT_L(l))
#define BUBBLE_SWAP(d1,d2)  do {\
                                uint16  temp;\
                                if (d1 > d2) {\
                                    temp = d1;\
                                    d1 = d2;\
                                    d2 = temp;\
                                }\
                            } while (0)

static uint16 Touch_ads7843_Read_AD24(int fd_tch, unsigned char CMD) {
//    uint16 l;
//    CS = 0;
//    Touch_ads7843_WriteBtye(fd_tch, CMD); //送控制字即用差分方式读X坐标 详细请见有关资料
//    DCLK = 1;
//    _nop_();
//    _nop_();
//    _nop_();
//    _nop_();
//    DCLK = 0;
//    _nop_();
//    _nop_();
//    _nop_();
//    _nop_();
//    l = Touch_ads7843_ReadByte(fd_tch);
//    CS = 1;
//    return l;
    if (fd_tch >= 0) { //without LCD_CTRL_BIT_READ
        uchar tx[3] = {CMD, 0x00, 0x00};

        if (Spi_transfer(fd_tch, tx, ARRAY_SIZE(tx), SPI_TCH_DEV_NO) == 3) {
            uchar* rx = Spi_get_rx_buff();
            uint16 ad = TCH_COMBIN(rx[1],rx[2]);
            return (ad & 0x0fff);   //only 12 bits(LSB) validate
        }
    }
    return 0;
}
//读取一个坐标值
//连续读取READ_TIMES次数据,对这些数据升序排列,
//然后去掉最低和最高LOST_VAL个数,取平均值 
#define READ_TIMES 15 //读取次数
#define LOST_VAL 5	  //丢弃值

static uint16 Touch_ads7843_Read_XY24(int fd_tch, uchar xy) {
    uint16 i, j;
    uint16 buf[READ_TIMES];
    uint16 sum = 0;
    uint16 temp;
    for (i = 0; i < READ_TIMES; i++) {
        buf[i] = Touch_ads7843_Read_AD24(fd_tch, xy);
    }
    for (i = 0; i < READ_TIMES - 1; i++) { //排序
        for (j = i + 1; j < READ_TIMES; j++) {
            BUBBLE_SWAP(buf[i],buf[j]); //升序排列
        }
    }
    sum = 0;
    for (i = LOST_VAL; i < READ_TIMES - LOST_VAL; i++)sum += buf[i];
    temp = sum / (READ_TIMES - 2 * LOST_VAL);
    return temp;
}

////////////////////////////////////////////////////////////////////////////
static uint Touch_ads7843_Read_AD40(int fd_tch, uint16 *x, uint16 *y) {
    if (fd_tch >= 0) { //without LCD_CTRL_BIT_READ
        uchar tx[5] = {CMD_RDX, 0x00, CMD_RDY, 0x00, 0x00};

        if (Spi_transfer(fd_tch, tx, ARRAY_SIZE(tx), SPI_TCH_DEV_NO) == 5) {
            uchar* rx = Spi_get_rx_buff();
            *x = TCH_COMBIN(rx[1],rx[2]);
            *y = TCH_COMBIN(rx[3],rx[4]);
            return 1;   //only 12 bits(LSB) validate
        }
    }
    return 0;
}

//读取一个坐标值
//连续读取READ_TIMES次数据,对这些数据升序排列,
//然后去掉最低和最高LOST_VAL个数,取平均值 
#define READ_TIMES 15 //读取次数
#define LOST_VAL 5	  //丢弃值

static uint16 Touch_ads7843_Read_XY40(int fd_tch, uint16 *x, uint16 *y) {
    uint16 i, j;
    uint16 buf_x[READ_TIMES];
    uint16 buf_y[READ_TIMES];
    uint16 sum_x = 0;
    uint16 sum_y = 0;
    for (i = 0; i < READ_TIMES; i++) {
        if (!Touch_ads7843_Read_AD40(fd_tch, &buf_x[i], &buf_y[i])) return 0;
    }
    for (i = 0; i < READ_TIMES - 1; i++) { //排序
        for (j = i + 1; j < READ_TIMES; j++) {
            BUBBLE_SWAP(buf_x[i],buf_x[j]); //升序排列
            BUBBLE_SWAP(buf_y[i],buf_y[j]); //升序排列
        }
    }
    for (i = LOST_VAL; i < READ_TIMES - LOST_VAL; i++) {
        sum_x += buf_x[i];
        sum_y += buf_y[i];
    }
    *x = sum_x / (READ_TIMES - 2 * LOST_VAL);
    *y = sum_y / (READ_TIMES - 2 * LOST_VAL);
    return 1;
}

#define TCH_LEN_OF_QUERY    (READ_TIMES * 4 + 2)

static uint Touch_ads7843_Read_XY32s(int fd_tch, uint16 *x, uint16 *y) {
    uint16 i, j;
    uint16 buf_x[READ_TIMES];
    uint16 buf_y[READ_TIMES];
    uint16 sum_x = 0;
    uint16 sum_y = 0;
    uchar tx[TCH_LEN_OF_QUERY];
    
    memset (tx, 0x00, TCH_LEN_OF_QUERY);
    for (i = 0; i < READ_TIMES; i++) {
        tx[i * 4 + 0] = CMD_RDX;
        tx[i * 4 + 2] = CMD_RDY;
    }
    if (Spi_transfer(fd_tch, tx, ARRAY_SIZE(tx), SPI_TCH_DEV_NO) == TCH_LEN_OF_QUERY) {
        uchar* rx = Spi_get_rx_buff();
        for (i = 0; i < READ_TIMES; i++) {
            buf_x[i] = TCH_COMBIN(rx[i * 4 + 1],rx[i * 4 + 2]);
            buf_y[i] = TCH_COMBIN(rx[i * 4 + 3],rx[i * 4 + 4]);
        }
    }
    for (i = 0; i < READ_TIMES - 1; i++) { //排序
        for (j = i + 1; j < READ_TIMES; j++) {
            BUBBLE_SWAP(buf_x[i],buf_x[j]); //升序排列
            BUBBLE_SWAP(buf_y[i],buf_y[j]); //升序排列
        }
    }
    for (i = LOST_VAL; i < READ_TIMES - LOST_VAL; i++) {
        sum_x += buf_x[i];
        sum_y += buf_y[i];
    }
    *x = sum_x / (READ_TIMES - 2 * LOST_VAL);
    *y = sum_y / (READ_TIMES - 2 * LOST_VAL);
    return 1;
}

////////////////////////////////////////////////////////////////////////////
//带滤波的坐标读取
//最小值不能少于100.

static uchar Touch_ads7843_Read24(int fd_tch, uint16 *x, uint16 *y) {
    uint16 xtemp, ytemp;
    xtemp = Touch_ads7843_Read_XY24(fd_tch, CMD_RDX);
    ytemp = Touch_ads7843_Read_XY24(fd_tch, CMD_RDY);
    if (xtemp < 100 || ytemp < 100)return 0; //读数失败
    *x = xtemp;
    *y = ytemp;
    return 1; //读数成功
}

static uchar Touch_ads7843_Read32s(int fd_tch, uint16 *x, uint16 *y) {
    uint16 xtemp, ytemp;
    if (!Touch_ads7843_Read_XY32s(fd_tch, &xtemp, &ytemp)) return 0;
    if (xtemp < 100 || ytemp < 100)return 0; //读数失败
    *x = xtemp;
    *y = ytemp;
    return 1; //读数成功
}
//2次读取ADS7846,连续读取2次有效的AD值,且这两次的偏差不能超过
//50,满足条件,则认为读数正确,否则读数错误.	   
//该函数能大大提高准确度
#define ERR_RANGE 20 //误差范围 

static uchar Touch_ads7843_Read2_24(int fd_tch, uint16 *x, uint16 *y) {
    uint16 x1, y1;
    uint16 x2, y2;
    uchar flag;
    flag = Touch_ads7843_Read24(fd_tch, &x1, &y1);
    if (flag == 0)return (0);
    flag = Touch_ads7843_Read24(fd_tch, &x2, &y2);
    if (flag == 0)return (0);
    if (((x2 <= x1 && x1 < x2 + ERR_RANGE) || (x1 <= x2 && x2 < x1 + ERR_RANGE))//前后两次采样在+-ERR_RANGE内
            &&((y2 <= y1 && y1 < y2 + ERR_RANGE) || (y1 <= y2 && y2 < y1 + ERR_RANGE))) {
        *x = (x1 + x2) >> 1;
        *y = (y1 + y2) >> 1;
        return 1;
    } else return 0;
}
//精确读取一次坐标,校准的时候用的	   

#define WYY_DEBUG(fmt,...)   do {\
    time_t  moment = time(&moment);\
    printf("<%d>    %s(%d)  " fmt "\n",moment,__func__,__LINE__,##__VA_ARGS__);\
    } while (0)

static char static_cursor[] = "\\|/-\\|/-";
static uchar static_nCurStat;
const int TCH_LEN_CURSOR_STATE = sizeof(static_cursor);

static uchar Touch_ads7843_Read2_32s(int fd_tch, uint16 *x, uint16 *y) {
    uint16 x1, y1;
    uint16 x2, y2;
    uchar flag;
    flag = Touch_ads7843_Read32s(fd_tch, &x1, &y1);
    if (flag == 0)return (0);
    flag = Touch_ads7843_Read32s(fd_tch, &x2, &y2);
    if (flag == 0)return (0);
    if (Spi_get_debug() & SPI_DEBUG_CH2_BUSY) {
        int n = (static_nCurStat++) % TCH_LEN_CURSOR_STATE;
        printf(VT100_GOTO("25","71") VT100_STYLE_BLUE "%c\n" VT100_RESET, static_cursor[n]);
        fflush(stdout);
    }
    if (((x2 <= x1 && x1 < x2 + ERR_RANGE) || (x1 <= x2 && x2 < x1 + ERR_RANGE))//前后两次采样在+-ERR_RANGE内
            &&((y2 <= y1 && y1 < y2 + ERR_RANGE) || (y1 <= y2 && y2 < y1 + ERR_RANGE))) {
        *x = (x1 + x2) >> 1;
        *y = (y1 + y2) >> 1;
        return 1;
    } else return 0;
}
//精确读取一次坐标,校准的时候用的	   

static uchar Touch_ads7843_Read_TP_Once(int fd_tch) {
    uchar re = 0;
    uint16 x1, y1;
    WYY_DEBUG("%s","Begin++++++++++++++++");
    while (re == 0) {
//        while (!Touch_ads7843_Read2(fd_tch, &ads7843_tp_pixad.x, &ads7843_tp_pixad.y));
        while (!Touch_ads7843_Read2_32s(fd_tch, &ads7843_tp_pixad.x, &ads7843_tp_pixad.y));
//        delayms(10);
//        while (!Touch_ads7843_Read2(fd_tch, &x1, &y1));
        while (!Touch_ads7843_Read2_32s(fd_tch, &x1, &y1));
        if (ads7843_tp_pixad.x == x1 && ads7843_tp_pixad.y == y1) {
            re = 1;
        }
    }
    WYY_DEBUG("x=%d, y=%d",x1,y1);
    return re;
}
//////////////////////////////////////////////////
//与LCD部分有关的函数  
//画一个触摸点
//用来校准用的

static void Touch_ads7843_Draw_Cross(int fd_lcd, uint16 x, uint16 y,uint16 color) {
    lcd_ili9341_POINT_COLOR = color;
    Lcd_ili9341_DrawLine(fd_lcd, x - 12, y, x + 13, y); //横线
    Lcd_ili9341_DrawLine(fd_lcd, x, y - 12, x, y + 13); //竖线
    Lcd_ili9341_DrawPoint(fd_lcd, x + 1, y + 1);
    Lcd_ili9341_DrawPoint(fd_lcd, x - 1, y + 1);
    Lcd_ili9341_DrawPoint(fd_lcd, x + 1, y - 1);
    Lcd_ili9341_DrawPoint(fd_lcd, x - 1, y - 1);
}

static void Touch_ads7843_Drow_Touch_Point(int fd_lcd, uint16 x, uint16 y) {
    Lcd_ili9341_DrawLine(fd_lcd, x - 12, y, x + 13, y); //横线
    Lcd_ili9341_DrawLine(fd_lcd, x, y - 12, x, y + 13); //竖线
    Lcd_ili9341_DrawPoint(fd_lcd, x + 1, y + 1);
    Lcd_ili9341_DrawPoint(fd_lcd, x - 1, y + 1);
    Lcd_ili9341_DrawPoint(fd_lcd, x + 1, y - 1);
    Lcd_ili9341_DrawPoint(fd_lcd, x - 1, y - 1);
    //	Draw_Circle(x,y,6);//画中心圈
}
//转换结果
//根据触摸屏的校准参数来决定转换后的结果,保存在X0,Y0中

static uchar Touch_ads7843_Convert_Pos(int fd_tch) {
    uchar l = 0;
//    if (Touch_ads7843_Read2(fd_tch, &ads7843_tp_pixad.x, &ads7843_tp_pixad.y)) {
    if (Touch_ads7843_Read2_32s(fd_tch, &ads7843_tp_pixad.x, &ads7843_tp_pixad.y)) {
        l = 1;
        ads7843_tp_pixlcd.x = ads7843_tp_pixad.x > ads7843_chx ?
                            ((uint) ads7843_tp_pixad.x - (uint) ads7843_chx)*1000 / ads7843_vx :
                            ((uint) ads7843_chx - (uint) ads7843_tp_pixad.x)*1000 / ads7843_vx;
        ads7843_tp_pixlcd.y = ads7843_tp_pixad.y > ads7843_chy ?
                            ((uint) ads7843_tp_pixad.y - (uint) ads7843_chy)*1000 / ads7843_vy :
                            ((uint) ads7843_chy - (uint) ads7843_tp_pixad.y)*1000 / ads7843_vy;
    }
    return l;
}
//触摸屏校准代码
//得到四个校准参数
#define BORDER_WIDE 50   //校准坐标偏移量	
#define tp_xiaozhun 1000   //校准精度
#define BC                  lcd_ili9341_BACK_COLOR
#define FC                  LCD_ILI9341_RED

static void Touch_ads7843_Adjust_Info(int fd_lcd, uint x, uint y, uint vx, uint vy, uint chx, uint chy, uint no) {
    uint lx = 0;
    uint ly = 50;
    char sInfo[64];
    
    memset (sInfo, 0x00, 64);
    sprintf(sInfo, "VX%d:", no);
    Lcd_ili9341_ShowString(fd_lcd, lx, ly, 16, 8, sInfo);
    lx += 40;
    Lcd_ili9341_ShowNum(fd_lcd, lx, ly, vx, 5);
    lx = 0;
    ly += 20;
    sprintf(sInfo, "VY%d:", no);
    Lcd_ili9341_ShowString(fd_lcd, lx, ly, 16, 8, sInfo);
    lx += 40;
    Lcd_ili9341_ShowNum(fd_lcd, lx, ly, vy, 5);
    lx = 0;
    ly += 20;
    sprintf(sInfo, "CHX%d:", no);
    Lcd_ili9341_ShowString(fd_lcd, lx, ly, 16, 8, sInfo);
    lx += 40;
    Lcd_ili9341_ShowNum(fd_lcd, lx, ly, chx, 5);
    lx = 0;
    ly += 20;
    sprintf(sInfo, "CHY%d:", no);
    Lcd_ili9341_ShowString(fd_lcd, lx, ly, 16, 8, sInfo);
    lx += 40;
    Lcd_ili9341_ShowNum(fd_lcd, lx, ly, chy, 5);
    
    if (no > 2) {
        lx = 30;
        ly += 30;
        Lcd_ili9341_ShowString(fd_lcd, lx, ly, 32, 16, "Adjust Done!");
        ly += 30;
        Lcd_ili9341_ShowString(fd_lcd, lx, ly, 32, 16, "Touch anywhere to Exit");
    }
}

//void Touch_ads7843_Adjust(int fd_lcd, int fd_tch) {
//    float vx1, vx2, vy1, vy2; //比例因子，此值除以1000之后表示多少个AD值代表一个像素点
//    uint16 chx1, chx2, chy1, chy2; //默认像素点坐标为0时的AD起始值
//    uint16 lx, ly;
//    struct ADS7843_TP_PIX32_ p[4];
//    uchar cnt = 0;
//
//    lcd_ili9341_BACK_COLOR = LCD_ILI9341_WHITE;
//    Lcd_ili9341_Clear(fd_lcd, LCD_ILI9341_WHITE); //清屏
//    Touch_ads7843_Draw_Cross(fd_lcd, BORDER_WIDE, BORDER_WIDE,FC); //画点1 
//    Touch_ads7843_Read_AD40(fd_tch, &lx, &ly);
//    WYY_DEBUG("Waiting for point [%d]",cnt + 1);
//    while (1) {
//        if (SPI_TCH_KEY_PRESSED == Touch_ads7843_Key(fd_tch)) {//按键按下了
////            lx = Touch_ads7843_Read_XY(fd_tch, CMD_RDX);
////            ly = Touch_ads7843_Read_XY(fd_tch, CMD_RDY);
////            printf("\nClicked [%d,%d]\n",lx,ly);
//            if (Touch_ads7843_Read_TP_Once(fd_tch)) { //得到单次按键值
//                p[cnt].x = ads7843_tp_pixad.x;
//                p[cnt].y = ads7843_tp_pixad.y;
//                printf("\nClicked [%d,%d]\n",p[cnt].x,p[cnt].y);
//                cnt++;
//            }
//            switch (cnt) {
//                case 1:
//                    Touch_ads7843_Draw_Cross(fd_lcd, BORDER_WIDE, BORDER_WIDE,BC); //画点1 
//                    while (SPI_TCH_KEY_RELEASED != Touch_ads7843_Key(fd_tch));//等待松手
//                    Touch_ads7843_Draw_Cross(fd_lcd, LCD_ILI9341_WIDE - BORDER_WIDE - 1, BORDER_WIDE,FC); //画点2
//                    break;
//                case 2:
//                    Touch_ads7843_Draw_Cross(fd_lcd, LCD_ILI9341_WIDE - BORDER_WIDE - 1, BORDER_WIDE,BC); //画点2
//                    while (SPI_TCH_KEY_RELEASED != Touch_ads7843_Key(fd_tch));//等待松手
//                    Touch_ads7843_Draw_Cross(fd_lcd, BORDER_WIDE, LCD_ILI9341_HIGH - BORDER_WIDE - 1,FC); //画点3
//                    break;
//                case 3:
//                    Touch_ads7843_Draw_Cross(fd_lcd, BORDER_WIDE, LCD_ILI9341_HIGH - BORDER_WIDE - 1,BC); //画点3
//                    while (SPI_TCH_KEY_RELEASED != Touch_ads7843_Key(fd_tch));//等待松手
//                    Touch_ads7843_Draw_Cross(fd_lcd, LCD_ILI9341_WIDE - BORDER_WIDE - 1, LCD_ILI9341_HIGH - BORDER_WIDE - 1,FC); //画点4
//                    break;
//                case 4: //全部四个点已经得到
//                    Touch_ads7843_Draw_Cross(fd_lcd, LCD_ILI9341_WIDE - BORDER_WIDE - 1, LCD_ILI9341_HIGH - BORDER_WIDE - 1,BC); //画点4
//                    while (SPI_TCH_KEY_RELEASED != Touch_ads7843_Key(fd_tch));//等待松手
//                    vx1 = p[1].x > p[0].x ? (p[1].x - p[0].x + 1)*1000 / (LCD_ILI9341_WIDE - BORDER_WIDE - BORDER_WIDE) : (p[0].x - p[1].x - 1)*1000 / (LCD_ILI9341_WIDE - BORDER_WIDE - BORDER_WIDE);
//                    chx1 = p[1].x > p[0].x ? p[0].x - (vx1 * BORDER_WIDE) / 1000 : p[0].x + (vx1 * BORDER_WIDE) / 1000;
//                    vy1 = p[2].y > p[0].y ? (p[2].y - p[0].y - 1)*1000 / (LCD_ILI9341_HIGH - BORDER_WIDE - BORDER_WIDE) : (p[0].y - p[2].y - 1)*1000 / (LCD_ILI9341_HIGH - BORDER_WIDE - BORDER_WIDE);
//                    chy1 = p[2].y > p[0].y ? p[0].y - (vy1 * BORDER_WIDE) / 1000 : p[0].y + (vy1 * BORDER_WIDE) / 1000;
//
//                    vx2 = p[3].x > p[2].x ? (p[3].x - p[2].x + 1)*1000 / (LCD_ILI9341_WIDE - BORDER_WIDE - BORDER_WIDE) : (p[2].x - p[3].x - 1)*1000 / (LCD_ILI9341_WIDE - BORDER_WIDE - BORDER_WIDE);
//                    chx2 = p[3].x > p[2].x ? p[2].x - (vx2 * BORDER_WIDE) / 1000 : p[2].x + (vx2 * BORDER_WIDE) / 1000;
//                    vy2 = p[3].y > p[1].y ? (p[3].y - p[1].y - 1)*1000 / (LCD_ILI9341_HIGH - BORDER_WIDE - BORDER_WIDE) : (p[1].y - p[3].y - 1)*1000 / (LCD_ILI9341_HIGH - BORDER_WIDE - BORDER_WIDE);
//                    chy2 = p[3].y > p[1].y ? p[1].y - (vy2 * BORDER_WIDE) / 1000 : p[1].y + (vy2 * BORDER_WIDE) / 1000;
//
//
//                    if ((vx1 > vx2 && vx1 > vx2 + tp_xiaozhun) || (vx1 < vx2 && vx1 < vx2 - tp_xiaozhun) || (vy1 > vy2 && vy1 > vy2 + tp_xiaozhun) || (vy1 < vy2 && vy1 < vy2 - tp_xiaozhun)) {
//                        cnt = 0;
//                        Lcd_ili9341_Clear(fd_lcd, LCD_ILI9341_WHITE); //清屏 
//                        Touch_ads7843_Draw_Cross(fd_lcd, BORDER_WIDE, BORDER_WIDE,FC); //画点1 
//                        continue;
//                    }
//                    ads7843_vx = (vx1 + vx2) / 2;
//                    ads7843_vy = (vy1 + vy2) / 2;
//                    ads7843_chx = (chx1 + chx2) / 2;
//                    ads7843_chy = (chy1 + chy2) / 2;
//
//                    //显示校准信息
//                    Lcd_ili9341_Clear(fd_lcd, LCD_ILI9341_WHITE); //清屏 
//                    lcd_ili9341_POINT_COLOR = LCD_ILI9341_RED;
//                    lcd_ili9341_BACK_COLOR = LCD_ILI9341_WHITE;
//
//                    Touch_ads7843_Adjust_Info(fd_lcd,   0,  50, vx1, vy1, chx1, chy1, 1);
//                    Touch_ads7843_Adjust_Info(fd_lcd, 100,  50, vx2, vy2, chx2, chy2, 2);
//                    Touch_ads7843_Adjust_Info(fd_lcd,  50, 150, ads7843_vx, ads7843_vx, ads7843_chx, ads7843_chy, 2);
//
//                    lx = 30;
//                    ly += 30;
//                    Lcd_ili9341_ShowString(fd_lcd, lx, ly, "Adjust Done!");
//                    ly += 30;
//                    Lcd_ili9341_ShowString(fd_lcd, lx, ly, "Touch anywhere to Exit");
//                    Touch_ads7843_Read_TP_Once(fd_tch); //等待任意键后继续
//
//                    Lcd_ili9341_Clear(fd_lcd, LCD_ILI9341_WHITE); //清屏
//                    return; //校正完成				 
//            }
//        }
//    }
//}

static struct ADS7843_TP_PIX32_     static_cross[4] = {
    {BORDER_WIDE                        ,BORDER_WIDE},
    {LCD_ILI9341_WIDE - BORDER_WIDE - 1 ,BORDER_WIDE},
    {BORDER_WIDE                        ,LCD_ILI9341_HIGH - BORDER_WIDE - 1},
    {LCD_ILI9341_WIDE - BORDER_WIDE - 1 ,LCD_ILI9341_HIGH - BORDER_WIDE - 1},
};
static struct ADS7843_TP_PIX32_     static_p[4];
static int Touch_ads7843_Wait_for_Cross(int fd_lcd, int fd_tch, int pt_no, uint16 *x0, uint16 *y0) {
    uint16 cnt = pt_no % 4;
    uint16 x1 = static_cross[cnt].x;
    uint16 y1 = static_cross[cnt].y;
    if (0 == cnt) {
        lcd_ili9341_BACK_COLOR = LCD_ILI9341_WHITE;
        Lcd_ili9341_Clear(fd_lcd, LCD_ILI9341_WHITE); //����
    }
    if (*x0 && *y0) {
        Touch_ads7843_Draw_Cross(fd_lcd, *x0, *y0,BC); //画点 old
    }
    if (x1 && y1) {
        Touch_ads7843_Draw_Cross(fd_lcd, x1, y1,FC); //画点 new
    }
    WYY_DEBUG("\nWaiting for click point [%d]", pt_no + 1);
    while (1) {
        if (SPI_TCH_KEY_PRESSED == Touch_ads7843_Key(fd_tch)) {//按键按下了
            if (Touch_ads7843_Read_TP_Once(fd_tch)) { //得到单次按键值
                static_p[cnt].x = ads7843_tp_pixad.x;
                static_p[cnt].y = ads7843_tp_pixad.y;
                printf("\nClicked [%d,%d]\n",static_p[cnt].x,static_p[cnt].y);
            }
            while (SPI_TCH_KEY_RELEASED != Touch_ads7843_Key(fd_tch));//等待松手
            *x0 = x1;
            *y0 = y1;
            return (pt_no + 1);
        }
    }
}

unsigned char Touch_ads7843_wait_press(int fd_tch, uint16 *x, uint16 *y) {
    while (1) {
        if (SPI_TCH_KEY_PRESSED == Touch_ads7843_Key(fd_tch)) {//����������
            if (Touch_ads7843_Read_TP_Once(fd_tch)) { //�õ����ΰ���ֵ
                *x = ads7843_tp_pixad.x;
                *y = ads7843_tp_pixad.y;
                return 1;
            }
            return 0;
        }
    }
}

unsigned char Touch_ads7843_wait_click(int fd_tch, uint16 *x, uint16 *y) {
    uint16  lx,ly;
    for (Touch_ads7843_Read_AD40(fd_tch, &lx, &ly); 1; usleep(10000)) {
        if (SPI_TCH_KEY_PRESSED == Touch_ads7843_Key(fd_tch)) {//����������
            if (Touch_ads7843_Read_TP_Once(fd_tch)) { //�õ����ΰ���ֵ
                while (SPI_TCH_KEY_RELEASED != Touch_ads7843_Key(fd_tch)) usleep(10000);//�ȴ�����
                *x = (3800 < ads7843_tp_pixad.x) ? 0 : (3800 - ads7843_tp_pixad.x) / 14;
                *y = (ads7843_tp_pixad.y - 400) / 10;
                printf("\nClicked [%d,%d]\n",*x,*y);
                return 1;
            }
            return 0;
        }
    }
}

static int Touch_ads7843_calculating(int fd_lcd, int fd_tch, struct ADS7843_TP_PIX32_ *p) {
    float vx1, vx2, vy1, vy2; //比例因子，此值除以1000之后表示多少个AD值代表一个像素点
    uint16 chx1, chx2, chy1, chy2; //默认像素点坐标为0时的AD起始值

    vx1 = p[1].x > p[0].x ? (p[1].x - p[0].x + 1)*1000 / (LCD_ILI9341_WIDE - BORDER_WIDE - BORDER_WIDE) :
                            (p[0].x - p[1].x - 1)*1000 / (LCD_ILI9341_WIDE - BORDER_WIDE - BORDER_WIDE);
    chx1 = p[1].x > p[0].x ? p[0].x - (vx1 * BORDER_WIDE) / 1000 : p[0].x + (vx1 * BORDER_WIDE) / 1000;
    vy1 = p[2].y > p[0].y ? (p[2].y - p[0].y - 1)*1000 / (LCD_ILI9341_HIGH - BORDER_WIDE - BORDER_WIDE) :
                            (p[0].y - p[2].y - 1)*1000 / (LCD_ILI9341_HIGH - BORDER_WIDE - BORDER_WIDE);
    chy1 = p[2].y > p[0].y ? p[0].y - (vy1 * BORDER_WIDE) / 1000 : p[0].y + (vy1 * BORDER_WIDE) / 1000;

    vx2 = p[3].x > p[2].x ? (p[3].x - p[2].x + 1)*1000 / (LCD_ILI9341_WIDE - BORDER_WIDE - BORDER_WIDE) :
                            (p[2].x - p[3].x - 1)*1000 / (LCD_ILI9341_WIDE - BORDER_WIDE - BORDER_WIDE);
    chx2 = p[3].x > p[2].x ? p[2].x - (vx2 * BORDER_WIDE) / 1000 : p[2].x + (vx2 * BORDER_WIDE) / 1000;
    vy2 = p[3].y > p[1].y ? (p[3].y - p[1].y - 1)*1000 / (LCD_ILI9341_HIGH - BORDER_WIDE - BORDER_WIDE) :
                            (p[1].y - p[3].y - 1)*1000 / (LCD_ILI9341_HIGH - BORDER_WIDE - BORDER_WIDE);
    chy2 = p[3].y > p[1].y ? p[1].y - (vy2 * BORDER_WIDE) / 1000 : p[1].y + (vy2 * BORDER_WIDE) / 1000;


    if ((vx1 > vx2 && vx1 > vx2 + tp_xiaozhun) || (vx1 < vx2 && vx1 < vx2 - tp_xiaozhun) ||
        (vy1 > vy2 && vy1 > vy2 + tp_xiaozhun) || (vy1 < vy2 && vy1 < vy2 - tp_xiaozhun)) {
        printf( VT100_STYLE_ALERT "Fail  p0 = [%d,%d]\n", p[0].x, p[0].y);
        printf( VT100_STYLE_ALERT "Fail  p1 = [%d,%d]\n", p[1].x, p[1].y);
        printf( VT100_STYLE_ALERT "Fail  p2 = [%d,%d]\n", p[2].x, p[2].y);
        printf( VT100_STYLE_ALERT "Fail  p3 = [%d,%d]\n", p[3].x, p[3].y);
        printf( VT100_STYLE_ALERT "Fail  VXY1 = [%lf,%lf], VXY2 = [%lf,%lf]\n" VT100_RESET, vx1,vy1,vx2,vy2);
        return 0; //Adjust again
    }
    ads7843_vx = (vx1 + vx2) / 2;
    ads7843_vy = (vy1 + vy2) / 2;
    ads7843_chx = (chx1 + chx2) / 2;
    ads7843_chy = (chy1 + chy2) / 2;

    //显示校准信息  display result
    Lcd_ili9341_Clear(fd_lcd, LCD_ILI9341_WHITE); //清屏 
    lcd_ili9341_POINT_COLOR = LCD_ILI9341_RED;
    lcd_ili9341_BACK_COLOR = LCD_ILI9341_WHITE;
    printf( VT100_STYLE_ALERT "OK    VXY1 = [%lf,%lf], VXY2 = [%d,%d]\n" VT100_RESET,vx1,vy1,vx2,vy2);

    Touch_ads7843_Adjust_Info(fd_lcd,   0,  50, vx1, vy1, chx1, chy1, 1);
    Touch_ads7843_Adjust_Info(fd_lcd, 100,  50, vx2, vy2, chx2, chy2, 2);
    Touch_ads7843_Adjust_Info(fd_lcd,  50, 150, ads7843_vx, ads7843_vx, ads7843_chx, ads7843_chy, 3);
//    while (SPI_TCH_KEY_CLICKED != Touch_ads7843_Key(fd_tch));//等待松手
    return 1;
}


void Touch_ads7843_Adjust(int fd_lcd, int fd_tch) {
    uint16 lx = 0, ly = 0;
    uint16 x0 = 0, y0 = 0;
    uchar cnt = 0;

    Touch_ads7843_Draw_Cross(fd_lcd, BORDER_WIDE, BORDER_WIDE,FC); //画点1 
    Touch_ads7843_Read_AD40(fd_tch, &lx, &ly);
    while (1) {
        cnt = Touch_ads7843_Wait_for_Cross(fd_lcd, fd_tch, cnt, &x0, &y0);
        switch (cnt) {
            case 4: //全部四个点已经得到
                if (cnt = Touch_ads7843_calculating(fd_lcd, fd_tch, static_p)) {
                    Touch_ads7843_Read_TP_Once(fd_tch);
                    Lcd_ili9341_Clear(fd_lcd, LCD_ILI9341_WHITE); //清屏
                    return; //校正完成
                }
            default:;
        }
    }
}
#undef  BC
#undef  FC

void Touch_ads7843_point(int fd_lcd, int fd_tch) //绘图函数
{
    double t = 0;

    while (1) {
        if (!Touch_ads7843_Key(fd_tch)) {
            t = 0;
            if (Touch_ads7843_Convert_Pos(fd_tch)) //得到坐标值
            {
                //	Lcd_ili9341_ShowString(fd_lcd, 10,250,"X:");Lcd_ili9341_ShowNum(fd_lcd, 30,250,(uint)ads7843_tp_pixad.x,6);	
                //Lcd_ili9341_ShowString(fd_lcd, 180,250,"Y:");Lcd_ili9341_ShowNum(fd_lcd, 200,250,(uint)ads7843_tp_pixad.y,6);	
                Lcd_ili9341_ShowString(fd_lcd, 10, 250, 16, 8, "X:");
                Lcd_ili9341_ShowNum(fd_lcd, 30, 250, ads7843_tp_pixad.x, 4);
                Lcd_ili9341_ShowString(fd_lcd, 180, 250, 16, 8, "Y:");
                Lcd_ili9341_ShowNum(fd_lcd, 200, 250, ads7843_tp_pixad.y, 4);
                Lcd_ili9341_DrawPoint_big(fd_lcd, ads7843_tp_pixlcd.x, ads7843_tp_pixlcd.y);
            }

        } else {
            t++;
            if (t > 65000) {
                return;
            }
        }

    }
}


