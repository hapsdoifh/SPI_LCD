#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <time.h>
#include <linux/spi/spidev.h>
#include "lcd_spi.h"

//wangyaoyang
/* Read / Write SPI device extra signal*/
#define SPI_DCX_BIT_SIGNAL              0x80000000
#define SPI_DCX_BIT_GPIO_PIN            0x7fffffff
#define SPI_IOC_RD_SIGNAL_BUSY          _IOR(SPI_IOC_MAGIC, 6, __u32)
#define SPI_IOC_WR_SIGNAL_DCX           _IOW(SPI_IOC_MAGIC, 6, __u32)
//wangyaoyang

#define MAX_SPI_BUF_SIZE     1024
static uchar static_tx_buff[MAX_SPI_BUF_SIZE] = {0,};
static uchar static_rx_buff[MAX_SPI_BUF_SIZE] = {0,};
static char static_str_tx_file_path[64];
static bool static_from_file = FALSE;
static uint static_debug = FALSE;
static uchar static_mode;
static uchar static_bits = 8;
static uint static_speed = 100000;
static uint16 static_delay;

static uint SPI_DEBUG_RECV[] = {
    SPI_DEBUG_CH0_RECV,
    SPI_DEBUG_CH1_RECV,
    SPI_DEBUG_CH2_RECV,
};

static uint SPI_DEBUG_SEND[] = {
    SPI_DEBUG_CH0_SEND,
    SPI_DEBUG_CH1_SEND,
    SPI_DEBUG_CH2_SEND,
};

#if (MACH_AR934X)
#define MAX_GPIO_PIN    AR934X_GPIO_COUNT
#elif (MACH_QCA956X)
#define MAX_GPIO_PIN    QCA956X_GPIO_COUNT
#else
#define MAX_GPIO_PIN    AR933X_GPIO_COUNT
#endif

void GPIO_SET(int fd, uint pin, uint val) {
    if (0 <= pin && pin < MAX_GPIO_PIN) {
        uint val_pin = (val ? SPI_DCX_BIT_SIGNAL : 0) | pin;
        ioctl(fd, SPI_IOC_WR_SIGNAL_DCX, &val_pin);
    }
}

bool GPIO_GET(int fd, uint pin) {
    if (0 <= pin && pin < MAX_GPIO_PIN) {
        uint val = pin;
        int ret = ioctl(fd, SPI_IOC_RD_SIGNAL_BUSY, &val);
        if (ret == -1) return FALSE;
        return (!!val);
    }
    return FALSE;
}

void Spi_Set_DCX(int fd, uchar dcx) {
    if (dcx != SPI_DCX_NONE) {
        GPIO_SET(fd, GPIO_SPI_LCD_DCX, dcx);
    }
}

void pabort(const char *s) {
    perror(s);
    abort();
}

void Spi_set_tx_file(char* optarg)  {   static_from_file = TRUE; strcpy(static_str_tx_file_path, optarg);    }
bool Spi_Is_From_File()             {   return static_from_file;            }
void Spi_set_speed(char* optarg)    {   static_speed = atoi(optarg); }
void Spi_set_delay(char* optarg)    {   static_delay = atoi(optarg);        }
void Spi_set_bits(char* optarg)     {   static_bits = atoi(optarg);         }
void Spi_set_mode_loop(void)        {   static_mode |= SPI_LOOP;            }
void Spi_set_mode_cpha(void)        {   static_mode |= SPI_CPHA;            }
void Spi_set_mode_cpol(void)        {   static_mode |= SPI_CPOL;            }
void Spi_set_mode_lsb_first(void)   {   static_mode |= SPI_LSB_FIRST;       }
void Spi_set_mode_cs_high(void)     {   static_mode |= SPI_CS_HIGH;         }
void Spi_set_mode_3wire(void)       {   static_mode |= SPI_3WIRE;           }
void Spi_set_mode_no_cs(void)       {   static_mode |= SPI_NO_CS;           }
void Spi_set_mode_ready(void)       {   static_mode |= SPI_READY;           }
void Spi_enable_debug(char* optarg) {   static_debug = strtol(optarg,NULL,16); }
uint Spi_get_debug()                {   return static_debug; }

int Spi_setting(int fd) {
    int ret = 0;
    if (fd >= 0) {
        /*
         * spi mode
         */
        ret = ioctl(fd, SPI_IOC_WR_MODE, &static_mode);
        if (ret == -1)
            pabort("can't set spi write mode");

        ret = ioctl(fd, SPI_IOC_RD_MODE, &static_mode);
        if (ret == -1)
            pabort("can't get spi read mode");

        /*
         * bits per word
         */
        ret = ioctl(fd, SPI_IOC_WR_BITS_PER_WORD, &static_bits);
        if (ret == -1)
            pabort("can't set bits per word");

        ret = ioctl(fd, SPI_IOC_RD_BITS_PER_WORD, &static_bits);
        if (ret == -1)
            pabort("can't get bits per word");

        /*
         * max speed hz
         */
        ret = ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ/2, &static_speed);
        if (ret == -1)
            pabort("can't set max speed hz");

        ret = ioctl(fd, SPI_IOC_RD_MAX_SPEED_HZ/2, &static_speed);
        if (ret == -1)
            pabort("can't get max speed hz");

        if (static_debug == 0x8000) {
            printf("\nSPI setting = [mode: %s %s %s %s %s %s %s %s]"
                    "[bits/word: %d] [max speed: %d %cHz]\n",
                    SPI_LOOP & static_mode ? "LOOP" : "",
                    SPI_CPHA & static_mode ? "CPHA:1" : "CPHA:0",
                    SPI_CPOL & static_mode ? "CPOL:1" : "CPOL:0",
                    SPI_LSB_FIRST & static_mode ? "LSB_FIRST" : "",
                    SPI_CS_HIGH & static_mode ? "CS_HIGH" : "CS_LOW",
                    SPI_3WIRE & static_mode ? "3WIRE" : "",
                    SPI_NO_CS & static_mode ? "NO_CS" : "",
                    SPI_READY & static_mode ? "READY" : "",
                    static_bits,
                    static_speed > 1000000 ? static_speed / 1000000 : static_speed / 1000,
                    static_speed > 1000000 ? 'M' : 'K');
        }
    }
    return ret;
}

uchar* Spi_get_rx_buff() { return static_rx_buff; }

uint Spi_transfer(int fd, uchar *tx, uchar len, uint dev_no) {
    int ret = 0;
    
    if (tx && 0 < len && len < MAX_SPI_BUF_SIZE) {
        struct spi_ioc_transfer tr = {
            .tx_buf = (unsigned long) tx,
            .rx_buf = (unsigned long) static_rx_buff,
            //		.len = ARRAY_SIZE(tx),
            .len = len,
            .delay_usecs = static_delay,
            .speed_hz = static_speed,
            .bits_per_word = static_bits,
        };

        if (dev_no == 0xFFFFFFFF || (static_debug & SPI_DEBUG_SEND[dev_no])) {
            printf(" SPI-%d [%d] => [", dev_no, len);
            for (ret = 0; ret < len && ret < 5; ret++) {
                printf("0x%.2X ", tx[ret]);
            }
            printf("%s]\n",(len > 5) ? " ... " : "");
        }

        ret = ioctl(fd, SPI_IOC_MESSAGE(1), &tr);
        if (ret < 1) {
            pabort("can't send spi message");
            return 0;
        }

        if (dev_no == 0xFFFFFFFF || (static_debug & SPI_DEBUG_RECV[dev_no])) {
            printf(" SPI-%d [%d] <= [", dev_no, ret);
            for (ret = 0; ret < len; ret++) {
                printf("0x%.2X ", static_rx_buff[ret]);
            }
            printf("]\n");
        }
        return len;
    }
    return 0;
}

#define IS_DEBUGGING(dbg)   (dbg ? 0xFFFFFFFF : 0)
static int parse_hex_str(char* sHex) {
    char sBuff[1024];
    char *head = sBuff;
    char *hex = NULL;
    char sNum[5];
    int nNum = 0, nOff = 0;
    time_t  moment = time(&moment);

    memset (sBuff,0x00,1024);
    strcpy (sBuff,sHex);
//    printf("[%d]<",moment);
    while ((hex = strstr(head, "0x")) || (hex = strstr(head, "0X"))) {
        memset(sNum, 0x00, 5);
        memcpy(sNum, hex, 4);
//        printf("%s ", sNum);
//        printf(" <= %s", head);
        sscanf(sNum, "%x", &nNum);
        static_tx_buff[nOff] = (uchar) nNum;
        nOff++;
        head = hex + 4;
    }
//    printf(">");
    return nOff;
}

uint Spi_write_reg(int fd) {
    uchar default_tx[] = {
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
        0x40, 0x00, 0x00, 0x00, 0x00, 0x95,
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
        0xDE, 0xAD, 0xBE, 0xEF, 0xBA, 0xAD,
        0xF0, 0x0D,
    };
    FILE* fp = NULL;
    uchar len = ARRAY_SIZE(default_tx);
    if (strlen(static_str_tx_file_path) > 0 && (fp = fopen(static_str_tx_file_path, "r"))) {
        char strInput[1024];
        while (fgets(strInput, 1023, fp)) {
            len = parse_hex_str(strInput);
            if (len > 0) {
                if (static_tx_buff[0] == 0xff) {
                    Spi_Set_DCX(fd, SPI_DCX_COMD);
                    len = Spi_transfer(fd, static_tx_buff+1, len-1, IS_DEBUGGING(static_debug));
                } else if (static_tx_buff[0] == 0x00) {
                    Spi_Set_DCX(fd, SPI_DCX_DATA);
                    len = Spi_transfer(fd, static_tx_buff+1, len-1, IS_DEBUGGING(static_debug));
                } else len = Spi_transfer(fd, static_tx_buff, len, IS_DEBUGGING(static_debug));
            }
        }
        fclose(fp);
        return len;
    }
    return Spi_transfer(fd, default_tx, len, IS_DEBUGGING(static_debug));
}


