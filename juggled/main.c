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
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <avr/pgmspace.h>
#include <avr/io.h>
#include <util/delay.h>

#include "main.h"
#include "cmd.h"
#include "chb.h"
#include "at86rf230/chb_drvr.h" // for short address
#include "juggling.h"
#include "adxl.h"
#include "spi.h"

static bool rd_adxl = true; // default to on. But allow usb cmd to change it.

U32 adxl_read_count = 0;
U32 adxl_transmit_count = 0;

#define CYCLES_PER_TRANSMIT 200

READ_WRITE_FLAG__FLAG_IMP(read_adxl_flag);
READ_WRITE_FLAG__FLAG_IMP_DEFAULT(adxl_flag, false);
READ_WRITE_FLAG__FLAG_IMP_DEFAULT(print_flag, false);
READ_WRITE_FLAG__FLAG_IMP_DEFAULT(transmit_flag, true);

static bool transmit_adxl = true;
static U16 time_to_transmit = false;

#define CYCLES_INIT_DONE 10000



/**************************************************************************/
/*!

 Globals and small helpers for main
*/
/**************************************************************************/
U8 dat[ADXL_PACKET_LENGTH]; // data of packet to be sent
U8* read_start = dat + ADXL_MAGIC_LENGTH;

void main_read_adxl()
{
    // reinit adxl
    spi_init();
    adxl_init();
    // read adxl
    adxl_read_count++;
    read_adxl(read_start);
    acc_X = (read_start[1] << 8)
           + read_start[0];
    acc_Y = (read_start[3] << 8)
           + read_start[2];
    acc_Z = (read_start[5] << 8)
           + read_start[4];
}

/**************************************************************************/
/*!
    This is the main function. Duh.
*/
/**************************************************************************/
int main()
{
    // lame time counter, should be using timer interrupt
    unsigned int t = 0;
    // turn on the led
    DDRC |= 1<<PORTC7;
    PORTC |= 1<<PORTC7;

    spi_init();
    adxl_init();

    cmd_init();

    // turn off the led
    PORTC &= ~(1<<PORTC7);

    chb_init();

    // set our short address - I think its eeprom, so only set it if it
    // isn't that already
    if (chb_get_short_addr() != JUGGLED_SHORT_ADDRESS)
        chb_set_short_addr(JUGGLED_SHORT_ADDRESS);

    spi_init(); // don't ask me why this is done twice. It just is. I WILL INVESTIGATE!

    // turn on the led
    PORTC |= 1<<PORTC7;

    dat[0] = ADXL_MAGIC_BYTE_1;
    dat[1] = ADXL_MAGIC_BYTE_2;
    
    bool initing = true;

    while (1)
    {
        ++t;
        if (initing && t == CYCLES_INIT_DONE) {
            initing = false;
            PORTC &= ~(1<<PORTC7);
            time_to_transmit = CYCLES_PER_TRANSMIT;
        }
        cmd_poll();

        if (rd_adxl)
        {
            main_read_adxl();

            // write packet - just the raw bytes from SPI will do.
            if (transmit_flag && time_to_transmit <= 0) {
                adxl_transmit_count++;
                time_to_transmit = CYCLES_PER_TRANSMIT;
                // reinit chibi stack (no need to reset address)
                chb_init();
                chb_write(STATIONARY_SHORT_ADDRESS, dat, ADXL_PACKET_LENGTH);
            }
            if (time_to_transmit > 0) --time_to_transmit;
            if (print_flag)
                printf_P(PSTR("%d %d\t%d\t%d\n"), adxl_read_count, acc_X, acc_Y, acc_Z);

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

/**************************************************************************/
/*!

*/
/**************************************************************************/
void cmd_who(U8 argc, char **argv)
{
    printf_P(PSTR("Juggled"));
}


/**************************************************************************/
/*!

*/
/**************************************************************************/
void cmd_print_adxl_transmit_count(U8 argc, char **argv)
{
    printf_P(PSTR("adxl transmitted %d\n"), adxl_transmit_count);
}


/**************************************************************************/
/*!

*/
/**************************************************************************/
void cmd_print_adxl_read_count(U8 argc, char **argv)
{
    printf_P(PSTR("adxl read %d\n"), adxl_read_count);
}


void cmd_send_test_message(U8 argc, char **argv)
{
    static U8 test_count = 0;
    U8 str[10];
    sprintf(str, "%d", test_count++);
    chb_init();
    chb_write(STATIONARY_SHORT_ADDRESS, str, strlen(str));
}

READ_WRITE_FLAG__CMD_IMPL(read_adxl_flag);
READ_WRITE_FLAG__CMD_IMPL(adxl_flag);
READ_WRITE_FLAG__CMD_IMPL(print_flag);
READ_WRITE_FLAG__CMD_IMPL(transmit_flag);

