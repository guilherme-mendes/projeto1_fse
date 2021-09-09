#include <lcd_control.h>

int fd;

// float to string
void float_type(float myFloat)
{
  char buffer[20];
  sprintf(buffer, "%4.2f", myFloat);
  ln_type(buffer);
}

// clr lcd go home loc 0x80
void ClrDisplay(void)
{
  byte_d(0x01, LCD_CMD);
  byte_d(0x02, LCD_CMD);
}

// go to location on LCD
void loca_d(int line)
{
  byte_d(line, LCD_CMD);
}

// this allows use of any size string
void ln_type(const char *s)
{

  while (*s)
    byte_d(*(s++), LCD_CHR);
}

void byte_d(int bits, int mode)
{

  //Send byte to data pins
  // bits = the data
  // mode = 1 for data, 0 for command
  int bits_high;
  int bits_low;
  // uses the two half byte writes to LCD
  bits_high = mode | (bits & 0xF0) | LCD_BACKLIGHT;
  bits_low = mode | ((bits << 4) & 0xF0) | LCD_BACKLIGHT;

  // High bits
  wiringPiI2CReadReg8(fd, bits_high);
  t_enbl(bits_high);

  // Low bits
  wiringPiI2CReadReg8(fd, bits_low);
  t_enbl(bits_low);
}

void print_d(float TI,float TR, float TE) 
{
    if (wiringPiSetup() == -1)
    exit(1);
    
    fd = wiringPiI2CSetup(I2C_ADDR);

    init_d();
    loca_d(L1);
    loca_d(L2);
    ln_type("TE");
    float_type(TE);
    ln_type("TI");
    float_type(TI);
    ln_type("TR");
    float_type(TR);
}

void t_enbl(int bits)
{
  // Toggle enable pin on LCD display
  delayMicroseconds(500);
  wiringPiI2CReadReg8(fd, (bits | ENABLE));
  delayMicroseconds(500);
  wiringPiI2CReadReg8(fd, (bits & ~ENABLE));
  delayMicroseconds(500);
}

void init_d()
{
  // Initialise display
  byte_d(0x33, LCD_CMD); // Initialise
  byte_d(0x32, LCD_CMD); // Initialise
  byte_d(0x06, LCD_CMD); // Cursor move direction
  byte_d(0x0C, LCD_CMD); // 0x0F On, Blink Off
  byte_d(0x28, LCD_CMD); // Data length, number of lines, font size
  byte_d(0x01, LCD_CMD); // Clear display
  delayMicroseconds(500);
}

