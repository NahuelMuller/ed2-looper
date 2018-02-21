/*****************************************************************************
*   Libreria principal del proyecto                                          *
*****************************************************************************/

#include  "board.h"   //Libreria lpcopen_lpc4337

void leds_init(void);
void tecs_init(void);
void dac_init(void);
void adc0_init(void);
void dma_init(void);

typedef struct {  // Estructura para Linked List Item
  unsigned int  SRCADDR;   // Source Address Register
	unsigned int  DESTADDR;  // Destination Address Register
	unsigned int  NEXT_LLI;  // Next Linked List Item Register
	unsigned int  CONTROL;   // Control Register
} LLI_T;
