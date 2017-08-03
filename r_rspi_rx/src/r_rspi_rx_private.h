/***********************************************************************************************************************
* DISCLAIMER
* This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products. No
* other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all
* applicable laws, including copyright laws.
* THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
* THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED. TO THE MAXIMUM
* EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES
* SHALL BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO THIS
* SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
* Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability of
* this software. By using this software, you agree to the additional terms and conditions found by accessing the
* following link:
* http://www.renesas.com/disclaimer
*
* Copyright (C) 2013, 2014 Renesas Electronics Corporation. All rights reserved.
***********************************************************************************************************************/
/***********************************************************************************************************************
* File Name    : r_rspi_rx_if.h
* Description  : Functions for using RSPI on RX devices.
************************************************************************************************************************
* History : DD.MM.YYYY Version Description
*         : 25.10.2013 1.00     First Release
*         : 05.05.2014 1.20     Version number change
***********************************************************************************************************************/

#ifndef RSPI_PRIVATE_H
#define RSPI_PRIVATE_H

/***********************************************************************************************************************
Includes   <System Includes> , "Project Includes"
***********************************************************************************************************************/
/* Fixed width integer support. */
/* Access to peripherals and board defines. */
#include "platform.h"

/***********************************************************************************************************************
Macro definitions
***********************************************************************************************************************/
/* Version Number of API. */
#define RSPI_RX_VERSION_MAJOR           (1)
#define RSPI_RX_VERSION_MINOR           (20)

/* Number of channels of RSPI this MCU has. */
#if defined(BSP_MCU_RX62N) || defined(BSP_MCU_RX621)
#define RSPI_MAX_CHANNELS   (2)
#elif defined(BSP_MCU_RX63N) || defined(BSP_MCU_RX630) || defined(BSP_MCU_RX631)
#define RSPI_MAX_CHANNELS   (3)
#elif defined(BSP_MCU_RX210) || defined(BSP_MCU_RX110) || defined(BSP_MCU_RX111)
#define RSPI_MAX_CHANNELS   (1)
#else
#error  "ERROR in r_rspi_rx package. Either no MCU chosen or MCU is not supported"
#endif

#define    RSPI_BYTE_DATA  0x01
#define    RSPI_WORD_DATA  0x02
#define    RSPI_LONG_DATA  0x04

/* Bitmask definitions of RSPI register settings options. These are for registers that contain bit fields
*  to control individual options. Do not change. */
/* RSPI Control Register (SPCR) */
#define RSPI_SPCR_SPMS   (0x01) /* 0: SPI (four-wire). Uses internal slave select. */
                                /* 1: Clock synchronous operation (three-wire). Uses external slave select. */
#define RSPI_SPCR_TXMD   (0x02) /* 0: Full-duplex synchronous.    1: Only transmit operations  */
#define RSPI_SPCR_MODFEN (0x04) /* 0: Mode fault error detection disabled. 1: Detection enabled. */
#define RSPI_SPCR_MSTR   (0x08) /* 0: Slave mode.                 1: Master mode. */
#define RSPI_SPCR_SPEIE  (0x10) /* 0: Error Interrupt disabled.   1: Error Interrupt enabled.*/
#define RSPI_SPCR_SPTIE  (0x20) /* 0: Transmit Interrupt disabled 1: Transmit Interrupt Enabled */
#define RSPI_SPCR_SPE    (0x40) /* 0: RSPI operation disabled.    1: RSPI operation enabled. */
#define RSPI_SPCR_SPRIE  (0x80) /* 0: Receive Interrupt disabled. 1: Receive Interrupt Enabled. */

/*RSPI Slave Select Polarity Register (SSLP) */
#define RSPI_SSLP_SSL0P  (0x01) /* 0 = active low. 1 = active high. */
#define RSPI_SSLP_SSL1P  (0x02)
#define RSPI_SSLP_SSL2P  (0x04)
#define RSPI_SSLP_SSL3P  (0x08)
#define RSPI_SSLP_MASK   (0x0F)

/* RSPI Pin Control Register (SPPCR) */
#define RSPI_SPPCR_SPLP  (0x01) /* 0: Normal mode. 1: Loopback mode (reversed transmit data = receive). */
#define RSPI_SPPCR_SPLP2 (0x02) /* 0: Normal mode. 1: Loopback mode (transmit data = receive data). */
#define RSPI_SPPCR_MOIFV (0x04) /* 0: MOSI pin idles low. 1: MOSI pin idles high. */
#define RSPI_SPPCR_MOIFE (0x08) /* 0: MOSI pin idles at final previous data. 1: MOSI pin idles at MOIFV. */
#define RSPI_SPPCR_MASK  (0x33) /* Protect reserved bits */

/* RSPI Sequence Control Register (SPSCR) */
#define RSPI_SPSCR (0x07)  /* Sequence SPCMD0-7 by 0-n */

/* RSPI Bit Rate Register (SPBR) */
#define RSPI_SPBR  (0xFF)

/* RSPI Status Register  (SPSR) */
#define RSPI_SPSR_OVRF  (0x01) /* 0: No overrun error.          1: An overrun error occurred. */
#define RSPI_SPSR_IDLNF (0x02) /* 0: RSPI is in the idle state. 1: RSPI is in the transfer state. */
#define RSPI_SPSR_MODF  (0x04) /* 0: No mode fault error        1: A mode fault error occurred */
#define RSPI_SPSR_PERF  (0x08) /* 0: No parity error.           1: A parity error occurred. */
#define RSPI_SPSR_MASK  (0xA0) /* Protect reserved bits. */

/* RSPI Data Control Register (SPDCR) masks*/
#define RSPI_SPDCR_SPFC   (0x03) /* 0: Transfer n+1 frames at a time */
#define RSPI_SPDCR_SPRDTD (0x10) /* 0: read SPDR values from RX buffer. 1: values read from transmit buffer*/
#define RSPI_SPDCR_SPLW   (0x20) /* 0: SPDR is accessed in words. 1: SPDR is accessed in longwords. */
/* RSPI Data Control Register (SPDCR) settings. */
#define RSPI_DATA_ACCESS_16   (0x00) /* b5 0: SPDR is accessed in short words. */
#define RSPI_DATA_ACCESS_32   (0x20) /* b5 1: SPDR is accessed in longwords. */

/* RSPI Clock Delay Register (SPCKD) */
#define RSPI_SPCKD_MASK (0x07) /* n+1 RSPCK */

/* RSPI Slave Select Negation Delay Register (SSLND) */
#define RSPI_SSLND_MASK (0x07) /* n+1 RSPCK */

/* RSPI Next-Access Delay Register (SPND) */
#define RSPI_SPND_MASK  (0x07) /* n+1 RSPCK + 2 PCLK */

/* RSPI Control Register 2 (SPCR2) */
#define RSPI_SPCR2_SPPE  (0x01) /* 0: No parity.                      1: Adds parity bit. */
#define RSPI_SPCR2_SPOE  (0x02) /* 0: Even parity.                    1: Odd parity. */
#define RSPI_SPCR2_SPIIE (0x04) /* 0: Disable Idle interrupt          1: Enable Idle interrupt */
#define RSPI_SPCR2_PTE   (0x08) /* 0: Disable parity self diagnostic. 1: Enable parity self diagnostic. */
#define RSPI_SPCR2_MASK  (0x0F) /* Protect reserved bits. */

/* Bit masks for RSPI Command Registers 0 to 7 (SPCMD0 to SPCMD7). */
#define RSPI_SPCMD_CPHA   (0x0001)  /* 0: Data sampling on odd edge, data variation on even edge. */
                                    /* 1: Data variation on odd edge, data sampling on even edge. */
#define RSPI_SPCMD_CPOL   (0x0002)  /* 0: RSPCK is low when idle. 1: RSPCK is high when idle. */
#define RSPI_SPCMD_BRDV   (0x000C)  /* Base bit-rate divisor. Div 2^N.  2-bits  */
#define RSPI_SPCMD_SSLA   (0x0070)  /* n = Slave select 0-3 when SPI mode. */
#define RSPI_SPCMD_SSLKP  (0x0080)  /* 0: SSL signal negated at end of transfer. 1: Keep SSL level at end until next. */
#define RSPI_SPCMD_SPB    (0x0F00)  /* b11 to b8 SPB[3:0] bitmask */
#define RSPI_SPCMD_LSBF   (0x1000)  /* 0: MSB first. 1: LSB first. */
#define RSPI_SPCMD_SPNDEN (0x2000)  /* 0: A next-access delay of 1 RSPCK + 2 PCLK. */
                                    /* 1: delay = the setting of the RSPI nextaccess delay register (SPND)*/
#define RSPI_SPCMD_SLNDEN (0x4000)  /* 0: An SSL negation delay of 1 RSPCK. */
                                    /* 1: Delay = setting of RSPI slave select negation delay register (SSLND)*/
#define RSPI_SPCMD_SCKDEN (0x8000)  /* 0: An RSPCK delay of 1 RSPCK. */
                                    /* 1: Delay is equal to the setting of the RSPI clock delay register (SPCKD). */

#define RSPI_SPB_16_MASK      (0x0C00)    /* Length settings with any of these bits set do not need 32-bit access. */

/* Clock phase and polarity settings. */
#define RSPI_DATA_ON_ODD_CLK  (0x0000)    /* 0: Data sampling on odd clock edge. */
#define RSPI_DATA_ON_EVEN_CLK (0x0001)    /* 1: Data sampling on even clock edge. */
#define RSPI_CLK_IDLE_LO      (0x0000)    /* 0: RSPCK is low when idle. */
#define RSPI_CLK_IDLE_HI      (0x0002)    /* 1: RSPCK is high when idle. */

#ifndef NULL
 #define NULL   _NULL
#endif /* NULL */

/***********************************************************************************************************************
Typedef definitions
***********************************************************************************************************************/
typedef struct rspi_config_block_s
{
    uint8_t channel;
    uint8_t current_slave;  /* Number of the currently assigned slave. */
    bool rspi_chnl_opened;  /* This variable determines whether the peripheral has already been initialized. */
    void   (*pcallback)(void *pcbdat); /* pointer to user callback function. */
} rspi_config_block_t;

#endif /* RSPI_PRIVATE_H */
