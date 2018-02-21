/*****************************************************************************
*   Materia:      Electronica Digital II - 2017 - UNSAM ECyT                 *
*   Estudiantes:  Fonseca, Maximiliano - Muller, Nahuel                      *
*   Proyecto:     Loopera basada en el chip LPC4337 del kit EDU-CIAA         *
*****************************************************************************/

/*------ LIBRARIES ------*/
#include  "main.h"
#include  "Prueba_printf.h"

/*------ VARIABLES ------*/
static char aux[256];         // Debugging (usada por printf)
LLI_T   DAC_LLI0, DAC_LLI1,   // LLIs DMA
        ADC_LLI0, ADC_LLI1;

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

  // Configuracion del canal del DMA usado por el DAC


  // Configuracion del canal del DMA usado por el ADC


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
