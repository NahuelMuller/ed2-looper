/*****************************************************************************
*   Materia:      Electronica Digital II - 2017 - UNSAM ECyT                 *
*   Estudiantes:  Fonseca, Maximiliano - Muller, Nahuel                      *
*   Proyecto:     Loopera basada en el chip LPC4337 del kit EDU-CIAA         *
*****************************************************************************/

/*------ LIBRARIES ------*/
#include  "main.h"
#include  "Prueba_printf.h"

/*------ DEFINITIONS ------*/
#define   SAMPLES   100

/*------ VARIABLES ------*/
static char   aux[256];             // Debugging (usada por printf)

LLI_T         DAC_LLI0, DAC_LLI1,   // LLIs DMA
              ADC_LLI0, ADC_LLI1;

unsigned int  DAC_BUF0[SAMPLES], DAC_BUF1[SAMPLES],   // Buffers DAC y ADC
              ADC_BUF0[SAMPLES], ADC_BUF1[SAMPLES];

/*------ MAIN ------*/
int main(void){

  UART_Init();
  sprintf_mio(aux, "Inicio del programa\r\n");
  DEBUGSTR(aux);

  // Configuracion inicial de perifericos varios
  leds_init();
  tecs_init();
  dac_init();
  adc0_init();
  dma_init();

  // Configuracion del canal del DMA usado por el DAC (DMA channel 0)
    // Carga de LLIs
    DAC_LLI0.SRCADDR  = (unsigned int) &(LPC_DAC->CR);
    DAC_LLI1.SRCADDR  = (unsigned int) &(LPC_DAC->CR);
    DAC_LLI0.DESTADDR = (unsigned int) DAC_BUF0;
    DAC_LLI1.DESTADDR = (unsigned int) DAC_BUF1;
    DAC_LLI0.NEXT_LLI = (((unsigned int) &(DAC_LLI1)) & (1 << 1));
    DAC_LLI1.NEXT_LLI = (((unsigned int) &(DAC_LLI0)) & (1 << 1));
    DAC_LLI0.CONTROL  = (SAMPLES << 0) |  // Transfer size
                        (0 << 12) |       // Source burst size = 1
                        (0 << 15) |       // Destination burst size = 1
                        (2 << 18) |       // Source transfer width = Word (32-bit)
                        (2 << 21) |       // Destination transfer width = Word (32-bit)
                        (1 << 24) |       // AHB Master 1 selected for source transfer
                        (1 << 25) |       // AHB Master 1 selected for destination transfer
                        (0 << 26) |       // The source address is not incremented after each transfer
                        (1 << 27) |       // The destination address is incremented after each transfer
                        (1 << 31) ;       // The terminal count interrupt is enabled
    DAC_LLI1.CONTROL  = DAC_LLI0.CONTROL;
    // DMA Channel registers
    LPC_GPDMA->CH[0].SRCADDR  = DAC_LLI0.SRCADDR;
    LPC_GPDMA->CH[0].DESTADDR = DAC_LLI0.DESTADDR;
    LPC_GPDMA->CH[0].LLI      = DAC_LLI0.NEXT_LLI;
    LPC_GPDMA->CH[0].CONTROL  = DAC_LLI0.CONTROL;
    LPC_GPDMA->CH[0].CONFIG   = (1 << 0) |    // Channel enabled
                                (0xF << 1) |  // Source peripheral DAC
                                (0x2 << 11) | // Peripheral to memory (DMA control)
                                (0 << 14) |   // Interrupt error mask
                                (1 << 15) |   // Terminal count interrupt mask
                                (0 << 18) ;   // Enable DMA requests

  // Configuracion del canal del DMA usado por el ADC (DMA channel 1)
    // Carga de LLIs
    // DMA Channel Configuration registers


  while (1){  // Manejo de flags
  }

  return 0;   // En teoria nunca llega hasta aca (--teoria--)

}

/*------ INTERRUPTS ------*/
void DMA_IRQHandler(){    // Interrupcion del DMA
}
void GPIO0_IRQHandler(){  // Interrupcion asignada a TEC_1
  LPC_GPIO_PIN_INT->IST |= (0x01 << 0); // Clear Interrupt
}
void GPIO1_IRQHandler(){  // Interrupcion asignada a TEC_2
  LPC_GPIO_PIN_INT->IST |= (0x01 << 1);	// Clear Interrupt
}
void GPIO2_IRQHandler(){  // Interrupcion asignada a TEC_3
  LPC_GPIO_PIN_INT->IST |= (0x01 << 2);	// Clear Interrupt
}
void GPIO3_IRQHandler(){  // Interrupcion asignada a TEC_4
  LPC_GPIO_PIN_INT->IST |= (0x01 << 3);	// Clear Interrupt
}















/*------ END ------*/
