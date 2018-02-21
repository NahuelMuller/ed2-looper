//#define	NO_LIBS

#include "board.h"
#include <stdbool.h>
#define SYSTEM_BAUD_RATE 115200
#define CIAA_BOARD_UART USART2

/************************************************************************************
 System Control Unit (SCU)
 ************************************************************************************/

#define SCU_BASE			0x40086000

// Offset de los registros de configuracion para los pines (pag. 405)
// Leds		
#define	SFSP2_0				0x100	// pin P2_0  -> LED0_R (Puerto 5, Pin 0)
#define	SFSP2_1				0x104	// pin P2_1  -> LED0_G (Puerto 5, Pin 1)
#define	SFSP2_2				0x108	// pin P2_2  -> LED0_B (Puerto 5, Pin 2)
#define	SFSP2_10			0x128	// pin P2_10 -> LED1   (Puerto 0, Pin 14)
#define	SFSP2_11			0x12C	// pin P2_11 -> LED2   (Puerto 1, Pin 11)
#define	SFSP2_12			0x130	// pin P2_12 -> LED3   (Puerto 1, Pin 12)

// Funcion y modo (pag. 413)
#define SCU_MODE_EPD		(0x0 << 3)		// habilita la resistencia de pull-down (deshabilita con 0)
#define SCU_MODE_EPUN		(0x0 << 4)		// habilita la resistencia de pull-up (deshabilita con 1)
#define SCU_MODE_DES		(0x2 << 3)		// deshabilita las resistencias de pull-down y pull-up
#define SCU_MODE_EHZ		(0x1 << 5)		// 1 Rapido (ruido medio con alta velocidad)
												// 0 Lento (ruido bajo con velocidad media)
#define SCU_MODE_EZI		(0x1 << 6)		// habilita buffer de entrada (deshabilita con 0)
#define SCU_MODE_ZIF_DIS	(0x1 << 7)		// deshabilita el filtro anti glitch de entrada (habilita con 1)

#define SCU_MODE_FUNC0		0x0				// seleccion de la funcion 0 del pin
#define SCU_MODE_FUNC1		0x1				// seleccion de la funcion 1 del pin
#define SCU_MODE_FUNC2		0x2				// seleccion de la funcion 2 del pin
#define SCU_MODE_FUNC3		0x3				// seleccion de la funcion 3 del pin
#define SCU_MODE_FUNC4		0x4				// seleccion de la funcion 4 del pin
#define SCU_MODE_FUNC5		0x5				// seleccion de la funcion 5 del pin
#define SCU_MODE_FUNC6		0x6				// seleccion de la funcion 6 del pin

#define SCU					((SCU_T         *) 	SCU_BASE)

/************************************************************************************
 Systtick
 ************************************************************************************/
#define _SysTick_BASE		0xE000E010	// Systick Base Address
#define _SysTick			((SysTick_T 	*) 	_SysTick_BASE )

// SysTick CTRL: Mascaras (ARM pag. 4-33/4-35)
#define _SysTick_CTRL_CLKSOURCE_PROC_Msk	(1 << 2)	// SysTick CTRL: CLKSOURCE Mask
#define _SysTick_CTRL_CLKSOURCE_EXT_Msk		(0 << 2)	// SysTick CTRL: CLKSOURCE Mask
#define _SysTick_CTRL_TICKINT_Msk	(1 << 1)	// SysTick CTRL: TICKINT Mask
#define _SysTick_CTRL_ENABLE_Msk	(1 << 0)	// SysTick CTRL: ENABLE Mask

/************************************************************************************
 General Purpose DMA (GPDMA)
 ************************************************************************************/
#define GPDMA_BASE  	0x40002000
#define GPDMA			((GPDMA_T*) GPDMA_BASE)

/************************************************************************************
 UART / USART
 ************************************************************************************/
#define USART0_BASE		0x40081000
#define USART2_BASE		0x400C1000
#define USART3_BASE		0x400C2000
#define UART1_BASE		0x40082000

#define USART0			((USART_T	*) USART0_BASE)
#define USART2			((USART_T	*) USART2_BASE)
#define USART3			((USART_T	*) USART3_BASE)
#define UART1			((USART_T	*) UART1_BASE)

#define _UART_IER_RBRINT	(1 << 0)	// RBR Interrupt enable


/************************************************************************************
 *	Clocks
 ************************************************************************************/
#define CCU1_BASE		0x40051000
#define CCU2_BASE		0x40052000
#define CCU1			((CCU1_T	*) CCU1_BASE)
#define CCU2			((CCU2_T	*) CCU2_BASE)


/************************************************************************************
   ESTRUCTURA PARA LOS DIFERENTES REGISTROS
 ************************************************************************************/

// System Control Unit Register
typedef struct {
	int  SFSP[16][32];		// Los pines digitales estan divididos en 16 grupos (P0-P9 y PA-PF)
	int  RESERVED0[256];
	int  SFSCLK[4];			// Pin configuration register for pins CLK0-3
	int  RESERVED16[28];
	int  SFSUSB;			// Pin configuration register for USB
	int  SFSI2C0;			// Pin configuration register for I2C0-bus pins
	int  ENAIO[3];			// Analog function select registers
	int  RESERVED17[27];
	int  EMCDELAYCLK;		// EMC clock delay register
	int  RESERVED18[63];
	int  PINTSEL[2];		// Pin interrupt select register for pin int 0 to 3 index 0, 4 to 7 index 1
} SCU_T;

// CCU clock config/status register pair
typedef struct {
	unsigned int	CFG;					// CCU clock configuration register
	unsigned		STAT;					// CCU clock status register
} _CCU_CFGSTAT_T;

// CCU1 register block structure
typedef struct {
	unsigned int	PM;						// CCU1 power mode register
	unsigned int	BASE_STAT;				// CCU1 base clocks status register
	unsigned int	RESERVED0[62];
	_CCU_CFGSTAT_T	CLKCCU[CLK_CCU1_LAST];	// Start of CCU1 clock registers
} CCU1_T;

// USART Register
typedef struct {

	union {
		unsigned int  DLL; // Divisor Latch LSB. Least significant byte of the baud rate divisor value. The full divisor is used to generate a baud rate from the fractional rate divider (DLAB = 1).
		unsigned int  THR; // Transmit Holding Register. The next character to be transmitted is written here (DLAB = 0).
		unsigned int  RBR; // Receiver Buffer Register. Contains the next received character to be read (DLAB = 0).
	};

	union {
		unsigned int IER;	// Interrupt Enable Register. Contains individual interrupt enable bits for the 7 potential UART interrupts (DLAB = 0).
		unsigned int DLM;	// Divisor Latch MSB. Most significant byte of the baud rate divisor value. The full divisor is used to generate a baud rate from the fractional rate divider (DLAB = 1).
	};

	union {
		unsigned int FCR;	// FIFO Control Register. Controls UART FIFO usage and modes.
		unsigned int IIR;	// Interrupt ID Register. Identifies which interrupt(s) are pending.
	};

	unsigned int LCR;		// Line Control Register. Contains controls for frame formatting and break generation.
	unsigned int MCR;		// Modem Control Register. Only present on USART ports with full modem support.
	unsigned int LSR;		// Line Status Register. Contains flags for transmit and receive status, including line errors.
	unsigned int MSR;		// Modem Status Register. Only present on USART ports with full modem support.
	unsigned int SCR;		// Scratch Pad Register. Eight-bit temporary storage for software.
	unsigned int ACR;		// Auto-baud Control Register. Contains controls for the auto-baud feature.
	unsigned int ICR;		// IrDA control register (not all UARTS)
	unsigned int FDR;		// Fractional Divider Register. Generates a clock input for the baud rate divider.
	unsigned int OSR;		// Oversampling Register. Controls the degree of oversampling during each bit time. Only on some UARTS.
	unsigned int TER1;		// Transmit Enable Register. Turns off USART transmitter for use with software flow control.
	unsigned int RESERVED0[3];
    unsigned int HDEN;		// Half-duplex enable Register- only on some UARTs
	unsigned int RESERVED1[1];
	unsigned int SCICTRL;	// Smart card interface control register- only on some UARTs

	unsigned int RS485CTRL;	// RS-485/EIA-485 Control. Contains controls to configure various aspects of RS-485/EIA-485 modes.
	unsigned int RS485ADRMATCH;	// RS-485/EIA-485 address match. Contains the address match value for RS-485/EIA-485 mode.
	unsigned int RS485DLY;		// RS-485/EIA-485 direction control delay.

	union {
		unsigned int SYNCCTRL;	// Synchronous mode control register. Only on USARTs.
		unsigned int FIFOLVL;	// FIFO Level register. Provides the current fill levels of the transmit and receive FIFOs.
	};

	unsigned int TER2;			// Transmit Enable Register. Only on LPC177X_8X UART4 and LPC18XX/43XX USART0/2/3.
} USART_T;


STATIC INLINE void UART_SendByte(USART_T *pUART, unsigned char data){
	pUART->THR = (unsigned int) data;
}

/*********************************************************************************
   Encabezados de funciones
 *********************************************************************************/
 
int sprintf_mio(char *, const char *, ...);
void UART_Init(void);