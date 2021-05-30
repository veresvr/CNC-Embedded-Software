/*			discription:
		 		library configure timer2 for generate an interrupt on timer counter overflow.
				
				was create:			26.05.2015
				update:					04.07.2015
				status: 				WORK

				need make:			

				other notes:       
				clock must be 8 MHz.				
				Tyrkin Vladimir, ru.veres@yandex.ru
*/
#include "stdint.h"
#include "stm32f10x.h"

#define TIMER2_PRESCALLER		8000
 
 
void TIMER2_Init(void);
void TIMER2_wait_msec(uint16_t msec);
void TIMER2_start(void);
void TIMER2_stop(void);								

void TIMER2_Init(void)
{	
	// clock 
  RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;					// Timer 2 clock enable 
	
	TIM2->CR1 |= TIM_CR1_ARPE										// TIM_CR1_ARPE  Auto-reload preload enable
							|TIM_CR1_URS;										// Only counter overflow/underflow generates an update interrupt or DMA request if enabled
		
	// interrupt
	TIM2->DIER |= TIM_DIER_UIE;
	
	// timer
	TIM2->ARR = 0;
	TIM2->PSC = TIMER2_PRESCALLER-1;						// frequensy after prescaller will be 122 Hz.
	TIM2->CNT = 0;															// COUNTER
	TIM2->EGR |= TIM_EGR_TG;										// The TIF flag is set in TIMx_SR register. 
																							// Related interrupt or DMA transfer can occur if enabled.
}

void TIMER2_wait_msec(uint16_t msec)
{
	TIMER2_stop();
	TIM2->CNT = 0;
	TIM2->ARR = (uint16_t)msec;		// 65535 max
	
	TIMER2_start(); 
}
void TIMER2_start(void)
{
	TIM2->CNT = 0;															// COUNTER
	TIM2->CR1 |= TIM_CR1_CEN;										// TIM_CR1_CEN	Counter enable
}		

void TIMER2_stop(void)
{
		TIM2->CR1 &= ~TIM_CR1_CEN;										// TIM_CR1_CEN	Counter enable
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
