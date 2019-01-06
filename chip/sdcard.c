#include <common/pie.h>
#include <bus/pin.h>
#include <bus/spi.h>
#include <chip/sdcard.h>
#include <string.h>

//#define SD_DEBUG

#ifdef SD_DEBUG
#define sd_dbg(fmt, ...)   pie_print(fmt, ##__VA_ARGS__)
#else
#define sd_dbg(fmt, ...)
#endif

#define DUMMY 0xFF

#define SD_TIMEOUT 100

// SD卡类型定义  
#define SD_TYPE_ERR     0X00
#define SD_TYPE_MMC     0X01
#define SD_TYPE_V1      0X02
#define SD_TYPE_V2      0X04
#define SD_TYPE_V2HC    0X06     
// SD卡指令表      
#define CMD0    0       //卡复位
#define CMD1    1
#define CMD8    8       //命令8 ，SEND_IF_COND
#define CMD9    9       //命令9 ，读CSD数据
#define CMD10   10      //命令10，读CID数据
#define CMD12   12      //命令12，停止数据传输
#define CMD16   16      //命令16，设置SectorSize 应返回0x00
#define CMD17   17      //命令17，读sector
#define CMD18   18      //命令18，读Multi sector
#define CMD23   23      //命令23，设置多sector写入前预先擦除N个block
#define CMD24   24      //命令24，写sector
#define CMD25   25      //命令25，写Multi sector
#define CMD41   41      //命令41，应返回0x00
#define CMD55   55      //命令55，应返回0x01
#define CMD58   58      //命令58，读OCR信息
#define CMD59   59      //命令59，使能/禁止CRC，应返回0x00
//数据写入回应字意义
#define MSD_DATA_OK                0x05
#define MSD_DATA_CRC_ERROR         0x0B
#define MSD_DATA_WRITE_ERROR       0x0D
#define MSD_DATA_OTHER_ERROR       0xFF
//SD卡回应标记字
#define MSD_RESPONSE_NO_ERROR      0x00
#define MSD_IN_IDLE_STATE          0x01
#define MSD_ERASE_RESET            0x02
#define MSD_ILLEGAL_COMMAND        0x04
#define MSD_COM_CRC_ERROR          0x08
#define MSD_ERASE_SEQUENCE_ERROR   0x10
#define MSD_ADDRESS_ERROR          0x20
#define MSD_PARAMETER_ERROR        0x40
#define MSD_RESPONSE_FAILURE       0xFF

#define SD_LOW_SPEED    2000000
#define SD_HIGH_SPEED   500000


int sd_init(struct sdcard* sd, spi_t* spi, pin_t* pin, int cs_pin)
{
    ASSERT(sd);
    ASSERT(spi);
    ASSERT(pin);

    sd->spi = spi;
    sd->pin = pin;
    sd->cs_pin = cs_pin;

    int ret = spi_set_mode(sd->spi, SPI_MODE_2);

    if(ret != ENO_OK)
      return ret;

    return ENO_OK;
}

int sd_wait_ready(struct sdcard* sd)
{
  ASSERT(sd);
  ASSERT(sd->spi);

  millis_t lasttime = get_millis();
  uint8_t data = 0;
  do{
    spi_read(sd->spi, &data, 1);
    if(data == 0xFF)
      return 0;
  }while(get_millis() - lasttime < SD_TIMEOUT);

  return -1;
}

int sd_deselect(struct sdcard* sd)
{
  ASSERT(sd);
  ASSERT(sd->pin);
  ASSERT(sd->spi);

  pin_set_level(sd->pin, sd->cs_pin, PIN_LEVEL_HIGH);
  uint8_t dummy = DUMMY;
  spi_write(sd->spi, &dummy, 1);

  return 0;
}

int sd_select(struct sdcard* sd)
{
  pin_set_level(sd->pin, sd->cs_pin, PIN_LEVEL_LOW);

  if(sd_wait_ready(sd) != 0)
  {
    sd_deselect(sd);
    return -1;
  }
  
  return 0;
}


int sd_check_response(struct sdcard* sd, uint8_t rep)
{
  ASSERT(sd);
  ASSERT(sd->spi);

  millis_t lasttime = get_millis();
  uint8_t data;
  do{
    spi_read(sd->spi, &data, 1);
    if(data == rep)
      return 0;
  }while(get_millis() - lasttime < SD_TIMEOUT);

  return -1;
}

int sd_write_block(struct sdcard* sd, uint8_t cmd, uint8_t* wr_data)
{
  ASSERT(sd);
  ASSERT(sd->spi);

  if(sd_wait_ready(sd) != 0)
    return -1;

  spi_write(sd->spi, &cmd, 1);

  if(cmd != 0xFD)
  {
    spi_write(sd->spi, wr_data, 512);

    uint16_t dummy = 0xFFFF;

    spi_write(sd->spi, (uint8_t*)&dummy, 2);

    uint8_t data;
    spi_read(sd->spi, &data, 1);

    if((data & 0x1F) != 0x05)
      return -1;
  }

  return 0;
}

int sd_read_data(struct sdcard* sd, uint8_t* rd_data, size_t rd_len)
{
  ASSERT(sd);
  ASSERT(sd->spi);
  ASSERT(rd_data);

  if(sd_check_response(sd, 0xFE))
    return -1;

  spi_read(sd->spi, rd_data, rd_len);

  uint16_t dummy = 0xFFFF;

  spi_write(sd->spi, (uint8_t*)&dummy, 2);

  return 0;
}

int sd_write_command(struct sdcard* sd, uint8_t cmd, uint32_t args, uint8_t crc)
{
  ASSERT(sd);
  ASSERT(sd->spi);
  uint8_t r1 = 0;
  sd_deselect(sd);
  if(sd_select(sd) != 0)
    return -1;
  uint8_t wr_data[6];
  wr_data[0] = cmd | 0x40;
  wr_data[1] = (args >> 24) & 0xFF;
  wr_data[2] = (args >> 16) & 0xFF;
  wr_data[3] = (args >> 8) & 0xFF;
  wr_data[4] = args & 0xFF;
  wr_data[5] = crc;
  spi_write(sd->spi, wr_data, sizeof(wr_data));

  uint8_t dummy = DUMMY;
  if(cmd == CMD12)
    spi_write(sd->spi, &dummy, 1);
  uint8_t retry = 0x1F;
  do{
    spi_read(sd->spi, &r1, 1);
  }while((r1 & 0x80) && retry --);

  return r1;
}

int sd_get_cid(struct sdcard* sd, uint8_t* cid)
{
  ASSERT(sd);
  ASSERT(cid);

  int r1;

  r1 = sd_write_command(sd, CMD10, 0, 0x01);
  if(r1 == 0x00)
    r1 = sd_read_data(sd, cid, 16);
  sd_deselect(sd);

  if(r1)
    return -1;
  return 0;
}

int sd_get_csd(struct sdcard* sd, uint8_t*csd)
{
  ASSERT(sd);
  ASSERT(csd);

  int r1 = 0;
  r1 = sd_write_command(sd, CMD9, 0, 0x01);
  if(r1 == 0)
    r1 = sd_read_data(sd, csd, 16);

  sd_deselect(sd);
  if(r1)
    return -1;
  return 0;
}

int sd_get_sector_count(struct  sdcard* sd)
{
  uint8_t csd[16];
  uint32_t capacity;
  uint16_t csize;
  uint8_t n;

  if(sd_get_csd(sd, csd) != 0)
    return 0;
  if((csd[0] & 0xC0) == 0x40)
  {
    csize = csd[9] + ((uint16_t)csd[8] << 8) + 1;
    capacity = (uint32_t)csize << 10;
  }
  else
  {
    n = (csd[5] & 15) + ((csd[10] & 128) >> 7) + ((csd[9] & 3) << 1) + 2;
    csize = (csd[8] >> 6) + ((uint16_t)csd[7] << 2) + ((uint16_t)(csd[6] & 3) << 10) + 1;
    capacity = (uint32_t)csize << (n - 9);
  }

  return capacity;
}

int sd_initialize(struct sdcard* sd)
{
  ASSERT(sd);
  ASSERT(sd->spi);

  int r1 = 0;
  uint16_t retry;
  uint8_t rd_data[4];
  spi_set_speed(sd->spi, SD_LOW_SPEED);

  uint8_t dummy = 0xFF;

  for(int i = 0; i < 10; i++)
    spi_write(sd->spi, &dummy, 1);
  retry = 20;
  do{
    r1 = sd_write_command(sd, CMD0, 0, 0x95);
  }while((r1 != 0x01) && retry --);

  sd->type = 0;
  if(r1 == 0x01)
  {
    if(sd_write_command(sd, CMD8, 0x1AA, 0x87) == 1)
    {
      spi_read(sd->spi, rd_data, 4);
      if(rd_data[2] == 0x01 && rd_data[3] == 0xAA)
      {
        retry = 0xFFFE;
        do{
          sd_write_command(sd, CMD55, 0, 0x01);
          r1 = sd_write_command(sd, CMD41, 0x40000000, 0x01);
        }while(r1 && retry--);
        if(retry && sd_write_command(sd, CMD58, 0, 0x01) == 0)
        {
          spi_read(sd->spi, rd_data, 4);
          if(rd_data[0] & 0x40)
            sd->type = SD_TYPE_V2HC;
          else
            sd->type = SD_TYPE_V2;
        }
      }
    }
    else
    {
      sd_write_command(sd, CMD55, 0, 0x01);
      r1 = sd_write_command(sd, CMD41, 0, 0x01);
      if(r1 < 0)
        return -1;
      if(r1 <= 1)
      {
        sd->type = SD_TYPE_V1;
        retry = 0xFFFE;
        do{
          sd_write_command(sd, CMD55, 0, 0x01);
          r1 = sd_write_command(sd, CMD41, 0, 0x01);
        }while(r1 && retry --);
      }
      else
      {
        sd->type = SD_TYPE_MMC;
        retry = 0xFFFE;
        do{
          r1 = sd_write_command(sd, CMD1, 0, 0x01);
        }while(r1 && retry--);
      }
      if(retry == 0 || sd_write_command(sd, CMD16, 512, 0x01) != 0)
        sd->type = SD_TYPE_ERR;

    }
  }

  sd_deselect(sd);
  spi_set_speed(sd->spi, SD_HIGH_SPEED);
  if(sd->type)
    return 0;
  else if(r1)
    return r1;

  return -1;
}

int sd_read_disk(struct sdcard* sd, uint32_t sector, uint8_t* rd_data, size_t nsec)
{
  int ret = 0;
  if(sd->type != SD_TYPE_V2HC)
    sector <<= 9;
  if(nsec == 1)
  {
    ret = sd_write_command(sd, CMD17, sector, 0x01);
    if(ret == 0)
      ret = sd_read_data(sd, rd_data, 512);
  }
  else
  {
    ret = sd_write_command(sd, CMD18, sector, 0x01);
    do{
      ret = sd_read_data(sd, rd_data, 512);
      rd_data += 512;
    }while(-- nsec && ret == 0);
    sd_write_command(sd, CMD12, 0, 0x01);
  }
  sd_deselect(sd);
  return ret;
}

int sd_write_disk(struct sdcard* sd, uint32_t sector, uint8_t* wr_data, size_t nsec)
{
  ASSERT(sd);
  ASSERT(wr_data);

  int ret = 0;
  if(sd->type != SD_TYPE_V2HC)
    sector *= 512;
  if(nsec == 1)
  {
    ret = sd_write_command(sd, CMD24, sector, 0x01);
    if(ret == 0)
      ret = sd_write_block(sd, 0xFE, wr_data);
  }
  else
  {
    if(sd->type != SD_TYPE_MMC)
    {
      sd_write_command(sd, CMD55, 0, 0x01);
      sd_write_command(sd, CMD23, nsec, 0x01);
    }
    ret = sd_write_command(sd, CMD25, sector, 0x01);
    if(ret == 0)
    {
      do{
        ret = sd_write_block(sd, 0xFC, wr_data);
        wr_data += 512;
      }while(-- nsec && ret == 0);
      ret = sd_write_block(sd, 0xFD, 0);
    }
  }
  sd_deselect(sd);
  return ret;
}