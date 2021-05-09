
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
#include <stdarg.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include <signal.h>
#include <bits/signum.h>
#include <fcntl.h>
#include <sys/msg.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <termios.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>
#include <errno.h>

#include "lcd_ili9341.h"
#include "lcd_ili9341_font.h"

static const char *static_device_lcd = "/dev/spidev1.0";
static const char *static_device_touch = "/dev/spidev1.1";
static pthread_mutex_t static_hHoldScreen;
static int static_nPrinterID = -1;
static int static_nPrinterTimer = 0;
static int static_nUpdateTimer = 0;
static bool static_rf_updated = false;
static bool static_iw_updated = false;
static unsigned char nCurrentPage = PAGE_0;
static unsigned char nDisplaySchema = NF_DISP_STYLE_THIN_BAR;
static int static_fd_lcd = -1;
static int static_fd_tch = -1;

#define LCD_TYPE_NONE       0x00
#define LCD_TYPE_OMCJ4X8C   0x01
#define LCD_TYPE_ILI9341    0x02

//#define LCD_TYPE    LCD_TYPE_OMCJ4X8C
//#define LCD_TYPE    LCD_TYPE_ILI9341
#define LCD_TYPE    LCD_TYPE_NONE

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

//static int static_icon_row = 1;
//static int static_icon_col;
//static void Lcd_ili9341_demo(int fd_lcd, int fd_tch) {
//    int x0 = 0;
//    int y0 = 0;
//    uint16 x = 0;
//    uint16 y = 0;
//    uint16 nImg = 0;
//    LCD_Image*  img[8] = {  &lcd_image_lcd_ili9341_image_logo,
//                            &lcd_image_lcd_ili9341_image_drop,
//                            &lcd_image_lcd_ili9341_image_girl1,
//                            &lcd_image_lcd_ili9341_image_girl2,
//                            &lcd_image_lcd_ili9341_image_girl3,
//                            &lcd_image_lcd_ili9341_image_lemon,
//                            &lcd_image_lcd_ili9341_image_tiger,
//                            &lcd_image_lcd_ili9341_image_sea};
//
//    do {
//        if (0 == nImg) {
//            lcd_ili9341_BACK_COLOR = LCD_ILI9341_BLACK;
//            lcd_ili9341_POINT_COLOR = LCD_ILI9341_WHITE;
//            Lcd_ili9341_Init(fd_lcd); //tft��ʼ��
//            Lcd_ili9341_Clear(fd_lcd, LCD_ILI9341_BLACK); //����
//        }
//        Lcd_ili9341_ShowIcon(fd_lcd, x0, y0,
//                img[nImg]->m_width + x0,
//                img[nImg]->m_hight + y0,
//                img[nImg]->m_image);
//        lcd_ili9341_POINT_COLOR = LCD_ILI9341_RED;
//        Lcd_ili9341_ShowChinese(fd_lcd, 20, 100, &lcd_brandname_a,FALSE);
//        lcd_ili9341_POINT_COLOR = LCD_ILI9341_MAGENTA;
//        Lcd_ili9341_ShowChinese(fd_lcd, 20, 135, &lcd_brandname_d,FALSE);
//        lcd_ili9341_POINT_COLOR = LCD_ILI9341_WHITE;
//        Lcd_ili9341_ShowString(fd_lcd, 5, 280, "X:");
//        Lcd_ili9341_ShowNum(fd_lcd, 25, 280, x, 3);
//        Lcd_ili9341_ShowString(fd_lcd, 5, 302, "Y:");
//        Lcd_ili9341_ShowNum(fd_lcd, 25, 302, y, 3);
//        if (Touch_ads7843_wait_click(fd_tch, &x, &y)) {
//            nImg ++;
//            nImg %= 8;
//            continue;
//        }
//    } while (1);
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

static char static_strPrompt[128];
static char* lcdGetTimeString() {
    static char strTime[64];
    static bool disp_reset;
    static time_t last_seconds;
    struct timeval curr_timeval;
    struct timezone tz;
    gettimeofday(&curr_timeval,&tz);
    time_t current_time = curr_timeval.tv_sec;
    time_t msec = curr_timeval.tv_usec / 10000;

    if (0 == msec % 10) {
        time_t day_seconds = current_time % 86400;
        time_t hour = day_seconds / 3600;
        time_t hour_seconds = day_seconds % 3600;
        time_t minute = hour_seconds / 60;
        time_t seconds = hour_seconds % 60;
        if (last_seconds != seconds) {
            last_seconds = seconds;
            disp_reset = !disp_reset;
        }
        memset (strTime,0x00,64);
        sprintf(strTime,"%02d:%02d:%02d.%d", hour, minute, seconds,msec/10);
        return strTime;
    }
    return NULL;
}

/*
    if (KeyOpen(&keyb, false)) {
    } else {
        ShowStatusBar("%s", "Can not open radio COM port.");
        return 0;
    }
*/
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

#define GWS_VB_LOAD(index,type,address)     \
        if (NULL != (pVar = GetGwsVB(index)))     \
            VB_Load(&pVar->m_value,type,address);
//        else printf("\nGWS Varbind not found(index=%d)\n",index)

#define IW_VB_LOAD(index,type,address)     \
        if (NULL != (pVar = GetIwVB(index)))     \
            VB_Load(&pVar->m_value,type,address);
//        else printf("\nWifi Varbind not found(index=%d)\n",index)

static void lcdPromptGet(pthread_mutex_t* hMutex, char* prompt) {
    if (0 == pthread_mutex_trylock(hMutex)) {
        if (prompt)
            strcpy (prompt,static_strPrompt);
        pthread_mutex_unlock(hMutex);
    }
}

static char* lcdPickupUserInput() {
    static char buff[128];
    
    memset (buff,0x00,128);
    if (fgets(buff, 127, stdin)) {
        char* ln = strchr(buff,'\n');
        if (ln && ln >= buff) ln[0] = 0;
        return buff;
    }
    return NULL;
}

static char* lcdShowCommandLine(pthread_mutex_t* hMutex, bool bNeedInput) {
    char *sTime = lcdGetTimeString();
    char prompt[128];
    memset (prompt,0x00,128);
    lcdPromptGet(hMutex,prompt);
    if (0 == pthread_mutex_trylock(hMutex)) {
        char* input = NULL;
        if (bNeedInput) sTime = "Waiting...";
        if (sTime) {
            unsigned int nIP = Socket_GetIP("br-lan");
            lcd_printf32(  0, 0, 7,LCD_ILI9341_CYAN,LCD_ILI9341_BLACK, "GH0228");
            lcd_printf(120, 0,24,LCD_ILI9341_BLACK,LCD_ILI9341_GREEN, "%s",IPconvertN2A(nIP));
            lcd_printf(250, 0, 9,LCD_ILI9341_RED,LCD_ILI9341_BLACK, "%s",sTime);
        }
        if (bNeedInput) input = lcdPickupUserInput();
        pthread_mutex_unlock(hMutex);
        return input;
    }
    return NULL;
}

static void lcdPrint(VarBind* vb, int colWidth) {    
    if (NULL == vb || 0 == vb->m_k) return;
    IW_LIST* l = NULL;
    uint16  row = (vb->m_x + 1) * 16;
    uint16  col = vb->m_y * 8;
    uint16  val = (vb->m_y + colWidth) * 8;
//    printf("row = %d, col = %d, %s\n",vb->m_x,vb->m_y,vb->m_title);
    switch (BindValueType(&vb->m_value)) {
        case VAR_BOOL:
        case VAR_STRING:
            lcd_printf(col, row, 8, LCD_ILI9341_GREEN,LCD_ILI9341_BLACK,"%s",vb->m_display);
            lcd_printf(val, row, 9, LCD_ILI9341_WHITE,LCD_ILI9341_BLACK,"%s",VB_AsString(&vb->m_value));
            if (ENABLE_GWSMAN_LOG) {
                FILE*   flog = fopen("/var/run/gwsman.log","a+");
                if (flog) {
                    fprintf(flog,"\n%s\t:%s",vb->m_display,VB_AsString(&vb->m_value));
                    fclose(flog);
                }
            }
            break;
        case VAR_INTEGER:
            lcd_printf(col, row, 8, LCD_ILI9341_GREEN,LCD_ILI9341_BLACK,"%s",vb->m_display);
            do {
                int v = VB_AsInt(&vb->m_value);
                int h = v & 0xff000000;
                v = (h == 0xff000000) ? v : (0x80ffffff & v);
                lcd_printf(val, row, 7, LCD_ILI9341_WHITE,LCD_ILI9341_BLACK, "%d",v);
            } while (0);
            if (ENABLE_GWSMAN_LOG) {
                FILE*   flog = fopen("/var/run/gwsman.log","a+");
                if (flog) {
                    fprintf(flog,"\n%s\t:%d",vb->m_display,VB_AsInt(&vb->m_value));
                    fclose(flog);
                }
            }
            break;
        case VAR_LIST:
            if (NULL != (l = VB_AsList(&vb->m_value))) {
                uint16  row = (vb->m_x + 1) * 16;
                uint16  col = vb->m_y * 8;
                uint16  val = (vb->m_y + 19) * 8;
                int j = 0;
                lcd_printf(col, row, 20, LCD_ILI9341_BLACK, LCD_ILI9341_GREEN, "%s", l->m_header);
                lcd_printf(val, row, 20, LCD_ILI9341_BLACK, LCD_ILI9341_GREEN, "%s", l->m_header + 22);
                for (j = 0; j < l->m_nLine && j < IW_LIST_MAXLINE; j++,row += 16) {
                    lcd_printf(col, row + 16, 20, LCD_ILI9341_BLACK, LCD_ILI9341_WHITE, "%s", l->m_value[j]+10);
                    lcd_printf(val, row + 16, 20, LCD_ILI9341_BLACK, LCD_ILI9341_WHITE, "%s", l->m_value[j]+32);
                }
            }
            break;
        default:;
    }
}

static void lcdPrintRfInfo(bool* updated, pthread_mutex_t* hMutex) {
    if (0 == pthread_mutex_trylock(hMutex)) {
        if (!(*updated)) {
            int i = 0;
            *updated = true;
            for (i = 0; i < MAX_GWS_VAR; i++) {
                lcdPrint(&gwsVars[i], 8);
            }
        }
        pthread_mutex_unlock(hMutex);
    }
}

static void lcdPrintIwInfo(bool* updated, pthread_mutex_t* hMutex) {
    if (0 == pthread_mutex_trylock(hMutex)) {
        if (!(*updated)) {
            int i = 0;
            *updated = true;
            for (i = 0; i < MAX_WIFI_VAR; i++) {
                lcdPrint(&wifiVars[i], 8);
            }
        }
        pthread_mutex_unlock(hMutex);
    }
}

static void lcdPrintNoiseFloor(pthread_mutex_t* hMutex, GWS_CHAN_NF nf_list[], int region, short nCurrChannel) {
}

static void lcdThreadPrintData(int sig) {
    if (SIGALRM == sig && static_nPrinterID >= 0 && NULL != static_pPrinterKpi) {
        pthread_mutex_t* hMutex = &static_hHoldScreen;

        if (0 == pthread_mutex_trylock(hMutex)) {
            pthread_mutex_unlock(hMutex);
        } else return;
        static_nPrinterTimer ++;
        if ((static_nUpdateTimer++) > SECOND_TICKS) {
            static_nUpdateTimer = 0;
            static_rf_updated = false;
            static_iw_updated = false;
        }
        GWS_CHAN_NF* noise = NULL;
        int region = 0, channel = 0;
        bool art_occupied = false;
        RW_KPI_VAR(static_nPrinterID,noise,static_pPrinterKpi->m_noise_floor);
        RW_KPI_VAR(static_nPrinterID,region,static_pPrinterKpi->m_radio.m_nRegion);
        RW_KPI_VAR(static_nPrinterID,channel,static_pPrinterKpi->m_currchannel);
        if (!art_occupied) lcdShowCommandLine(hMutex, false);
        switch (nCurrentPage) {
            case PAGE_0:
                if (static_nPrinterTimer > (500 * MSEC_TICKS)) {  //100 ms per time
                    static_nPrinterTimer = 0;
                    lcdPrintRfInfo(&static_rf_updated, hMutex);
                    lcdPrintIwInfo(&static_iw_updated, hMutex);
                }
            break;
            case PAGE_1:
                if (noise && static_nPrinterTimer > (200 * MSEC_TICKS)) {  //100 ms per time
                    static_nPrinterTimer = 0;
                    lcdPrintNoiseFloor(hMutex, noise, region, channel);
                }
            break;
            case PAGE_2:
//                if (noise && static_nPrinterTimer > (2000 * MSEC_TICKS)) {  //100 ms per time
//                    static_nPrinterTimer = 0;
//                    guiPrintChannelMapping(hMutex, noise, region, channel);
//                }
            break;
            case PAGE_3:
//                if (static_nPrinterTimer > (500 * MSEC_TICKS)) {  //100 ms per time
//                    static_nPrinterTimer = 0;
//                    art_occupied = art2_guiPrintMenu(hMutex);
//                }
            break;
            default:;
        }
    }
}

static bool lcdInitPrinterTimer() {
    struct itimerval interval;

    interval.it_value.tv_sec = 0;
    interval.it_value.tv_usec = TIME_SLOT;
    interval.it_interval.tv_sec = 0;
    interval.it_interval.tv_usec = TIME_SLOT;

    static_pPrinterKpi = ConnectShm(&static_nPrinterID);
    
    if (static_nPrinterID >= 0 && NULL != static_pPrinterKpi) {
        VarBind* pVar = NULL;

        GWS_VB_LOAD(GWS_FIRMWARE, VAR_STRING, static_pPrinterKpi->m_radio.m_sFirmware);
        GWS_VB_LOAD(GWS_BOARDSNO, VAR_INTEGER, &static_pPrinterKpi->m_radio.m_nBoardSNO);
        GWS_VB_LOAD(GWS_REGION, VAR_INTEGER, &static_pPrinterKpi->m_radio.m_nRegion);
        GWS_VB_LOAD(GWS_CHANNO, VAR_INTEGER, &static_pPrinterKpi->m_radio.m_nChanNo);
        GWS_VB_LOAD(GWS_IFOUT, VAR_INTEGER, &static_pPrinterKpi->m_radio.m_nIFOUT);
        GWS_VB_LOAD(GWS_AGCMODE, VAR_INTEGER, &static_pPrinterKpi->m_radio.m_nAGCMode);
        GWS_VB_LOAD(GWS_TEMP, VAR_INTEGER, &static_pPrinterKpi->m_radio.m_nTemp);
        GWS_VB_LOAD(GWS_TXCAL, VAR_BOOL, &static_pPrinterKpi->m_radio.m_bTXCal);
        GWS_VB_LOAD(GWS_RXCAL, VAR_BOOL, &static_pPrinterKpi->m_radio.m_bRXCal);
        GWS_VB_LOAD(GWS_TX, VAR_BOOL, &static_pPrinterKpi->m_radio.m_bTX);
        GWS_VB_LOAD(GWS_RX, VAR_BOOL, &static_pPrinterKpi->m_radio.m_bRX);
        GWS_VB_LOAD(GWS_CURTXPWR, VAR_INTEGER, &static_pPrinterKpi->m_radio.m_nCurTxPwr);
        GWS_VB_LOAD(GWS_RXGAIN, VAR_INTEGER, &static_pPrinterKpi->m_radio.m_nRXGain);
        GWS_VB_LOAD(GWS_BRDMAXPWR, VAR_INTEGER, &static_pPrinterKpi->m_radio.m_nBrdMaxPwr);
        GWS_VB_LOAD(GWS_RXMAXGAIN, VAR_INTEGER, &static_pPrinterKpi->m_radio.m_nRxMaxGain);
        GWS_VB_LOAD(GWS_TXATTEN, VAR_INTEGER, &static_pPrinterKpi->m_radio.m_nTxAtten);
        GWS_VB_LOAD(GWS_RXFATTEN, VAR_INTEGER, &static_pPrinterKpi->m_radio.m_nRxFAtten);
        GWS_VB_LOAD(GWS_MAXATTEN, VAR_INTEGER, &static_pPrinterKpi->m_radio.m_nMaxAtten);
        GWS_VB_LOAD(GWS_RXRATTEN, VAR_INTEGER, &static_pPrinterKpi->m_radio.m_nRxRAtten);
        GWS_VB_LOAD(GWS_MAXTXPWR, VAR_INTEGER, &static_pPrinterKpi->m_radio.m_nMaxTxPwr);
        GWS_VB_LOAD(GWS_MINTXPWR, VAR_INTEGER, &static_pPrinterKpi->m_radio.m_nMinTxPwr);

        IW_VB_LOAD(WIFI_SSID, VAR_STRING, static_pPrinterKpi->m_wifi.m_sSSID);
        IW_VB_LOAD(WIFI_MODE, VAR_STRING, static_pPrinterKpi->m_wifi.m_sMode);
        IW_VB_LOAD(WIFI_CHANNEL, VAR_INTEGER, &static_pPrinterKpi->m_wifi.m_nChannel);
        IW_VB_LOAD(WIFI_BSSID, VAR_STRING, static_pPrinterKpi->m_wifi.m_sBSSID);
        IW_VB_LOAD(WIFI_ENCRYPTION, VAR_STRING, static_pPrinterKpi->m_wifi.m_sEncryption);
        IW_VB_LOAD(WIFI_TXPOWER, VAR_INTEGER, &static_pPrinterKpi->m_wifi.m_nTxPower);
        IW_VB_LOAD(WIFI_HARDWARE_NAME, VAR_STRING, static_pPrinterKpi->m_wifi.m_sHardwareName);
        IW_VB_LOAD(WIFI_HWMODELIST, VAR_STRING, static_pPrinterKpi->m_wifi.m_sHwModeList);
        IW_VB_LOAD(WIFI_QUALITY, VAR_STRING, static_pPrinterKpi->m_wifi.m_sQuality);
        IW_VB_LOAD(WIFI_SIGNAL, VAR_INTEGER, &static_pPrinterKpi->m_wifi.m_nSignal);
        IW_VB_LOAD(WIFI_NOISE, VAR_INTEGER, &static_pPrinterKpi->m_wifi.m_nNoise);
        IW_VB_LOAD(WIFI_BITRATE, VAR_INTEGER, &static_pPrinterKpi->m_wifi.m_nBitRate);
        IW_VB_LOAD(WIFI_ASSOCLIST, VAR_LIST, &static_pPrinterKpi->m_assoc_list);
            
        signal(SIGALRM, lcdThreadPrintData);
        setitimer(ITIMER_REAL, &interval, NULL);
        return true;
    }
    return false;
}

static void lcdExitPrinterTimer() {
    if (static_pPrinterKpi) {
        static_nPrinterID = -1;
        CloseShm(static_pPrinterKpi);
        static_pPrinterKpi = NULL;
    }
}

int ili9341_main(int argc, char *argv[]) {
    int ret = 0;
    uint8_t mode = 0;
    sem_t*  semInstance = sem_open(SEM_NAME_GWSMAN_SERVER,O_RDWR);

    if (SEM_FAILED == semInstance) return;

    parse_opts(argc, argv);

    static_fd_lcd = open(static_device_lcd, O_RDWR);
    static_fd_tch = open(static_device_touch, O_RDWR);
    if (static_fd_lcd < 0 || static_fd_tch < 0) {
        if (static_fd_lcd >= 0) close(static_fd_lcd);
        if (static_fd_tch >= 0) close(static_fd_tch);
        if (semInstance) sem_close(semInstance);
        pabort("can't open device");
        return -1;
    }
    ret = Spi_setting(static_fd_lcd);
    if (ret >= 0) {
        char command[32];
        char cKeys[32];
        int nKeys = 0;
        int nTimer = 0;
        int currCommand = 0;
        int nQid = OpenMessageQueue(0);

        if (nQid < 0) {
            LCD_ShowStatusBar("GUI mode Error on open queue(%d),error = %d\n", nQid, errno);
            if (static_fd_tch >= 0) close(static_fd_tch);
            if (static_fd_lcd >= 0) close(static_fd_lcd);
            if (semInstance) sem_close(semInstance);
            return 0;
        }
        memset(cKeys, 0x00, 32);
        memset(command, 0x00, 32);
        pthread_mutex_init(&static_hHoldScreen, NULL);
        lcd_ili9341_BACK_COLOR = LCD_ILI9341_BLACK;
        lcd_ili9341_POINT_COLOR = LCD_ILI9341_WHITE;
        Lcd_ili9341_Init(static_fd_lcd); //tft��ʼ��
        Lcd_ili9341_Clear(static_fd_lcd, LCD_ILI9341_BLACK); //����
        usleep(100000);
        lcdInitPrinterTimer();

//        Lcd_ili9341_demo(fd_lcd, fd_touch);
        while (svrTestInstance(semInstance)) {
            uint16  x = 0, y = 0;
            if (Touch_ads7843_wait_click(static_fd_tch, &x, &y)) {
            }
            usleep(100000);
        }
        LCD_ShowStatusBar("%s", "Quit ...");
        lcdExitPrinterTimer();
        pthread_mutex_unlock(&static_hHoldScreen);
        pthread_mutex_destroy(&static_hHoldScreen);
        usleep(1000000);
    }

    if (static_fd_tch >= 0) close(static_fd_tch);
    if (static_fd_lcd >= 0) close(static_fd_lcd);
    if (semInstance) sem_close(semInstance);
    return ret;
}
