/***********************************************************************************************************************
 * File Name    : hwsetup.c
 * Device(s)    : RX
 * H/W Platform : MT01
 * Description  : Defines the initialization routines used each time the MCU is restarted.
 ***********************************************************************************************************************/
/***********************************************************************************************************************
 * History : DD.MM.YYYY Version  Description
 *         : 26.09.2015 1.00     First Release
 ***********************************************************************************************************************/

/***********************************************************************************************************************
Includes   <System Includes> , "Project Includes"
 ***********************************************************************************************************************/
/* I/O Register and board definitions */
#include "platform.h"

/***********************************************************************************************************************
Private global variables and functions
 ***********************************************************************************************************************/
/* MCU I/O port configuration function declaration */
static void output_ports_configure(void);

/* Interrupt configuration function declaration */
static void interrupts_configure(void);

/* MCU peripheral module configuration function declaration */
static void peripheral_modules_enable(void);


/***********************************************************************************************************************
 * Function name: hardware_setup
 * Description  : Contains setup functions called at device restart
 * Arguments    : none
 * Return value : none
 ***********************************************************************************************************************/
void hardware_setup(void)
{
	output_ports_configure();
	interrupts_configure();
	peripheral_modules_enable();
	bsp_non_existent_port_init();
}

/***********************************************************************************************************************
 * Function name: output_ports_configure
 * Description  : Configures the port and pin direction settings, and sets the pin outputs to a safe level.
 * Arguments    : none
 * Return value : none
 ***********************************************************************************************************************/
static void output_ports_configure(void)
{
	/* Unlock MPC registers to enable writing to them. */
	R_BSP_RegisterProtectDisable(BSP_REG_PROTECT_MPC);

	/* Port 0 - not used*/
	PORT0.PODR.BYTE = 0x00 ;    /* All outputs low to start */
	PORT0.PDR.BYTE  = 0xFF ;    /* all  are outputs */

	/* Port 1 -USB VBUS, USB Overcurrent Plasma - Limites (IRQ2) */
	PORT1.PMR.BYTE = 0x54;
	MPC.P12PFS.BYTE = 0x40; /* IRQ2 -  Limites*/
	MPC.P14PFS.BYTE = 0x12; /* USB0_OVRCURA */
	MPC.P16PFS.BYTE = 0x12; /* USB0_VBUS */
	PORT1.PODR.BYTE = 0x00 ; /* All unused pins  outputs low to start */
	PORT1.PDR.BYTE  = 0xFF ;  /* All  unused pins are outputs */

	/* Port 2 - Plasma - Emergencia (IRQ8), Arco OK (IRQ9), PWM Chp (P22), Tocha (P23)  */
	PORT2.PMR.BYTE = 0x03;
	MPC.P20PFS.BYTE = 0x40; /* IRQ8 - Emergencia */
	MPC.P21PFS.BYTE = 0x40; /* IRQ9 - Arco Ok */
	PORT2.PODR.BYTE = 0x08 ;    /* All outputs low to start except TORCH*/
	PORT2.PDR.BYTE  = 0xFE ;    /* All outputs - Emergencia (Input)  */

	/* Port 3 - JTAG, DA0(MTIOC0C), DA1(MTIOC0D) */
	PORT3.PMR.BYTE = 0x0C;
	MPC.P32PFS.BYTE = 0x01; /* MTIOC0C - DA0 */
	MPC.P33PFS.BYTE = 0x01; /* MTIOC0D - DA1 */
	PORT3.PODR.BYTE = 0x00 ;    /* All outputs low to start */
	PORT3.PDR.BYTE = 0x0C;

	/* Port 4 - TLINHA0-2 (P44 - 46),AN0 (AN001), AN1(AN002), THC Voltage (AN003) */
	PORT4.PMR.BYTE = 0x0E;
	MPC.P41PFS.BYTE = 0x80;  /* AN001 - AN0 */
	MPC.P42PFS.BYTE = 0x80;  /* AN002 - AN1 */
	MPC.P43PFS.BYTE = 0x80;  /* AN003 - THC Voltage */
	PORT4.PODR.BYTE = 0x00 ;    /* All outputs low to start */
	PORT4.PDR.BYTE  = 0x8F ;    /* TLINHA0-2 (P44 - 46) - inputs, All outputs  */
	PORT4.PCR.BYTE  = 0x70 ;	 /* Pull up */

	/* Port 5 - OUT0-3 (P50 - 53) */
	PORT5.PMR.BYTE  = 0x05 ;    /*  */
	MPC.P50PFS.BYTE = 0x0A ;    /* P50 is TXD2 */
	MPC.P52PFS.BYTE = 0x0A ;    /* P52 is RXD2 */
	PORT5.PODR.BYTE = 0x00 ;    /* All outputs low to start */
	PORT5.PDR.BYTE  = 0xFF ;    /* All outputs */

	/* Port A -   */
	PORTA.PODR.BYTE = 0x00 ;    /* All outputs low to start */
	PORTA.PDR.BYTE  = 0xFF ;    /* All outputs */

	//    /* Port B - IN0-4 (PB1 - B5) */
	PORTB.PODR.BYTE = 0x00 ;
	PORTB.PDR.BYTE = 0x01; /* IN0-4 - inputs, All outputs  */

	/* Port C -  LCD SPI signals, LCD CS (PC2)*/
	PORTC.PMR.BYTE  = 0x60 ;    /*  */
	MPC.PC5PFS.BYTE = 0x0D ;    /* PC5 is RSPCKA */
	MPC.PC6PFS.BYTE = 0x0D ;    /* PC6 is MOSIA */
	PORTC.PODR.BYTE = 0x00 ;    /* All outputs low to start */
	PORTC.PDR.BYTE  = 0xFF ;    /* All outputs*/

	/* Port D -  TCOL0-2 (P44 - 46) */
	PORTD.PODR.BYTE = 0x00 ;    /* All outputs low to start  	*/
	PORTD.PDR.BYTE  = 0xFF ;    /* All outputs*/

	/* Port E -  CNC signals*/
    PORTE.PMR.BYTE  = 0x0E ;    /* All GPIO for now */
    MPC.PE1PFS.BYTE = 0x0E ;    /* PE1 is RSPCKB */
    MPC.PE2PFS.BYTE = 0x0E ;    /* PE2 is MOSIB */
    MPC.PE3PFS.BYTE = 0x0D ;    /* PE3 is MISOB */
	PORTE.PODR.BYTE = 0x00 ;    /* All outputs low to start  */
	PORTE.PDR.BYTE  = 0xFF ;    /* All outputs*/

	/* Port J -  No used	*/
	PORTJ.PODR.BYTE = 0x00 ;    /* All outputs low to start  */
	PORTJ.PDR.BYTE  = 0xFF ;    /* All output */
	/* Lock MPC registers. */
	R_BSP_RegisterProtectEnable(BSP_REG_PROTECT_MPC);
}

/***********************************************************************************************************************
 * Function name: interrupts_configure
 * Description  : Configures interrupts used
 * Arguments    : none
 * Return value : none
 ***********************************************************************************************************************/
static void interrupts_configure(void)
{
	/* Add code here to setup additional interrupts */
}

/***********************************************************************************************************************
 * Function name: peripheral_modules_enable
 * Description  : Enables and configures peripheral devices on the MCU
 * Arguments    : none
 * Return value : none
 ***********************************************************************************************************************/
static void peripheral_modules_enable(void)
{
	/* Add code here to enable peripherals used by the application */
}
