/*****************************************************************************
*   Funciones de inicializacion de perifericos                               *
*****************************************************************************/

#include  "main.h"

void leds_init(void){		// Inicializacion de leds (apagados)

	// SCU: Set Pin Function and Mode
	LPC_SCU->SFSP[2][0] |= SCU_MODE_FUNC4;	// P2_0 (LED0_R) dirigido a GPIO5[0]
	LPC_SCU->SFSP[2][1] |= SCU_MODE_FUNC4;	// P2_1 (LED0_G) dirigido a GPIO5[1]
	LPC_SCU->SFSP[2][2] |= SCU_MODE_FUNC4;	// P2_2 (LED0_B) dirigido a GPIO5[2]
	LPC_SCU->SFSP[2][10] |= SCU_MODE_FUNC0;	// P2_10 (LED1) dirigido a GPIO0[14]
	LPC_SCU->SFSP[2][11] |= SCU_MODE_FUNC0;	// P2_11 (LED2) dirigido a GPIO1[11]
	LPC_SCU->SFSP[2][12] |= SCU_MODE_FUNC0;	// P2_12 (LED3) dirigido a GPIO1[12]

	// GPIO port: Set Direction (0 input; 1 output)
	LPC_GPIO_PORT->DIR[5] |= 0x07;					// GPIO5[2], GPIO5[1] y GPIO5[0] como salidas
	LPC_GPIO_PORT->DIR[0] |= (0x01 << 14);	// GPIO0[14] como salidas
	LPC_GPIO_PORT->DIR[1] |= (0x03 << 11);	// GPIO1[12] y GPIO1[11] como salidas

	// GPIO port: Clear Bits
	LPC_GPIO_PORT->CLR[5] |= 0x07;					// GPIO5[2], GPIO5[1] y GPIO5[0]
	LPC_GPIO_PORT->CLR[0] |= (0x01 << 14);	// GPIO0[14]
	LPC_GPIO_PORT->CLR[1] |= (0x03 << 11);	// GPIO1[12] y GPIO1[11]

}

void tecs_init(void){		// Inicializacion de TEC_1, TEC_2, TEC_3 y TEC_4

	// SCU: Set Pin Function and Mode
	LPC_SCU->SFSP[1][0] |= (SCU_MODE_FUNC0 | SCU_MODE_INACT | SCU_MODE_INBUFF_EN);	// P1_0 (TEC_1) dirigido a GPIO0[4], sin pull-up ni pull-down y con buffer de entrada
	LPC_SCU->SFSP[1][1] |= (SCU_MODE_FUNC0 | SCU_MODE_INACT | SCU_MODE_INBUFF_EN);	// P1_1 (TEC_2) dirigido a GPIO0[8], sin pull-up ni pull-down y con buffer de entrada
	LPC_SCU->SFSP[1][2] |= (SCU_MODE_FUNC0 | SCU_MODE_INACT | SCU_MODE_INBUFF_EN);	// P1_2 (TEC_3) dirigido a GPIO0[9], sin pull-up ni pull-down y con buffer de entrada
	LPC_SCU->SFSP[1][6] |= (SCU_MODE_FUNC0 | SCU_MODE_INACT | SCU_MODE_INBUFF_EN);	// P1_6 (TEC_4) dirigido a GPIO1[9], sin pull-up ni pull-down y con buffer de entrada

	// GPIO port: Set Direction (0 input; 1 output)
	LPC_GPIO_PORT->DIR[0] &= (~((0x01 << 9) | (0x01 << 8) | (0x01 << 4)));	// GPIO0[9], GPIO0[8] y GPIO0[4] como entradas
	LPC_GPIO_PORT->DIR[1] &= (~(0x01 << 9));																// GPIO1[9] como entrada

	// SCU: Pin interrupt select register 0 (p. 422 UM10503_rev1.9_2015_LPC43xx_User_Manual)
	LPC_SCU->PINTSEL0 |= (((0x00 << 5) | 0x04) << 0);		// GPIO pin interrupt 0 -> GPIO0[4]
	LPC_SCU->PINTSEL0 |= (((0x00 << 5) | 0x08) << 8);		// GPIO pin interrupt 1 -> GPIO0[8]
	LPC_SCU->PINTSEL0 |= (((0x00 << 5) | 0x09) << 16);	// GPIO pin interrupt 2 -> GPIO0[9]
	LPC_SCU->PINTSEL0 |= (((0x01 << 5) | 0x09) << 24);	// GPIO pin interrupt 3 -> GPIO1[9]

	// NVIC: Interrupt Set Enable Registers (p. 253-254 The_Definitive_Guide_to_ARM_Cortex-M3_and_Cortex-M4_Joseph_Yiu)
	NVIC->ISER[1] |= 0x0F;	// Enable for external interrupt #32–33-34-35 (GPIO pin interrupt 0,1,2,3)

	// Pin interrupt status register
	LPC_GPIO_PIN_INT->IST |= (0x0F << 0);	// Clear Interrupts

	// GPIO pin interrupt mode
	LPC_GPIO_PIN_INT->IENR |= 0x0F;	// Rising edge interrupt enable (GPIO pin interrupt 0,1,2,3)

}

void adc0_init(void){				// Inicializacion del ADC0

	// SCU: Analog function select register (ENAIO0)
	LPC_SCU->ENAIO[0] |= (1 << 3);	// Digital function selected on pin P7_5

	// SCU: Set Pin Function and Mode
	LPC_SCU->SFSP[7][5] |= SCU_MODE_INACT;	// Disable pull-down and pull-up on pin P7_5

	// A/D Control register
	LPC_ADC0->CR |= (1 << 3) |		// Select ADC0 channel
									(231 << 8) |	// ADC0 clkdiv => Freq = 204MHz / (11 * (clkdiv + 1))
									(1 << 16) |		// Burst mode => Repeated conversions
									(0 << 17) |		// 10 bits resolution
									(1 << 21) |		// Power on
									(0 << 24) |		// Conversion start => Burst controlled (not software)
									(0 << 27) ;		// Start signal edge => Burst => Not significant bit

	// A/D Interrupt Enable register
	LPC_ADC0->INTEN = (1 << 3) |	// Channel 3 interrupt enabled
										(0 << 8) ;	// Global interrupt disabled			(HE AQUI LA CAUSA DE TODO EL TIEMPO PERDIDO)

	// NVIC: Interrupt Set Enable Registers and Interrupt Clear Enable Registers
	NVIC->ICER[0] |= (1 << 17);	// Disable external interrupt #17 (ADC0)

}

void dac_init(void){		// Inicializacion del DAC

	// SCU: Analog function select register (ENAIO2)
	LPC_SCU->ENAIO[2] |= (1 << 0);	// Analog function DAC selected on pin P4_4

	// SCU: Set Pin Function and Mode
	LPC_SCU->SFSP[4][4] |= SCU_MODE_INACT;	// Disable pull-down and pull-up on pin P4_4

	// D/A Converter register
	LPC_DAC->CR |= (1 << 16);	// Maximum update rate of 400 kHz

	// D/A Control register
	LPC_DAC->CTRL |=	(1 << 1) |	// Enable double-buffering
										(1 << 2) |	// Time-out counter operation is enabled
										(1 << 3);		// Enable DAC and enable DMA Burst Request Input 15

	// D/A Converter counter value register
	LPC_DAC->CNTVAL |= (2552 & 0xFFFF);	// 16-bit reload value for the DAC interrupt/DMA timer => (11 * (clkdiv + 1))

}

void dma_init(void){	// Inicializacion del DMA

	// DMA Configuration Register
	LPC_GPDMA->CONFIG |= (1 << 0);  // DMA Controller enabled

	// NVIC: Interrupt Set Enable Registers and Interrupt Clear Enable Registers
	NVIC->ISER[0] |= (1 << 2);	// Enable external interrupt #2 (DMA)

}




/*------ END ------*/
