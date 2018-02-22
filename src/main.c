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

unsigned int  buffer[SAMPLES];    // Buffer intermedio

bool          REQ_DAC = 0,        // Flag indicador request DMA del DAC
              DAC_LLI0_ATR = 0,   // Flag DAC_LLI0 activo
              REQ_ADC = 0,        // Flag indicador request DMA del ADC
              ADC_LLI0_ATR = 0;   // Flag ADC_LLI0 activo

unsigned int  i;                  // Auxiliar

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

  // Configuracion del canal del DMA usado por el DAC (DMA channel 0)
    // Carga de LLIs
    DAC_LLI0.SRCADDR = (unsigned int) DAC_BUF0;
    DAC_LLI1.SRCADDR = (unsigned int) DAC_BUF1;
    DAC_LLI0.DESTADDR  = (unsigned int) &(LPC_DAC->CR);
    DAC_LLI1.DESTADDR  = (unsigned int) &(LPC_DAC->CR);
    DAC_LLI0.NEXT_LLI = (((unsigned int) &(DAC_LLI1)) & ~(1 << 1));
    DAC_LLI1.NEXT_LLI = (((unsigned int) &(DAC_LLI0)) & ~(1 << 1));
    DAC_LLI0.CONTROL  = (SAMPLES << 0) |  // Transfer size
                        (0 << 12) |       // Source burst size = 1
                        (0 << 15) |       // Destination burst size = 1
                        (2 << 18) |       // Source transfer width = Word (32-bit)
                        (2 << 21) |       // Destination transfer width = Word (32-bit)
                        (1 << 24) |       // AHB Master 1 selected for source transfer
                        (1 << 25) |       // AHB Master 1 selected for destination transfer
                        (1 << 26) |       // The source address is incremented after each transfer
                        (0 << 27) |       // The destination address is not incremented after each transfer
                        (1 << 31) ;       // The terminal count interrupt is enabled
    DAC_LLI1.CONTROL  = DAC_LLI0.CONTROL;
    // DMA Channel registers
    LPC_GPDMA->CH[0].SRCADDR  = DAC_LLI0.SRCADDR;
    LPC_GPDMA->CH[0].DESTADDR = DAC_LLI0.DESTADDR;
    LPC_GPDMA->CH[0].LLI      = DAC_LLI0.NEXT_LLI;
    LPC_GPDMA->CH[0].CONTROL  = DAC_LLI0.CONTROL;
    LPC_GPDMA->CH[0].CONFIG   = (1 << 0) |    // Channel enabled
                                (0xF << 6) |  // Destination peripheral DAC
                                (0x1 << 11) | // Memory to peripheral (DMA control)
                                (0 << 14) |   // Interrupt error mask
                                (1 << 15) |   // Terminal count interrupt mask
                                (0 << 18) ;   // Enable DMA requests

  // Configuracion del canal del DMA usado por el ADC (DMA channel 1)
    // Carga de LLIs
    // DMA Channel Configuration registers
    ADC_LLI0.SRCADDR  = (unsigned int) &(LPC_ADC0->DR[3]);
    ADC_LLI1.SRCADDR  = (unsigned int) &(LPC_ADC0->DR[3]);
    ADC_LLI0.DESTADDR = (unsigned int) ADC_BUF0;
    ADC_LLI1.DESTADDR = (unsigned int) ADC_BUF1;
    ADC_LLI0.NEXT_LLI = (((unsigned int) &(ADC_LLI1)) & ~(1 << 1));
    ADC_LLI1.NEXT_LLI = (((unsigned int) &(ADC_LLI0)) & ~(1 << 1));
    ADC_LLI0.CONTROL  = (SAMPLES << 0) |  // Transfer size
                        (0 << 12) |       // Source burst size = 1
                        (0 << 15) |       // Destination burst size = 1
                        (2 << 18) |       // Source transfer width = Word (32-bit)
                        (2 << 21) |       // Destination transfer width = Word (32-bit)
                        (1 << 24) |       // AHB Master 1 selected for source transfer
                        (1 << 25) |       // AHB Master 1 selected for destination transfer
                        (0 << 26) |       // The source address is not incremented after each transfer
                        (1 << 27) |       // The destination address is incremented after each transfer
                        (1 << 31) ;       // The terminal count interrupt is enabled
    ADC_LLI1.CONTROL  = ADC_LLI0.CONTROL;
    // DMA Channel registers
    LPC_GPDMA->CH[1].SRCADDR  = ADC_LLI0.SRCADDR;
    LPC_GPDMA->CH[1].DESTADDR = ADC_LLI0.DESTADDR;
    LPC_GPDMA->CH[1].LLI      = ADC_LLI0.NEXT_LLI;
    LPC_GPDMA->CH[1].CONTROL  = ADC_LLI0.CONTROL;
    LPC_GPDMA->CH[1].CONFIG   = (1 << 0) |    // Channel enabled
                                (0xD << 1) |  // Source peripheral ADC0
                                (0x2 << 11) | // Peripheral to memory (DMA control)
                                (0 << 14) |   // Interrupt error mask
                                (1 << 15) |   // Terminal count interrupt mask
                                (0 << 18) ;   // Enable DMA requests

  dma_init();

  while (1){  // Manejo de flags
    if (REQ_DAC){         // Hubo un request del DAC
      REQ_DAC = 0;
      if (DAC_LLI0_ATR){        // DAC_LLI0 en uso
        for (i = 0; i < SAMPLES; i++){
          DAC_BUF1[i] = buffer[i];
        }
      } else {                  // DAC_LLI1 en uso
        for (i = 0; i < SAMPLES; i++){
          DAC_BUF0[i] = buffer[i];
        }
      }
    } else if (REQ_ADC){  // Hubo un request del ADC
      REQ_ADC = 0;
      if (ADC_LLI0_ATR){        // ADC_LLI0 en uso
        for (i = 0; i < SAMPLES; i++){
          buffer[i] = ADC_BUF1[i];
        }
      } else {                  // ADC_LLI1 en uso
        for (i = 0; i < SAMPLES; i++){
          buffer[i] = ADC_BUF0[i];
        }
      }
    }
  }

  return 0;   // En teoria nunca llega hasta aca (--teoria--)

}

/*------ INTERRUPTS ------*/
void DMA_IRQHandler(){    // Interrupcion del DMA

  if (LPC_GPDMA->INTTCSTAT & (1 << 0)){   // Interrupt channel 0 (DAC)
    REQ_DAC = 1;
    LPC_GPDMA->INTTCCLEAR |= (1 << 0);    // Clear channel 0 interrupt
    if (LPC_GPDMA->CH[0].LLI == (unsigned int) &(DAC_LLI1)){
      DAC_LLI0_ATR = 1;   // DAC_LLI0 actualmente en uso por el DMA
    } else {
      DAC_LLI0_ATR = 0;   // DAC_LLI1 actualmente en uso por el DMA
    }
  } else {      // Interrupt channel 1 (ADC)
    REQ_ADC = 1;
    LPC_GPDMA->INTTCCLEAR |= (1 << 1);    // Clear channel 1 interrupt
    if (LPC_GPDMA->CH[1].LLI == (unsigned int) &(ADC_LLI1)){
      ADC_LLI0_ATR = 1;   // ADC_LLI0 actualmente en uso por el DMA
    } else {
      ADC_LLI0_ATR = 0;   // ADC_LLI1 actualmente en uso por el DMA
    }
  }

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
