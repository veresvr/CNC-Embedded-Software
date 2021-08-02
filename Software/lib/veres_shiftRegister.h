/*			discription:
		 		library configure and driving a shift register SNx4HC595.
				datasheet: https://www.ti.com/lit/ds/symlink/sn74hc595.pdf?ts=1627877923531&ref_url=https%253A%252F%252Fwww.google.com%252F
				default state of the chip:
					output enable (!OE pin13) - hight (output disable, all pins are in Hi-z)
					direct overriding clear (!SRCLR) - hight (output pins has its own states)
					typedef enum {DISABLE = 0, ENABLE = !DISABLE} Subdivisions;
				
				was create:			08.02.2021
				update:					08.02.2021
				status: 				not WORK

				need make:			

				other notes:       
				clock must be 8 MHz.				
				Tyrkin Vladimir, ru.veres@yandex.ru
*/
#include "stdint.h"
#include "stm32f10x.h"

FunctionalState state;

typedef struct 
	{ uint8_t FULL_STEP = 0;								
		uint8_t MICRO_1_2 = 0h001;
						MICRO_1_4 = 0h010;
						MICRO_1_8,
						MICRO_1_16,
						MICRO_1_32,
						MICRO_1_64,
						MICRO_1_128,
						NO_CHANGES = !FULL_STEP;
} Subdivisions;
 
void REGISTER_Init(void);
void REGISTER_setData(uint8_t dataXY, uint8_t dataZ);
void REGISTER_state(FunctionalState state);								

void REGISTER_Init(void)
{	
	// clock 
  RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;					// Timer 2 clock enable 
	
	TIM2->CR1 |= TIM_CR1_ARPE										// TIM_CR1_ARPE  Auto-reload preload enable
							|TIM_CR1_URS;										// Only counter overflow/underflow generates an update interrupt or DMA request if enabled
		
	// interrupt
	TIM2->DIER |= TIM_DIER_UIE;
	
	// timer
	TIM2->ARR = 0;
//	TIM2->PSC = TIMER2_PRESCALLER-1;						// frequensy after prescaller will be 122 Hz.
	TIM2->CNT = 0;															// COUNTER
	TIM2->EGR |= TIM_EGR_TG;										// The TIF flag is set in TIMx_SR register. 
																							// Related interrupt or DMA transfer can occur if enabled.
}

void REGISTER_setData(uint8_t dataXY, uint8_t dataZ)
{
	//REGISTER_stop();
	TIM2->CNT = dataXY | (dataZ << 3);
	//TIM2->ARR = (uint16_t)msec;		// 65535 max
	
	//REGISTER_state(); 
}
void REGISTER_state(FunctionalState state)
{
	if (state == DISABLE){}											// set register to Hi-z state
		else {};
	
}		


/*		Example of use this library.

// in main code must be handler for interrupt:

uint8_t	status = 0;									// global variable

void TIM2_IRQHandler(void)
{
	TIM2->SR &= ~TIM_SR_UIF;					// clear flag of event
	TIM2->SR &= ~TIM_SR_TIF;					
	
	// next code is example of work
	if (status == 0) {								// if LED off - turn on
			GPIOA->BSRR =GPIO_BSRR_BS3;
			status = 1;
	} else {
			GPIOA->BSRR =GPIO_BSRR_BR3;
			status = 0;
	}
}


*/
