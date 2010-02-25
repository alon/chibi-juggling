#include <avr/io.h>

#include "spi.h"

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


