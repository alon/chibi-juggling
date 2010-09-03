#include "adxl.h"
#include "spi.h"

// ------------ ADXL Control ----------------------------------------------

U16 acc_X = 0, acc_Y = 0, acc_Z = 0;
volatile bool is_adxl_on = false;

/**************************************************************************/
/*!

*/
/**************************************************************************/
void adxl_off()
{
    // ?
}

/**************************************************************************/
/*!

*/
/**************************************************************************/
void read_adxl(U8* dat)
{
    int i;
    SPI_ENB();
    spi_xfer_byte(0xf2);

    for (i=0; i<6; i++)
    {
        dat[i] = spi_xfer_byte(0);
    }
    SPI_DIS();
}

/**************************************************************************/
/*!

*/
/**************************************************************************/
void adxl_init()
{
    is_adxl_on = true;
    // set power bit
    SPI_ENB();
    spi_xfer_byte(0x2d);
    spi_xfer_byte(0x28);
    SPI_DIS();

    // set full resolution
    SPI_ENB();
    spi_xfer_byte(0x31);
    spi_xfer_byte(0x08);
    SPI_DIS();

    // clear fifo
    SPI_ENB();
    spi_xfer_byte(0x38);
    spi_xfer_byte(0x00);
    SPI_DIS();
}


