#ifndef __ADXL_H__
#define __ADXL_H__

#include <types.h>
#include "juggled_main.h"

// ADXL345 Docs (Rev A)

// CHB_SPI registers

// Bandwidth
#define ADXL345_BW_RATE 0x2c

// b1010
#define ADXL345_100HZ_OUTPUT 10

// Power Control register
#define ADXL345_POWER_CTL 0x2d

// Bit masks for the POWER_CTL register
#define ADXL345_MEASURE (1<<3)
#define ADXL345_LINK    (1<<5)


#define ADXL345_DATAX0 0x32

#define ADXL345_DATA_FORMAT 0x31

#define ADXL345_FULLRES 8
#define ADXL345_2G   0
#define ADXL345_4G   1
#define ADXL345_8G   2
#define ADXL345_16G  3

#define ADXL345_FIFO_CTL 0x38

// Figure 36, page 15, W_not bit == Read bit?
#define ADXL345_SPI_READ_MODE (1<<7)

#define ADXL345_SPI_MULTI_BYTE (1<<6)

extern U16 acc_X, acc_Y, acc_Z;
extern volatile bool is_adxl_on;

U8 adxl_read(U8 addr);
U8 adxl_multi_read(U8 addr, U8 *data, U8 len);
U8 adxl_write(U8 addr, U8 data);
void adxl_off();
void adxl_init();

#endif // __ADXL_H__

