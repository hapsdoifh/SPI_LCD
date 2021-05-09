#include "lcd_omcj4x8c_16.h"

static uint Lcd_omcj4x8c_wr(int fd, uchar rs, uchar content) {
    if (fd >= 0) { //without LCD_CTRL_BIT_READ
        uchar boot = LCD_CTRL_BIT_SYNC | (RS_DATA == rs ? LCD_CTRL_BIT_DATA : 0);
        uchar tx[] = {boot, content & 0xf0, (content << 4) & 0xf0};

        return Spi_transfer(fd, tx, ARRAY_SIZE(tx), 0);
    }
    return 0;
}

/*------------------��ʼ��-----------------*/
void Lcd_omcj4x8c_init_lcd(int fd) {
    Lcd_omcj4x8c_wr(fd, RS_COMD, 0x30); /*30---����ָ���*/
    Lcd_omcj4x8c_wr(fd, RS_COMD, 0x01); /*��������ַָ��ָ��00H*/
    usleep(100);
    Lcd_omcj4x8c_wr(fd, RS_COMD, 0x06); /*�����ƶ�����*/
    Lcd_omcj4x8c_wr(fd, RS_COMD, 0x0c); /*����ʾ�����α�*/
}

/*---------------��ʾ���ֻ��ַ�----------------*/
void Lcd_omcj4x8c_chn_disp(int fd, uchar code *chn) {
    uchar i, j;
    Lcd_omcj4x8c_wr(fd, RS_COMD, 0x30);
    Lcd_omcj4x8c_wr(fd, RS_COMD, 0x80);
    for (j = 0; j < 4; j++) {
        for (i = 0; i < 16; i++)
            Lcd_omcj4x8c_wr(fd, RS_DATA, chn[j * 16 + i]);
    }
}

/*-----------�ϰ�����ʾ���ֻ��ַ�------------*/
void Lcd_omcj4x8c_chn_disp1(int fd, uchar code *chn) {
    uchar i, j;
    Lcd_omcj4x8c_wr(fd, RS_COMD, 0x30);
    Lcd_omcj4x8c_wr(fd, RS_COMD, 0x80);
    j = 0;
    for (i = 0; i < 16; i++)
        Lcd_omcj4x8c_wr(fd, RS_DATA, chn[j * 16 + i]);
    Lcd_omcj4x8c_wr(fd, RS_COMD, 0x90);
    j = 1;
    for (i = 0; i < 16; i++)
        Lcd_omcj4x8c_wr(fd, RS_DATA, chn[j * 16 + i]);
}

/*----------------��ʾͼ��-----------------*/
void Lcd_omcj4x8c_img_disp(int fd, uchar code *img) {
    uchar i, j;
    for (j = 0; j < 32; j++) {
        for (i = 0; i < 8; i++) {
            Lcd_omcj4x8c_wr(fd, RS_COMD, 0x34);
            Lcd_omcj4x8c_wr(fd, RS_COMD, LCD_Y + j);
            Lcd_omcj4x8c_wr(fd, RS_COMD, LCD_X1 + i);
            Lcd_omcj4x8c_wr(fd, RS_COMD, 0x30);
            Lcd_omcj4x8c_wr(fd, RS_DATA, img[j * 16 + i * 2]);
            Lcd_omcj4x8c_wr(fd, RS_DATA, img[j * 16 + i * 2 + 1]);
        }
    }
    for (j = 32; j < 64; j++) {
        for (i = 0; i < 8; i++) {
            Lcd_omcj4x8c_wr(fd, RS_COMD, 0x34);
            Lcd_omcj4x8c_wr(fd, RS_COMD, LCD_Y + j - 32);
            Lcd_omcj4x8c_wr(fd, RS_COMD, LCD_X2 + i);
            Lcd_omcj4x8c_wr(fd, RS_COMD, 0x30);
            Lcd_omcj4x8c_wr(fd, RS_DATA, img[j * 16 + i * 2]);
            Lcd_omcj4x8c_wr(fd, RS_DATA, img[j * 16 + i * 2 + 1]);
        }
    }
    Lcd_omcj4x8c_wr(fd, RS_COMD, 0x36);
}

/*-------------�°�����ʾͼ��--------------*/
void Lcd_omcj4x8c_img_disp1(int fd, uchar code *img) {
    uchar i, j;
    for (j = 0; j < 32; j++) {
        for (i = 0; i < 8; i++) {
            Lcd_omcj4x8c_wr(fd, RS_COMD, 0x34);
            Lcd_omcj4x8c_wr(fd, RS_COMD, LCD_Y + j);
            Lcd_omcj4x8c_wr(fd, RS_COMD, LCD_X2 + i);
            Lcd_omcj4x8c_wr(fd, RS_COMD, 0x30);
            Lcd_omcj4x8c_wr(fd, RS_DATA, img[j * 16 + i * 2]);
            Lcd_omcj4x8c_wr(fd, RS_DATA, img[j * 16 + i * 2 + 1]);
        }
    }
    Lcd_omcj4x8c_wr(fd, RS_COMD, 0x36);
}

/*--------------��ʾ����----------------*/
void Lcd_omcj4x8c_lat_disp(int fd, uchar data1, uchar data2) {
    uchar i, j, k, x;
    x = LCD_X1;
    for (k = 0; k < 2; k++) {
        for (j = 0; j < 16; j++) {
            for (i = 0; i < 8; i++) {
                Lcd_omcj4x8c_wr(fd, RS_COMD, 0x34);
                Lcd_omcj4x8c_wr(fd, RS_COMD, LCD_Y + j * 2);
                Lcd_omcj4x8c_wr(fd, RS_COMD, x + i);
                Lcd_omcj4x8c_wr(fd, RS_COMD, 0x30);
                Lcd_omcj4x8c_wr(fd, RS_DATA, data1);
                Lcd_omcj4x8c_wr(fd, RS_DATA, data1);
            }
            for (i = 0; i < 8; i++) {
                Lcd_omcj4x8c_wr(fd, RS_COMD, 0x34);
                Lcd_omcj4x8c_wr(fd, RS_COMD, LCD_Y + j * 2 + 1);
                Lcd_omcj4x8c_wr(fd, RS_COMD, x + i);
                Lcd_omcj4x8c_wr(fd, RS_COMD, 0x30);
                Lcd_omcj4x8c_wr(fd, RS_DATA, data2);
                Lcd_omcj4x8c_wr(fd, RS_DATA, data2);
            }
        }
        x = LCD_X2;
    }
    Lcd_omcj4x8c_wr(fd, RS_COMD, 0x36);
}

/*-----------------------------------------------*/
//��data1=0xff,data2=0xffʱ,��x0,y0��������ʾ16xl*yl.

void Lcd_omcj4x8c_con_disp(int fd, uchar data1, uchar data2, uchar x0, uchar y0, uchar xl, uchar yl) {
    uchar i, j;
    for (j = 0; j < yl; j++) {
        for (i = 0; i < xl; i++) {
            Lcd_omcj4x8c_wr(fd, RS_COMD, 0x34);
            Lcd_omcj4x8c_wr(fd, RS_COMD, y0 + j);
            Lcd_omcj4x8c_wr(fd, RS_COMD, x0 + i);
            Lcd_omcj4x8c_wr(fd, RS_COMD, 0x30);
            Lcd_omcj4x8c_wr(fd, RS_DATA, data1);
            Lcd_omcj4x8c_wr(fd, RS_DATA, data2);
        }
    }
    Lcd_omcj4x8c_wr(fd, RS_COMD, 0x36);
}

/*--------------��DDRAM------------------*/
void Lcd_omcj4x8c_clrram(int fd) {
    Lcd_omcj4x8c_wr(fd, RS_COMD, 0x30);
    Lcd_omcj4x8c_wr(fd, RS_COMD, 0x01);
    usleep(180);
}

