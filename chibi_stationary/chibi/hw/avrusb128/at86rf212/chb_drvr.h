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
#ifndef CHIBI_DRVR_H
#define CHIBI_DRVR_H

#include <avr/io.h>
#include <avr/interrupt.h>
#include "types.h"

#define CHB_CHINA               0
#define CHB_EEPROM_IEEE_ADDR    0x00
#define CHB_EEPROM_SHORT_ADDR   0x09
#define CHB_AT86RF212_VER_NUM   0x01
#define CHB_AT86RF212_PART_NUM  0x07

#define CHB_SPI_CMD_RW      0xC0    /**<  Register Write (short mode). */
#define CHB_SPI_CMD_RR      0x80    /**<  Register Read (short mode). */
#define CHB_SPI_CMD_FW      0x60    /**<  Frame Transmit Mode (long mode). */
#define CHB_SPI_CMD_FR      0x20    /**<  Frame Receive Mode (long mode). */
#define CHB_SPI_CMD_SW      0x40    /**<  SRAM Write. */
#define CHB_SPI_CMD_SR      0x00    /**<  SRAM Read. */
#define CHB_SPI_CMD_RADDRM  0x7F    /**<  Register Address Mask. */

#define CHB_IRQ_BAT_LOW_MASK        0x80  /**< Mask for the BAT_LOW interrupt. */
#define CHB_IRQ_TRX_UR_MASK         0x40  /**< Mask for the TRX_UR interrupt. */
#define CHB_IRQ_TRX_END_MASK        0x08  /**< Mask for the TRX_END interrupt. */
#define CHB_IRQ_RX_START_MASK       0x04  /**< Mask for the RX_START interrupt. */
#define CHB_IRQ_PLL_UNLOCK_MASK     0x02  /**< Mask for the PLL_UNLOCK interrupt. */
#define CHB_IRQ_PLL_LOCK_MASK       0x01  /**< Mask for the PLL_LOCK interrupt. */

#define CHB_RSTPORT     PORTB
//#define CHB_RSTPIN      4           
#define CHB_RSTPIN      7           // changed for FL-AVRUSB128-DEV v1.1
#define CHB_SLPTRPORT   PORTB
#define CHB_SLPTRPIN    5
#define CHB_DDR_SLPTR   DDRB
#define CHB_DDR_RST     DDRB
#define CHB_RADIO_IRQ   INT4_vect

// enable rising edge interrupt on IRQ0
#define CFG_CHB_INTP_RISE_EDGE() do {EICRB |= _BV(ISC40) | _BV(ISC41); EIMSK |= _BV(INT4);} while(0)
    
#define CHB_ENTER_CRIT()    {U8 volatile saved_sreg = SREG; cli()
#define CHB_LEAVE_CRIT()    SREG = saved_sreg;}
#define CHB_RST_ENABLE()    do {CHB_RSTPORT &= ~(_BV(CHB_RSTPIN));} while (0)
#define CHB_RST_DISABLE()   do {CHB_RSTPORT |= (_BV(CHB_RSTPIN));} while (0)
#define CHB_SLPTR_ENABLE()  do {CHB_SLPTRPORT |= (_BV(CHB_SLPTRPIN));} while (0)
#define CHB_SLPTR_DISABLE() do {CHB_SLPTRPORT &= ~(_BV(CHB_SLPTRPIN));} while (0)

// CCA constants
enum
{
    CCA_ED                    = 1,    /**< Use energy detection above threshold mode. */
    CCA_CARRIER_SENSE         = 2,    /**< Use carrier sense mode. */
    CCA_CARRIER_SENSE_WITH_ED = 3     /**< Use a combination of both energy detection and carrier sense. */
};

// configuration parameters
enum
{
    CHB_CHANNEL             = 1,
    CHB_PAN_ID              = 0x1234,
    CHB_TX_PWR              = 0x0,
    CHB_SHORT_ADDR          = 0x0,
    CHB_IEEE_ADDR           = 0x0,
    CHB_MAX_FRAME_RETRIES   = 3,
    CHB_MAX_CSMA_RETRIES    = 4,
    CHB_CCA_MODE            = CCA_ED,
    CHB_MIN_BE              = 3,
    CHB_MAX_BE              = 5,
    CHB_CCA_ED_THRES        = 0x7,
    CHB_CSMA_SEED0          = 0,
    CHB_CSMA_SEED1          = 0,
    CHB_FRM_VER             = 1         // accept 802.15.4 ver 0 or 1 frames
};

// register addresses
enum
{
    TRX_STATUS              = 0x01,
    TRX_STATE               = 0x02,
    TRX_CTRL_0              = 0x03,
    TRX_CTRL_1              = 0x04,
    PHY_TX_PWR              = 0x05,
    PHY_RSSI                = 0x06,
    PHY_ED_LEVEL            = 0x07,
    PHY_CC_CCA              = 0x08,
    CCA_THRES               = 0x09,
    RX_CTRL                 = 0x0a,
    SFD_VALUE               = 0x0b,
    TRX_CTRL_2              = 0x0c,
    ANT_DIV                 = 0x0d,
    IRQ_MASK                = 0x0e,
    IRQ_STATUS              = 0x0f,
    VREG_CTRL               = 0x10,
    BATMON                  = 0x11,
    XOSC_CTRL               = 0x12,
    CC_CTRL_0               = 0x13,
    CC_CTRL_1               = 0x14,
    RX_SYN                  = 0x15,
    RF_CTRL_0               = 0x16,
    XAH_CTRL_1              = 0x17,
    FTN_CTRL                = 0x18,
    RF_CTRL_1               = 0x19,
    PLL_CF                  = 0x1a,
    PLL_DCU                 = 0x1b,
    PART_NUM                = 0x1c,
    VERSION_NUM             = 0x1d,
    MAN_ID_0                = 0x1e,
    MAN_ID_1                = 0x1f,
    SHORT_ADDR_0            = 0x20,
    SHORT_ADDR_1            = 0x21,
    PAN_ID_0                = 0x22,
    PAN_ID_1                = 0x23,
    IEEE_ADDR_0             = 0x24,
    IEEE_ADDR_1             = 0x25,
    IEEE_ADDR_2             = 0x26,
    IEEE_ADDR_3             = 0x27,
    IEEE_ADDR_4             = 0x28,
    IEEE_ADDR_5             = 0x29,
    IEEE_ADDR_6             = 0x2a,
    IEEE_ADDR_7             = 0x2b,
    XAH_CTRL_0              = 0x2c,
    CSMA_SEED_0             = 0x2d,
    CSMA_SEED_1             = 0x2e,
    CSMA_BE                 = 0x2f
};

// random defines
enum
{
    CHB_MAX_FRAME_RETRIES_POS   = 4,
    CHB_MAX_CSMA_RETIRES_POS    = 1,
    CHB_CSMA_SEED1_POS          = 0,
    CHB_CCA_MODE_POS            = 5,
    CHB_AUTO_CRC_POS            = 5,
    CHB_TRX_END_POS             = 3,
    CHB_TRAC_STATUS_POS         = 5,
    CHB_FVN_POS                 = 6,
    CHB_OQPSK_TX_OFFSET         = 2,
    CHB_BPSK_TX_OFFSET          = 3,
    CHB_MIN_FRAME_LENGTH        = 3,
    CHB_MAX_FRAME_LENGTH        = 0x7f,
};

// transceiver timing
enum{
    TIME_RST_PULSE_WIDTH        = 1, 
    TIME_P_ON_TO_CLKM_AVAIL     = 380,
    TIME_SLEEP_TO_TRX_OFF       = 240,
    TIME_TRX_OFF_TO_SLEEP       = 35,
    TIME_PLL_ON_TRX_OFF         = 1,
    TIME_TRX_OFF_RX_ON          = 110,
    TIME_RX_ON_TRX_OFF          = 1,
    TIME_PLL_ON_RX_ON           = 1,
    TIME_RX_ON_PLL_ON           = 1,
    TIME_PLL_LOCK_TIME          = 110,
    TIME_BUSY_TX_PLL_ON         = 32,
    TIME_ALL_STATES_TRX_OFF     = 1,
    TIME_RESET_TRX_OFF          = 26,
    TIME_TRX_IRQ_DELAY          = 9,
    TIME_TRX_OFF_PLL_ON         = 110,
    TIME_IRQ_PROCESSING_DLY     = 32
};

// trac status
enum{
    TRAC_SUCCESS               = 0,
    TRAC_SUCCESS_DATA_PENDING  = 1,
    TRAC_WAIT_FOR_ACK          = 2,
    TRAC_CHANNEL_ACCESS_FAIL   = 3,
    TRAC_NO_ACK                = 5,
    TRAC_INVALID               = 7
};

// radio statuses
enum{
    RADIO_SUCCESS = 0x40,                       /**< The requested service was performed successfully. */
    RADIO_UNSUPPORTED_DEVICE,                   /**< The connected device is not an Atmel AT86RF212. */
    RADIO_INVALID_ARGUMENT,                     /**< One or more of the supplied function arguments are invalid. */
    RADIO_TIMED_OUT,                            /**< The requested service timed out. */
    RADIO_WRONG_STATE,                          /**< The end-user tried to do an invalid state transition. */
    RADIO_BUSY_STATE,                           /**< The radio transceiver is busy receiving or transmitting. */
    RADIO_STATE_TRANSITION_FAILED,              /**< The requested state transition could not be completed. */
    RADIO_CCA_IDLE,                             /**< Channel is clear, available to transmit a new frame. */
    RADIO_CCA_BUSY,                             /**< Channel busy. */
    RADIO_TRX_BUSY,                             /**< Transceiver is busy receiving or transmitting data. */
    RADIO_BAT_LOW,                              /**< Measured battery voltage is lower than voltage threshold. */
    RADIO_BAT_OK,                               /**< Measured battery voltage is above the voltage threshold. */
    RADIO_CRC_FAILED,                           /**< The CRC failed for the actual frame. */
    RADIO_CHANNEL_ACCESS_FAILURE,               /**< The channel access failed during the auto mode. */
    RADIO_NO_ACK,                               /**< No acknowledge frame was received. */
};

// transceiver commands
enum
{
    CMD_NOP                 = 0,
    CMD_TX_START            = 2,
    CMD_FORCE_TRX_OFF       = 3,
    CMD_FORCE_PLL_ON        = 4,
    CMD_RX_ON               = 6,
    CMD_TRX_OFF             = 8,
    CMD_PLL_ON              = 9,
    CMD_RX_AACK_ON          = 22,
    CMD_TX_ARET_ON          = 25
};

// transceiver states
enum
{
    P_ON               = 0,
    BUSY_RX            = 1,
    BUSY_TX            = 2,
    RX_ON              = 6,
    TRX_OFF            = 8,
    PLL_ON             = 9,
    SLEEP              = 15,
    BUSY_RX_AACK       = 17,
    BUSY_TX_ARET       = 18,
    RX_AACK_ON         = 22,
    TX_ARET_ON         = 25,
    RX_ON_NOCLK        = 28,
    RX_AACK_ON_NOCLK   = 29,
    BUSY_RX_AACK_NOCLK = 30,
    TRANS_IN_PROG      = 31
};

// transceiver interrupt register
enum 
{
    IRQ_PLL_LOCK                = 0,
    IRQ_PLL_UNLOCK              = 1,
    IRQ_RX_START                = 2,
    IRQ_TRX_END                 = 3,
    IRQ_CCA_ED_READY            = 4,
    IRQ_AMI                     = 5,
    IRQ_TRX_UR                  = 6,
    IRQ_BAT_LOW                 = 7 
}; 

// transceiver modes
enum
{
    OQPSK_868MHZ    = 0,
    OQPSK_915MHZ    = 1,
    OQPSK_780MHZ    = 2,
    BPSK40_915MHZ   = 3
};

// init 
void chb_drvr_init();

// data access
U8 chb_reg_read(U8 addr);
U16 chb_reg_read16(U8 addr);
void chb_reg_write(U8 addr, U8 val);
void chb_reg_write16(U8 addr, U16 val);
void chb_reg_write64(U8 addr, U8 *val);
void chb_reg_read_mod_write(U8 addr, U8 val, U8 mask);
void chb_frame_write(U8 *hdr, U8 hdr_len, U8 *data, U8 data_len);

// general configuration
void chb_set_mode(U8 mode);
U8 chb_set_channel(U8 channel);
void chb_set_pwr(U8 val);
void chb_set_ieee_addr(U8 *addr);
void chb_get_ieee_addr(U8 *addr);
void chb_set_short_addr(U16 addr);
U16 chb_get_short_addr();
U8 chb_set_state(U8 state);

// data transmit
U8 chb_tx(U8 *hdr, U8 *data, U8 len);

#ifdef CHB_DEBUG
// sram access
void chb_sram_read(U8 addr, U8 len, U8 *data);
void chb_sram_write(U8 addr, U8 len, U8 *data);
#endif

#endif

