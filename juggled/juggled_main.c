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

#include "juggled_main.h"
#include "cmd.h"
#include "chb.h"
#include "at86rf230/chb_drvr.h" // for short address
#include "juggling.h"
#include "adxl345.h"

static U32 adxl_read_count = 0;
static U32 adxl_transmit_count = 0;

#define CYCLES_PER_TRANSMIT 1000

READ_WRITE_FLAG__FLAG_IMP_DEFAULT(read_adxl, true);
READ_WRITE_FLAG__FLAG_IMP_DEFAULT(print, false);
READ_WRITE_FLAG__FLAG_IMP_DEFAULT(transmit, true);

static U16 time_to_transmit = false;

#define ALIVE_TOTAL_CYCLES (1<<13)
#define ALIVE_ON_CYCLES (1<<9)



/**************************************************************************/
/*!

 Globals and small helpers for main
*/
/**************************************************************************/
static U8 dat[ADXL_PACKET_LENGTH+2]; // data of packet to be sent
static U8* read_start = &dat[ADXL_MAGIC_LENGTH];

void enable_adxl_interrupt();

void main_read_adxl()
{
    // reinit adxl
    //adxl_init();
    // read adxl
    adxl_read_count++;
    adxl_multi_read(ADXL345_DATAX0, read_start, 6);
    acc_X = (read_start[0]<<8) + read_start[1];
    acc_Y = (read_start[2]<<8) + read_start[3];
    acc_Z = (read_start[4]<<8) + read_start[5];
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

    cmd_init();

    chb_init();

    adxl_init();

    enable_adxl_interrupt();

    // turn off the led
    PORTC &= ~(1<<PORTC7);

    // set our short address - I think its eeprom, so only set it if it
    // isn't that already
    if (chb_get_short_addr() != JUGGLED_SHORT_ADDRESS) {
        chb_set_short_addr(JUGGLED_SHORT_ADDRESS);
    }

    // don't ask me why this is done twice. It just is. I WILL INVESTIGATE!
    //spi_init();

    // turn on the led
    PORTC |= 1<<PORTC7;

    dat[0] = ADXL_MAGIC_BYTE_1;
    dat[1] = ADXL_MAGIC_BYTE_2;
    dat[2] = ADXL_MAGIC_BYTE_1;
    dat[3] = ADXL_MAGIC_BYTE_2;
    dat[4] = ADXL_MAGIC_BYTE_1;
    dat[5] = ADXL_MAGIC_BYTE_2;
    dat[6] = ADXL_MAGIC_BYTE_1;
    dat[7] = ADXL_MAGIC_BYTE_2;
    
    while (1)
    {
        ++t;
        if ((t % ALIVE_TOTAL_CYCLES) == 0 ||
            (t % ALIVE_TOTAL_CYCLES) == ALIVE_ON_CYCLES) {
            PORTC ^= 1<<PORTC7;
        }
        cmd_poll();

        if (read_adxl)
        {
            //main_read_adxl();

            // write packet - just the raw bytes from SPI will do.
            if (transmit && time_to_transmit <= 0) {
                adxl_transmit_count++;
                time_to_transmit = CYCLES_PER_TRANSMIT;
                // reinit chibi stack (no need to reset address)
                //chb_init();
                chb_write(STATIONARY_SHORT_ADDRESS, dat, ADXL_PACKET_LENGTH);
            }
            if (time_to_transmit > 0) {
                --time_to_transmit;
            }
            if (print) {
                printf_P(PSTR("%d\t%d %d\t%d %d\t%d %d\t%d %d\t%d\n"),
                         adxl_read_count, dat[0], dat[1], dat[2], dat[3],
                         dat[4], dat[5], dat[6], dat[7], dat[8]);
            }

            //_delay_ms(1); // ok, magic delay. not really magic - works without. But transmit doesn't.
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
    val = adxl_read(addr);

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

    adxl_write(addr, val);
    // readback for confirm
    val = adxl_read(addr);
    
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
    char str[10];
    sprintf(str, "%u", test_count++);
    //chb_init();
    chb_write(STATIONARY_SHORT_ADDRESS, (U8*)str, strlen(str));
}

READ_WRITE_FLAG__CMD_IMPL(read_adxl);
READ_WRITE_FLAG__CMD_IMPL(print);
READ_WRITE_FLAG__CMD_IMPL(transmit);


// Interrupt based adxl reads
// issues:
//  SPI should be disabled for other chips and enabled only for adxl
//   - what port enables adxl?
//   - what port disables others?
//   - who are the others


// timer1 value to generate a 100 msec interrupt based on 0.125 usec 
// increment
#define TIMER_CNT (0xffff-0x320)

static volatile bool update = false;

void enable_adxl_interrupt()
{
    // init timer0 and set up for 100 msec timer interrupt
    TCNT1   = TIMER_CNT;                        
    TIMSK1  = _BV(TOIE1);               // enable the timer 0 overflow interrupt 
    TCCR1B  |= _BV(CS12) | _BV(CS10);   // init clock prescaler to 0.125 msec increment
}

void cmd_enable_adxl_interrupt(U8 argc, char **argv)
{
    enable_adxl_interrupt();
}

/**************************************************************************/
/*!
    Interrupt service routine
*/
/**************************************************************************/
ISR(TIMER1_OVF_vect)
{
    if (read_adxl) {
        // set the update flag so the main loop that data is updated
        update = true;

        // do a multiple read
        adxl_multi_read(ADXL345_DATAX0, read_start, 6);
    }

    // reload the timer
    TCNT1 = TIMER_CNT;
}
