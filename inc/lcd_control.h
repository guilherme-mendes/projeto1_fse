#ifndef _LCD_CONTROL_H_
#define _LCD_CONTROL_H_

#include <stdio.h>
#include <stdlib.h>
#include <wiringPi.h>
#include <wiringPiI2C.h>

#define I2C_ADDR 0x27
#define LCD_CHR 1
#define LCD_CMD 0
#define L1 0x80
#define L2 0xC0
#define LCD_BACKLIGHT 0x08
#define ENABLE 0b00000100

void t_enbl(int bits);
void print_d(float tempI, float tempR, float tempE);
void byte_d(int bits, int mode);
void init_d(void);

void float_type(float myFloat);
void loca_d(int line);
void ln_type(const char *s);

#endif