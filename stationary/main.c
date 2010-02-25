/*******************************************************************
 Copyright Alon Levy 2010

 Based on code written by Christopher Wang aka Akiba.
*******************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <avr/pgmspace.h>
#include <avr/io.h>
#include <util/delay.h>
#include "main.h"
#include "cmd_tbl.h"
#include "cmd.h"
#include "chb.h"
#include "at86rf230/chb_drvr.h"

#include "juggling.h"

static U16 acc_X = 0, acc_Y = 0, acc_Z = 0;
static bool print_adxl = false;
static bool do_receive = true;

/**************************************************************************/
/*!
    This is the main function. Duh.
*/
/**************************************************************************/
int main()
{
    pcb_t *pcb = chb_get_pcb();
    chb_rx_data_t rx_data;

    cmd_init();
    chb_init();

    // set our short address - I think its eeprom, so only set it if it
    // isn't that already
    U8 old_addr = chb_get_short_addr();
    if (old_addr != STATIONARY_SHORT_ADDRESS) {
        printf_P(PSTR("Changing short address from %d to %d\n"), old_addr, STATIONARY_SHORT_ADDRESS);
        chb_set_short_addr(STATIONARY_SHORT_ADDRESS);
    }

    // turn on the led
    DDRC |= 1<<PORTC7;
    PORTC |= 1<<PORTC7;

    // and off we go...
    while (1)
    {
        cmd_poll();

        if (do_receive && pcb->data_rcv)
        {
            rx_data.len = chb_read(&rx_data);
            // check also for expected length + 2 (known chibi bug)
            if ((rx_data.len != ADXL_PACKET_LENGTH && rx_data.len != ADXL_PACKET_LENGTH + 2) || rx_data.data[0] != ADXL_MAGIC_BYTE_1
                        || rx_data.data[1] != ADXL_MAGIC_BYTE_2) {
                if (rx_data.len != ADXL_PACKET_LENGTH && rx_data.len != ADXL_PACKET_LENGTH + 2)
                    printf_P(PSTR("Bad packet length, "));
                if (rx_data.data[0] != ADXL_MAGIC_BYTE_1)
                    printf_P(PSTR("Bad first byte, "));
                if (rx_data.data[1] != ADXL_MAGIC_BYTE_2)
                    printf_P(PSTR("Bad second byte, "));
                printf_P(PSTR("Non adxl (src %02X), #%d:"),
                    rx_data.src_addr, rx_data.len); //rx_data.data);
                for (U8 i=0; i<rx_data.len; i+=2) {
                    if (i+1 < rx_data.len)
                        printf_P(PSTR("%X, "), rx_data.data[i] + (rx_data.data[i+1]<<8));
                    else // last byte in an odd length string
                        printf_P(PSTR("%X, "), rx_data.data[i]);
                }
                printf_P(PSTR("\nStopping Receive\n"));
                do_receive = false;
            } else { // assume this is our juggling friend, interpret as three axes accel:
                acc_X = (rx_data.data[ADXL_MAGIC_LENGTH+1] << 8)
                       + rx_data.data[ADXL_MAGIC_LENGTH+0];
                acc_Y = (rx_data.data[ADXL_MAGIC_LENGTH+3] << 8)
                       + rx_data.data[ADXL_MAGIC_LENGTH+2];
                acc_Z = (rx_data.data[ADXL_MAGIC_LENGTH+5] << 8)
                       + rx_data.data[ADXL_MAGIC_LENGTH+4];
            }
            pcb->data_rcv = false;
            if (print_adxl)
                printf_P(PSTR("got %d, %d, %d\n"), acc_X, acc_Y, acc_Z);
        }

        _delay_ms(10);
    }
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
void cmd_set_short_addr(U8 argc, char **argv)
{
    if (argc != 2) return;
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
    printf_P(PSTR("Stationary"));
}

/**************************************************************************/
/*!

*/
/**************************************************************************/
void cmd_is_receiving(U8 argc, char **argv)
{
    printf_P(PSTR("do_receive = %d"), (int)do_receive);
}

/**************************************************************************/
/*!

*/
/**************************************************************************/
void cmd_recv(U8 argc, char **argv)
{
    if (argc != 2) return;
    do_receive = strtol(argv[1], NULL, 16);
}

