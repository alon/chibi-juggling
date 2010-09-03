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
#include <avr/io.h>
#include "spi.h"
#include "types.h"

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
