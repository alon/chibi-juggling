/*******************************************************************
    Copyright (C) 2009 FreakLabs
    All rights reserved.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions
    are met:

    1. Redistributions of source code must retain the above copyright
       notice, this list of conditions and the following disclaimer.
    2. Redistributions in binary form must reproduce the above copyright
       notice, this list of conditions and the following disclaimer in the
       documentation and/or other materials provided with the distribution.
    3. Neither the name of the the copyright holder nor the names of its contributors
       may be used to endorse or promote products derived from this software
       without specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS IS'' AND
    ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
    IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
    ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
    FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
    DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
    OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
    HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
    LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
    OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
    SUCH DAMAGE.

    Originally written by Christopher Wang aka Akiba.
    Please post support questions to the FreakLabs forum.

*******************************************************************/
/*!
    \file
    \ingroup


*/
/**************************************************************************/
#include "adxl345.h"
#include "spi.h"

// ------------ ADXL Control ----------------------------------------------

U16 acc_X = 0, acc_Y = 0, acc_Z = 0;
volatile bool is_adxl_on = false;

/**************************************************************************/
/*!
Personal init
*/
/**************************************************************************/
void adxl_init()
{
    is_adxl_on = true;
    spi_init();
    // set power bit
    adxl_write(ADXL345_POWER_CTL, ADXL345_MEASURE);

    // bandwidth
    adxl_write(ADXL345_BW_RATE, ADXL345_100HZ_OUTPUT);

    //SPI_ENB();
    //spi_xfer_byte(ADXL345_POWER_CTL);
    //spi_xfer_byte(ADXL345_LINK | ADXL345_MEASURE);
    //SPI_DIS();

    // set full resolution
    adxl_write(ADXL345_DATA_FORMAT, ADXL345_FULLRES | ADXL345_8G);
    //SPI_ENB();
    //spi_xfer_byte(ADXL345_DATA_FORMAT);
    //spi_xfer_byte(0x08);
    //SPI_DIS();

    // clear and disable the fifo
    adxl_write(ADXL345_FIFO_CTL, 0x00);
    //SPI_ENB();
    //spi_xfer_byte(ADXL345_FIFO_CTL);
    //spi_xfer_byte(0x00);
    //SPI_DIS();
}

// define the interface that will be used to access the ADXL345.
// 1 = I2C Mode, 0 = SPI Mode
// v1.0 doesn't do I2C, so disable.
#define ADXL345_I2C     0

#if (ADXL345_I2C == 1)
#include "i2c.h"
#include "twimaster.h"
#endif

// define the address that the ADXL345 will use
#define ADXL345_ADDR    0x53

/**************************************************************************/
/*!
    Init the ADXL345
*/
/**************************************************************************/
void orig_adxl_init()
{
    is_adxl_on = true;
#if (ADXL345_I2C == 1)
    i2c_init();
    i2c_write((ADXL345_ADDR<<1), ADXL345_POWER_CTL, (1<<ADXL345_MEASURE));     // enable measurement
#else
    spi_init();

    // enable measurement
    SPI_ENB();
    spi_xfer_byte(ADXL345_POWER_CTL);
    spi_xfer_byte(1<<ADXL345_MEASURE);
    SPI_DIS();
#endif
}

/**************************************************************************/
/*!
    Perform single byte read to ADXL345
*/
/**************************************************************************/
U8 adxl_read(U8 addr)
{
#if (ADXL345_I2C == 1)
    U8 dev_addr = ADXL345_ADDR << 1;

    return i2c_read(dev_addr, addr);
#else
    U8 val;

    addr |= ADXL345_SPI_READ_MODE;         // set the read bit in the first transfer

    SPI_ENB();
    val = spi_xfer_byte(addr);      // send the address
    val = spi_xfer_byte(val);       // send dummy data. we just want the return data.
    SPI_DIS();

    return val;
#endif
}

/**************************************************************************/
/*!
    Perform multi byte read to ADXL345
*/
/**************************************************************************/
U8 adxl_multi_read(U8 addr, U8 *data, U8 len)
{
#if (ADXL345_I2C == 1)
    U8 dev_addr = ADXL345_ADDR << 1;

    return i2c_multi_read(dev_addr, DATAX0, data, len);
#else
    addr |= (ADXL345_SPI_READ_MODE | ADXL345_SPI_MULTI_BYTE);

    SPI_ENB();
    spi_xfer_byte(addr);

    for (U8 i=0; i<len; i++)
    {
        data[i] = spi_xfer_byte(0);
    }
    SPI_DIS();
    return 0;
#endif
}

/**************************************************************************/
/*!
    Perform single byte write to ADXL345
*/
/**************************************************************************/
U8 adxl_write(U8 addr, U8 data)
{
#if (ADXL345_I2C == 1)
    U8 dev_addr = ADXL345_ADDR << 1;

    return i2c_write(dev_addr, addr, data);
#else
    // make sure read bit is cleared
    addr &= ~ADXL345_SPI_READ_MODE;

    SPI_ENB();
    spi_xfer_byte(addr);
    spi_xfer_byte(data);
    SPI_DIS();
    return 0;
#endif
}

