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


float oldDataX = 0,
			oldDataY = 0,
			oldDataZ = 0;
 
 
void moveXY(float newDataX, float newDataY);
void moveZ(float newDataZ);

						

void moveXY(float newDataX, float newDataY)
{	
	float deltaX = newDataX - oldDataX;
	float deltaY = newDataY - oldDataY;
//LENGHT CONVERTING TO STEPS	
	uint32_t 	stepsX, 
						stepsY, 
						deltaSteps,
						sideSteps,
						middleSteps;
	
if (stepsX != stepsY)
	{	if(stepsX > stepsY){
		middleSteps = stepsX / (stepsY - 1);
	} else {
		middleSteps = stepsY / (stepsX - 1);
	}
	sideSteps = middleSteps >> 2;							// i need divide it by 2
}
	

	oldDataX = newDataX;
	oldDataY = newDataY;
}

void moveZ(float newDataZ)
{

	
	 
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
