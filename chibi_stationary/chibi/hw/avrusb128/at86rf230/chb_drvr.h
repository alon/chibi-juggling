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

// Set to 1 if you're using a Raven platform. Otherwise, keep at 0
#define ATMEL_RAVEN   0

#define CHB_EEPROM_IEEE_ADDR    0x00
#define CHB_EEPROM_SHORT_ADDR   0x09

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

#if (ATMEL_RAVEN == 1)
    #define CHB_RSTPORT     PORTB
    #define CHB_RSTPIN      5
    #define CHB_SLPTRPORT   PORTB
    #define CHB_SLPTRPIN    4
    #define CHB_DDR_SLPTR   DDRB
    #define CHB_DDR_RST     DDRB

    #define CHB_RADIO_IRQ   TIMER1_CAPT_vect
    
    // enable rising edge interrupt for input capture flag
    // Raven USB boards don't have ext intp line connected to radio. for some reason
    // they're using an input capture pin. prob just hw intern designed boards. we 
    // can still get interrupt by using input capture flag & triggering interrupt on each
    // rising edge capture.
    #define CFG_CHB_INTP_RISE_EDGE() do {   \
        TCCR1B = (1<<ICES1) | (1<<CS11) | (1<<CS10);    \
        TIFR1 |= (1<<ICF1);                 \
        TIMSK1 |= (1<<ICIE1);           \
        } while(0)
#else
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
#endif
    
#define CHB_ENTER_CRIT()    {U8 volatile saved_sreg = SREG; cli()
#define CHB_LEAVE_CRIT()    SREG = saved_sreg;}
#define CHB_RST_ENABLE()    do {CHB_RSTPORT &= ~(_BV(CHB_RSTPIN));} while (0)
#define CHB_RST_DISABLE()   do {CHB_RSTPORT |= (_BV(CHB_RSTPIN));} while (0)
#define CHB_SLPTR_ENABLE()  do {CHB_SLPTRPORT |= (_BV(CHB_SLPTRPIN));} while (0)
#define CHB_SLPTR_DISABLE() do {CHB_SLPTRPORT &= ~(_BV(CHB_SLPTRPIN));} while (0)

enum
{
    CCA_ED                    = 1,    /**< Use energy detection above threshold mode. */
    CCA_CARRIER_SENSE         = 2,    /**< Use carrier sense mode. */
    CCA_CARRIER_SENSE_WITH_ED = 3     /**< Use a combination of both energy detection and carrier sense. */
};

enum
{
    CHB_CHANNEL             = 11,
    CHB_PAN_ID              = 0x1234,
    CHB_TX_PWR              = 0x0,
    CHB_SHORT_ADDR          = 0x0,
    CHB_IEEE_ADDR           = 0x0,
    CHB_MAX_FRAME_RETRIES   = 3,
    CHB_MAX_CSMA_RETRIES    = 4,
    CHB_MIN_BE              = 0,
    CHB_CCA_MODE            = CCA_ED,
    CHB_CCA_ED_THRES        = 0x7,
    CHB_CSMA_SEED0          = 0,
    CHB_CSMA_SEED1          = 0
};

enum
{
    TRX_STATUS      = 0x01,
    TRX_STATE       = 0x02,
    TRX_CTRL0       = 0x03,
    TRX_CTRL1       = 0x04,
    PHY_TX_PWR      = 0x05,
    PHY_RSSI        = 0x06,
    PHY_ED_LEVEL    = 0x07,
    PHY_CC_CCA      = 0x08,
    CCA_THRES       = 0x09,
    IRQ_MASK        = 0x0e,
    IRQ_STATUS      = 0x0f,
    VREG_CTRL       = 0x10,
    BATMON          = 0x11,
    XOSC_CTRL       = 0x12,
    FTN_CTRL        = 0x18,
    PLL_CF          = 0x1a,
    PLL_DCU         = 0x1b,
    PART_NUM        = 0x1c,
    VERSION_NUM     = 0x1d,
    MAN_ID_0        = 0x1e,
    MAN_ID_1        = 0x1f,
    SHORT_ADDR_0    = 0x20,
    SHORT_ADDR_1    = 0x21,
    PAN_ID_0        = 0x22,
    PAN_ID_1        = 0x23,
    IEEE_ADDR_0     = 0x24,
    IEEE_ADDR_1     = 0x25,
    IEEE_ADDR_2     = 0x26,
    IEEE_ADDR_3     = 0x27,
    IEEE_ADDR_4     = 0x28,
    IEEE_ADDR_5     = 0x29,
    IEEE_ADDR_6     = 0x2a,
    IEEE_ADDR_7     = 0x2b,
    XAH_CTRL_0      = 0x2c,
    CSMA_SEED_0     = 0x2d,
    CSMA_SEED_1     = 0x2e
};

enum
{
    CHB_MAX_FRAME_RETRIES_POS   = 4,
    CHB_MAX_CSMA_RETIRES_POS    = 1,
    CHB_MIN_BE_POS              = 6,
    CHB_CSMA_SEED1_POS          = 0,
    CHB_CCA_MODE_POS            = 5,
    CHB_AUTO_CRC_POS            = 7,
    CHB_TRX_END_POS             = 3,
    CHB_TRAC_STATUS_POS         = 5,
    CHB_MIN_FRAME_LENGTH        = 3,
    CHB_MAX_FRAME_LENGTH        = 0x7f
};

enum{
    TIME_TO_ENTER_P_ON               = 510,     /**<  Transition time from VCC is applied to P_ON. */
    TIME_P_ON_TO_TRX_OFF             = 510,     /**<  Transition time from P_ON to TRX_OFF. */
    TIME_SLEEP_TO_TRX_OFF            = 880,     /**<  Transition time from SLEEP to TRX_OFF. */
    TIME_RESET                       = 6,       /**<  Time to hold the RST pin low during reset */
    TIME_ED_MEASUREMENT              = 140,     /**<  Time it takes to do a ED measurement. */
    TIME_CCA                         = 140,     /**<  Time it takes to do a CCA. */
    TIME_PLL_LOCK                    = 150,     /**<  Maximum time it should take for the PLL to lock. */
    TIME_FTN_TUNING                  = 25,      /**<  Maximum time it should take to do the filter tuning. */
    TIME_NOCLK_TO_WAKE               = 6,       /**<  Transition time from *_NOCLK to being awake. */
    TIME_CMD_FORCE_TRX_OFF           = 1,       /**<  Time it takes to execute the FORCE_TRX_OFF command. */
    TIME_TRX_OFF_TO_PLL_ACTIVE       = 180,     /**<  Transition time from TRX_OFF to: RX_ON, PLL_ON, TX_ARET_ON and RX_AACK_ON. */
    TIME_STATE_TRANSITION_PLL_ACTIVE = 1,       /**<  Transition time from PLL active state to another. */
};

enum{
    RADIO_SUCCESS = 0x40,                       /**< The requested service was performed successfully. */
    RADIO_UNSUPPORTED_DEVICE,                   /**< The connected device is not an Atmel AT86RF230. */
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

enum
{
    CMD_NOP                  = 0,
    CMD_TX_START             = 2,
    CMD_FORCE_TRX_OFF        = 3,
    CMD_RX_ON                = 6,
    CMD_TRX_OFF              = 8,
    CMD_PLL_ON               = 9,
    CMD_RX_AACK_ON           = 22,
    CMD_TX_ARET_ON           = 25
};

enum
{
    P_ON                = 0,
    BUSY_RX             = 1,
    BUSY_TX             = 2,
    RX_ON               = 6,
    TRX_OFF             = 8,
    PLL_ON              = 9,
    SLEEP               = 15,
    BUSY_RX_AACK        = 17,
    BUSY_TX_ARET        = 18,
    RX_AACK_ON          = 22,
    TX_ARET_ON          = 25,
    RX_ON_NOCLK         = 28,
    RX_AACK_ON_NOCLK    = 29,
    BUSY_RX_AACK_NOCLK  = 30
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
U8 chb_set_channel(U8 channel);
void chb_set_ieee_addr(U8 *addr);
void chb_get_ieee_addr(U8 *addr);
void chb_set_short_addr(U16 addr);
U16 chb_get_short_addr();

// data transmit
U8 chb_tx(U8 *hdr, U8 *data, U8 len);

#ifdef CHB_DEBUG
// sram access
void chb_sram_read(U8 addr, U8 len, U8 *data);
void chb_sram_write(U8 addr, U8 len, U8 *data);
#endif

#endif

