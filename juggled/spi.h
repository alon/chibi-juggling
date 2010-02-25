#ifndef __SPI_H__
#define __SPI_H__

#include <types.h>

void spi_init();
U8 spi_xfer_byte(U8 data);

#endif // __SPI_H__

