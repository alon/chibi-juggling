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
    \defgroup cdc_demo CDC Demo
    \file main.c
    \ingroup cdc_demo
*/
/*******************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <avr/pgmspace.h>
#include <avr/io.h>
#include <util/delay.h>
#include "main.h"
#include "cmd.h"
#include "chb.h"
#include "chb_drvr.h" // for short address
#include "juggling.h"

#define SPI_ENB() do {PORTC &= ~(_BV(PORTC6));} while(0)
#define SPI_DIS() do {PORTC |= _BV(PORTC6);} while(0)

static bool rd_adxl = true; // default to on. But allow usb cmd to change it.
static bool print_adxl = false;

#define ENABLE_TRANSCEIVER

#ifdef ENABLE_TRANSCEIVER
static bool transmit_adxl = true;
#endif

U16 acc_X = 0, acc_Y = 0, acc_Z = 0;

/**************************************************************************/
/*!

*/
/**************************************************************************/
void spi_init()
{
    // configure the SPI slave_select, spi clk, and mosi pins as output. the miso pin
    // is cleared since its an input.
    DDRC |= _BV(PORTC6);
    PORTC |= _BV(PORTC6);

    DDRB |= _BV(PORTB1) | _BV(PORTB2);
    PORTB |= _BV(PORTB1); 

    // set to master mode
    // set clock polarity to be high idle
    // set clock phase to sample on trailing edge of sclk
    // set the clock freq to fck/128.
    SPCR = _BV(MSTR) | _BV(CPOL) | _BV(CPHA);
    SPSR = _BV(SPI2X);

    // enable the SPI master
    SPCR |= (1 << SPE);
}

/**************************************************************************/
/*!
    This function both reads and writes data. For write operations, include data
    to be written as argument. For read ops, use dummy data as arg. Returned
    data is read byte val.
*/
/**************************************************************************/
U8 spi_xfer_byte(U8 data)
{
    SPDR = data;
    while (!(SPSR & (1<<SPIF)));
    return SPDR;
}

/**************************************************************************/
/*!

*/
/**************************************************************************/
void adxl_init()
{
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

/**************************************************************************/
/*!
    This is the main function. Duh.
*/
/**************************************************************************/
int main()
{
    // turn on the led
    DDRC |= 1<<PORTC7;
    PORTC |= 1<<PORTC7;

    cmd_init();

    // turn off the led
    PORTC &= ~(1<<PORTC7);

#ifdef ENABLE_TRANSCEIVER
    chb_init();
#endif

    // init the spi
    spi_init();
    adxl_init();

#ifdef ENABLE_TRANSCEIVER
    // set our short address - I think its eeprom, so only set it if it
    // isn't that already
    if (chb_get_short_addr() != JUGGLED_SHORT_ADDRESS)
        chb_set_short_addr(JUGGLED_SHORT_ADDRESS);
#endif

    // turn on the led
    PORTC |= 1<<PORTC7;

    // and off we go...
    while (1)
    {
        cmd_poll();

        if (rd_adxl)
        {
            U8 i, dat[6];

            // read adxl
            SPI_ENB();
            spi_xfer_byte(0xf2);

            for (i=0; i<6; i++)
            {
                dat[i] = spi_xfer_byte(0);
            }
            SPI_DIS();
            // write packet - just the raw bytes from SPI will do.
#ifdef ENABLE_TRANSCEIVER
            if (transmit_adxl)
                chb_write(STATIONARY_SHORT_ADDRESS, dat, 6);
#endif

            acc_X = (dat[1] << 8) + dat[0]; // TODO - or reverse?
            acc_Y = (dat[3] << 8) + dat[2]; // TODO - or reverse?
            acc_Z = (dat[5] << 8) + dat[4]; // TODO - or reverse?

            if (print_adxl)
                printf_P(PSTR("%d\t%d\t%d\n"), acc_X, acc_Y, acc_Z);

            //_delay_ms(100);
        }
    }
}

/**************************************************************************/
/*!

*/
/**************************************************************************/
void cmd_reg_read(U8 argc, char **argv)
{
    U8 addr, val;

    addr = strtol(argv[1], NULL, 16);
    addr |= (1<<7);         // set the read bit in the first transfer

    SPI_ENB();
    val = spi_xfer_byte(addr);      // send the address
    val = spi_xfer_byte(val);       // send dummy data. we just want the return data.
    SPI_DIS();

    printf_P(PSTR("Reg Read: %04X, %02X.\n"), addr, val);
}

/**************************************************************************/
/*!

*/
/**************************************************************************/
void cmd_reg_write(U8 argc, char **argv)
{
    U8 addr, val;

    addr = strtol(argv[1], NULL, 16);
    val = strtol(argv[2], NULL, 16);

    // make sure the write bit is cleared in the addr
    addr &= ~(1<<7);

    SPI_ENB();
    spi_xfer_byte(addr);
    spi_xfer_byte(val);
    SPI_DIS();
    
    printf_P(PSTR("Write: %04X, %02X.\n"), addr, val);
}

/**************************************************************************/
/*!

*/
/**************************************************************************/
void cmd_enb_adxl(U8 argc, char **argv)
{
    U8 val = strtol(argv[1], NULL, 10);
    rd_adxl = val;
}

/**************************************************************************/
/*!

*/
/**************************************************************************/
void cmd_enb_print(U8 argc, char **argv)
{
    U8 val = strtol(argv[1], NULL, 10);
    print_adxl = val;
}

/**************************************************************************/
/*!

*/
/**************************************************************************/
void cmd_enb_transmit(U8 argc, char **argv)
{
    U8 val = strtol(argv[1], NULL, 10);
#ifdef ENABLE_TRANSCEIVER
    transmit_adxl = val;
#endif
}

/**************************************************************************/
/*!

*/
/**************************************************************************/
void cmd_set_short_addr(U8 argc, char **argv)
{
    U16 addr = strtol(argv[1], NULL, 16);
    chb_set_short_addr(addr);
}

/**************************************************************************/
/*!

*/
/**************************************************************************/
void cmd_get_short_addr(U8 argc, char **argv)
{
    U16 addr = chb_get_short_addr();
    printf_P(PSTR("Short Addr = %04X.\n"), addr);
}


