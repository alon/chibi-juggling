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
#include <avr/pgmspace.h>
#include "chb.h"
#include "chb_demo.h"
#include "chb_buf.h"
#include "chb_drvr.h"
#include "freakusb.h"
#include "cmd.h"

/**************************************************************************/
/*!

*/
/**************************************************************************/
int main()
{ 
    pcb_t *pcb = chb_get_pcb();
    chb_rx_data_t rx_data;

    // init the command line
    cmd_init();

    // init the chibi stack
    chb_init();

    // and off we go...
    while (1)
    {
        cmd_poll();

        if (pcb->data_rcv)
        {
            // get the length of the data
            rx_data.len = chb_read(&rx_data);
            printf_P(PSTR("Message received from node %02X: %s, len=%d, rssi=%02X.\n"), rx_data.src_addr, rx_data.data, rx_data.len, pcb->ed);
        }
    }
    return 0;
}

/**************************************************************************/
/*!

*/
/**************************************************************************/
void cmd_hello(U8 argc, char **argv)
{
    printf_P(PSTR("Hello World!\n"));
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
void cmd_set_ieee_addr(U8 argc, char **argv)
{
    U8 i, addr[8];

    memset(addr, 0, 8);
    for (i=0; i<argc-1; i++)
    {
        addr[i] = strtol(argv[i+1], NULL, 16);
    }
    chb_set_ieee_addr(addr);
}

/**************************************************************************/
/*!

*/
/**************************************************************************/
void cmd_get_ieee_addr(U8 argc, char **argv)
{
    U8 i, addr[8];

    chb_get_ieee_addr(addr);
    
    printf_P(PSTR("IEEE Addr = "));
    for (i=8; i>0; i--)
    {
        printf("%02X", addr[i-1]);
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
    val = chb_reg_read(addr);
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

    chb_reg_write(addr, val);
    printf_P(PSTR("Write: %04X, %02X.\n"), addr, val);

    val = chb_reg_read(addr);
    printf_P(PSTR("Readback: %04X, %02X.\n"), addr, val);
}

/**************************************************************************/
/*!

*/
/**************************************************************************/
void cmd_reg_dump(U8 argc, char **argv)
{
    U8 i, val;

    for (i=0; i<0x2e; i++)
    {
        val = chb_reg_read(i);
        printf_P(PSTR("Reg Read: %04X, %02X.\n"), i, val);
    }
}

/**************************************************************************/
/*!

*/
/**************************************************************************/
void cmd_tx(U8 argc, char **argv)
{
    U8 i, len, *data_ptr, data[50];
    U16 addr;

    addr = strtol(argv[1], NULL, 16);

    data_ptr = data;
    for (i=0; i<argc-2; i++)
    {
        len = strlen(argv[i+2]);
        strcpy((char *)data_ptr, (char *)argv[i+2]);
        data_ptr += len;
        *data_ptr++ = ' ';
    }
    *data_ptr++ = '\0';

    chb_write(addr, data, data_ptr - data);
}


#ifdef CHB_AT86RF212
/**************************************************************************/
/*!
    Set the mode of the transceiver, ie: BPSK, OQPSK, etc...
*/
/**************************************************************************/
void cmd_set_mode(U8 argc, char **argv)
{
    U8 mode = strtol(argv[1], NULL, 10);

    chb_set_state(TRX_OFF);

    chb_set_mode(mode);
    chb_set_channel(1);

    chb_set_state(RX_AACK_ON);
}

/**************************************************************************/
/*!
    Set the power level of the transceiver
*/
/**************************************************************************/
void cmd_set_pwr(U8 argc, char **argv)
{
    U8 pwr = strtol(argv[1], NULL, 10);
    U8 val;

    switch (pwr)
    {
    case 10: val = 0xc0; break;
    case 9: val = 0xa1; break;
    case 8: val = 0x81; break;
    case 7: val = 0x82; break;
    case 6: val = 0x83; break;
    case 5: val = 0x60; break;
    case 4: val = 0x61; break;
    case 3: val = 0x41; break;
    case 2: val = 0x42; break;
    case 1: val = 0x22; break;
    case 0: val = 0x23; break;
    default: return;
    }
    chb_set_pwr(val);
}
#endif
