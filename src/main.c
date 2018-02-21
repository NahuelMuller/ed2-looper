/*****************************************************************************
*   Materia:      Electronica Digital II - 2017 - UNSAM ECyT                 *
*   Estudiantes:  Fonseca, Maximiliano - Muller, Nahuel                      *
*   Proyecto:     Loopera basada en el chip LPC4337 del kit EDU-CIAA         *
*****************************************************************************/

/*------ LIBRARIES ------*/
#include  "main.h"

/*------ MAIN ------*/
int main(void){

  leds_init();
  tecs_init();

  while (1){
  }
  return 0;
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
