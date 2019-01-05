#include <chip/ili93xx.h>

//扫描方向定义
#define L2R_U2D  0 //从左到右,从上到下
#define L2R_D2U  1 //从左到右,从下到上
#define R2L_U2D  2 //从右到左,从上到下
#define R2L_D2U  3 //从右到左,从下到上

#define U2D_L2R  4 //从上到下,从左到右
#define U2D_R2L  5 //从上到下,从右到左
#define D2U_L2R  6 //从下到上,从左到右
#define D2U_R2L  7 //从下到上,从右到左   

#define DFT_SCAN_DIR  L2R_U2D  //默认的扫描方向

void ili93xx_write_reg(struct ili93xx* ili93xx, uint16_t value)
{
  ASSERT(ili93xx);
  ASSERT(ili93xx->ops);

  if(ili93xx->ops->write_reg)
    ili93xx->ops->write_reg(value);
}

void ili93xx_write_data(struct ili93xx* ili93xx, uint16_t value)
{
  ASSERT(ili93xx);
  ASSERT(ili93xx->ops);

  if(ili93xx->ops->write_data)
    ili93xx->ops->write_data(value);
}

uint16_t ili93xx_read_data(struct ili93xx* ili93xx)
{
  ASSERT(ili93xx);
  ASSERT(ili93xx->ops);

  if(ili93xx->ops->read_data)
    return ili93xx->ops->read_data();
  return 0;
}

void ili93xx_write_register(struct ili93xx* ili93xx, uint16_t reg, uint16_t value)
{
  ili93xx_write_reg(ili93xx, reg);
  ili93xx_write_data(ili93xx, value);
}

uint16_t ili93xx_read_register(struct ili93xx* ili93xx, uint16_t reg)
{
  ili93xx_write_reg(ili93xx, reg);
  return ili93xx_read_data(ili93xx);
}

void ili93xx_backlight_control(struct ili93xx* ili93xx, int state)
{
  ASSERT(ili93xx);
  ASSERT(ili93xx->ops);

  if(ili93xx->ops->backlight_control)
    ili93xx->ops->backlight_control(state);
}

void ili93xx_writeram_prepare(struct ili93xx* ili93xx)
{
  ili93xx_write_reg(ili93xx, ili93xx->wramcmd);
}

void ili93xx_set_cursor(struct ili93xx* ili93xx, uint16_t x, uint16_t y)
{
  ili93xx_write_reg(ili93xx, ili93xx->setxcmd);
  ili93xx_write_data(ili93xx, x >> 8);
  ili93xx_write_data(ili93xx, x & 0xFF);
  ili93xx_write_reg(ili93xx, ili93xx->setycmd);
  ili93xx_write_data(ili93xx, y >> 8);
  ili93xx_write_data(ili93xx, y & 0xFF);
}

void ili93xx_fill(struct ili93xx* ili93xx, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color)
{
  int xlen = x2 - x1 + 1;
  for(int i = y1; i <= y2; i ++)
  {
  ili93xx_set_cursor(ili93xx, x1, i);
  ili93xx_writeram_prepare(ili93xx);
  for(int j=0; j < xlen; j ++)
    ili93xx_write_data(ili93xx, color);
  }
}

void ili93xx_scan_dir(struct ili93xx* ili93xx, int dir)
{
  uint16_t regval = 0;
  uint16_t dirreg = 0;
  switch(dir)
  {
    case L2R_U2D://从左到右,从上到下
      regval|=(0<<7)|(0<<6)|(0<<5); 
      break;
    case L2R_D2U://从左到右,从下到上
      regval|=(1<<7)|(0<<6)|(0<<5); 
      break;
    case R2L_U2D://从右到左,从上到下
      regval|=(0<<7)|(1<<6)|(0<<5); 
      break;
    case R2L_D2U://从右到左,从下到上
      regval|=(1<<7)|(1<<6)|(0<<5); 
      break;   
    case U2D_L2R://从上到下,从左到右
      regval|=(0<<7)|(0<<6)|(1<<5); 
      break;
    case U2D_R2L://从上到下,从右到左
      regval|=(0<<7)|(1<<6)|(1<<5); 
      break;
    case D2U_L2R://从下到上,从左到右
      regval|=(1<<7)|(0<<6)|(1<<5); 
      break;
    case D2U_R2L://从下到上,从右到左
      regval|=(1<<7)|(1<<6)|(1<<5); 
      break;   
  }
  dirreg  = 0x36;
  ili93xx_write_register(ili93xx, dirreg, regval);
  ili93xx_write_reg(ili93xx, ili93xx->setxcmd);
  ili93xx_write_data(ili93xx, 0);
  ili93xx_write_data(ili93xx, 0);
  ili93xx_write_data(ili93xx, (ili93xx->width - 1) >> 8);
  ili93xx_write_data(ili93xx, (ili93xx->width - 1) & 0xFF);
  ili93xx_write_reg(ili93xx, ili93xx->setycmd);
  ili93xx_write_data(ili93xx, 0);
  ili93xx_write_data(ili93xx, 0);
  ili93xx_write_data(ili93xx, (ili93xx->height - 1) >> 8);
  ili93xx_write_data(ili93xx, (ili93xx->height - 1) & 0xFF);
}

void ili93xx_set_dir(struct ili93xx* ili93xx, uint8_t dir)
{
  if(dir == 0)
  {
    ili93xx->dir = dir;
    ili93xx->width = 240;
    ili93xx->height = 320;
    ili93xx->wramcmd = 0x2C;
    ili93xx->setxcmd = 0x2A;
    ili93xx->setycmd = 0x2B;
  }
  else
  {
    ili93xx->dir = 1;
    ili93xx->width = 320;
    ili93xx->height = 240;
    ili93xx->wramcmd = 0x2C;
    ili93xx->setxcmd = 0x2A;
    ili93xx->setycmd = 0x2B;
  }
  ili93xx_scan_dir(ili93xx, DFT_SCAN_DIR);
}

void ili93xx_init(struct ili93xx* ili93xx)
{
  ili93xx_read_register(ili93xx, 0x0000);
  ili93xx_write_reg(ili93xx, 0xD3);
  ili93xx_read_data(ili93xx);
  ili93xx_read_data(ili93xx);
  int id = ili93xx_read_data(ili93xx);
  id <<= 8;
  id |= ili93xx_read_data(ili93xx);
  pie_print("ili93xx id:0x%x", id);

  ili93xx_write_reg(ili93xx, 0xCF);  
  ili93xx_write_data(ili93xx, 0x00); 
  ili93xx_write_data(ili93xx, 0xC1); 
  ili93xx_write_data(ili93xx, 0X30); 
  ili93xx_write_reg(ili93xx, 0xED);  
  ili93xx_write_data(ili93xx, 0x64); 
  ili93xx_write_data(ili93xx, 0x03); 
  ili93xx_write_data(ili93xx, 0X12); 
  ili93xx_write_data(ili93xx, 0X81); 
  ili93xx_write_reg(ili93xx, 0xE8);  
  ili93xx_write_data(ili93xx, 0x85); 
  ili93xx_write_data(ili93xx, 0x10); 
  ili93xx_write_data(ili93xx, 0x7A); 
  ili93xx_write_reg(ili93xx, 0xCB);  
  ili93xx_write_data(ili93xx, 0x39); 
  ili93xx_write_data(ili93xx, 0x2C); 
  ili93xx_write_data(ili93xx, 0x00); 
  ili93xx_write_data(ili93xx, 0x34); 
  ili93xx_write_data(ili93xx, 0x02); 
  ili93xx_write_reg(ili93xx, 0xF7);  
  ili93xx_write_data(ili93xx, 0x20); 
  ili93xx_write_reg(ili93xx, 0xEA);  
  ili93xx_write_data(ili93xx, 0x00); 
  ili93xx_write_data(ili93xx, 0x00); 
  ili93xx_write_reg(ili93xx, 0xC0);  //Power control 
  ili93xx_write_data(ili93xx, 0x1B);   //VRH[5:0] 
  ili93xx_write_reg(ili93xx, 0xC1);  //Power control 
  ili93xx_write_data(ili93xx, 0x01);   //SAP[2:0];BT[3:0] 
  ili93xx_write_reg(ili93xx, 0xC5);  //VCM control 
  ili93xx_write_data(ili93xx, 0x30);   //3F
  ili93xx_write_data(ili93xx, 0x30);   //3C
  ili93xx_write_reg(ili93xx, 0xC7);  //VCM control2 
  ili93xx_write_data(ili93xx, 0XB7); 
  ili93xx_write_reg(ili93xx, 0x36);  // Memory Access Control 
  ili93xx_write_data(ili93xx, 0x48); 
  ili93xx_write_reg(ili93xx, 0x3A);   
  ili93xx_write_data(ili93xx, 0x55); 
  ili93xx_write_reg(ili93xx, 0xB1);   
  ili93xx_write_data(ili93xx, 0x00);   
  ili93xx_write_data(ili93xx, 0x1A); 
  ili93xx_write_reg(ili93xx, 0xB6);  // Display Function Control 
  ili93xx_write_data(ili93xx, 0x0A); 
  ili93xx_write_data(ili93xx, 0xA2); 
  ili93xx_write_reg(ili93xx, 0xF2);  // 3Gamma Function Disable 
  ili93xx_write_data(ili93xx, 0x00); 
  ili93xx_write_reg(ili93xx, 0x26);  //Gamma curve selected 
  ili93xx_write_data(ili93xx, 0x01); 
  ili93xx_write_reg(ili93xx, 0xE0);  //Set Gamma 
  ili93xx_write_data(ili93xx, 0x0F); 
  ili93xx_write_data(ili93xx, 0x2A); 
  ili93xx_write_data(ili93xx, 0x28); 
  ili93xx_write_data(ili93xx, 0x08); 
  ili93xx_write_data(ili93xx, 0x0E); 
  ili93xx_write_data(ili93xx, 0x08); 
  ili93xx_write_data(ili93xx, 0x54); 
  ili93xx_write_data(ili93xx, 0XA9); 
  ili93xx_write_data(ili93xx, 0x43); 
  ili93xx_write_data(ili93xx, 0x0A); 
  ili93xx_write_data(ili93xx, 0x0F); 
  ili93xx_write_data(ili93xx, 0x00); 
  ili93xx_write_data(ili93xx, 0x00); 
  ili93xx_write_data(ili93xx, 0x00); 
  ili93xx_write_data(ili93xx, 0x00);   
  ili93xx_write_reg(ili93xx, 0XE1);  //Set Gamma 
  ili93xx_write_data(ili93xx, 0x00); 
  ili93xx_write_data(ili93xx, 0x15); 
  ili93xx_write_data(ili93xx, 0x17); 
  ili93xx_write_data(ili93xx, 0x07); 
  ili93xx_write_data(ili93xx, 0x11); 
  ili93xx_write_data(ili93xx, 0x06); 
  ili93xx_write_data(ili93xx, 0x2B); 
  ili93xx_write_data(ili93xx, 0x56); 
  ili93xx_write_data(ili93xx, 0x3C); 
  ili93xx_write_data(ili93xx, 0x05); 
  ili93xx_write_data(ili93xx, 0x10); 
  ili93xx_write_data(ili93xx, 0x0F); 
  ili93xx_write_data(ili93xx, 0x3F); 
  ili93xx_write_data(ili93xx, 0x3F); 
  ili93xx_write_data(ili93xx, 0x0F); 
  ili93xx_write_reg(ili93xx, 0x2B); 
  ili93xx_write_data(ili93xx, 0x00);
  ili93xx_write_data(ili93xx, 0x00);
  ili93xx_write_data(ili93xx, 0x01);
  ili93xx_write_data(ili93xx, 0x3f);
  ili93xx_write_reg(ili93xx, 0x2A); 
  ili93xx_write_data(ili93xx, 0x00);
  ili93xx_write_data(ili93xx, 0x00);
  ili93xx_write_data(ili93xx, 0x00);
  ili93xx_write_data(ili93xx, 0xef);   
  ili93xx_write_reg(ili93xx, 0x11); //Exit Sleep
  pi_sleep_ms(120);
  ili93xx_write_reg(ili93xx, 0x29); //display on  

  ili93xx_set_dir(ili93xx, 0);
  ili93xx_backlight_control(ili93xx, 1);
}