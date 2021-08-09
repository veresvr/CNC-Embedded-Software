/*			discription:
		 		library configure and driving a shift register SNx4HC595 and stepper motor drivers LV8729.
				
				datasheet: https://www.ti.com/lit/ds/symlink/sn74hc595.pdf?ts=1627877923531&ref_url=https%253A%252F%252Fwww.google.com%252F
				default state of the chip:
					output enable (!OE pin13) - hight (output disable, all pins are in Hi-z)
					direct overriding clear (!SRCLR) - hight (output pins has its own states)
					typedef enum {DISABLE = 0, ENABLE = !DISABLE} Subdivisions;
					
					
				Max frequency for STEP pin = 50kHz.
				
				This library uses Bresenham line algorithm: Full information: https://en.wikipedia.org/wiki/Bresenham%27s_line_algorithm
				
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

#define NO_DATA						0

#define STEPS_PER_REV_XY	200
#define STEPS_PER_REV_Z		200
#define PULLEY_TEETH_XY		12
#define PULLEY_TEETH_Z		12
#define BELT_PITCH_XY_MM	2
#define BELT_PITCH_Z_MM		2

#define SUBDIVISION_1			1	
#define SUBDIVISION_2			2
#define SUBDIVISION_4			4
#define SUBDIVISION_8			8
#define SUBDIVISION_16		16
#define SUBDIVISION_32		32
#define SUBDIVISION_64		64
#define SUBDIVISION_128		128

#define LENGTH_PER_ONE_STEP_XY(SUBDIVISION)	((PULLEY_TEETH_XY*BELT_PITCH_XY_MM)/STEPS_PER_REV_XY)/(SUBDIVISION)

const double 	DistanceXYPerDiv1 = 0.12,
							DistanceXYPerDiv2 = 0.06,
							DistanceXYPerDiv4 = 0.03,
							DistanceXYPerDiv8 = 0.015,
							DistanceXYPerDiv16 = 0.0075,
							DistanceXYPerDiv32 = 0.00375,
							DistanceXYPerDiv64 = 0.001875,
							DistanceXYPerDiv128 = 0.0009375;

const double ActualDistance = DistanceXYPerDiv1;

float oldDataX = 0,
			oldDataY = 0,
			oldDataZ = 0;

FunctionalState state;
/*
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
} Subdivisions; */
 
 
void moveXY(float newDataX, float newDataY);
void moveZ(float newDataZ);

void DriverBoard_Init(void);
void REGISTER_setData(uint8_t dataXY, uint8_t dataZ);
void REGISTER_state(FunctionalState state);		


void moveXY(float newDataX, float newDataY)
{	
	float deltaX = newDataX - oldDataX;
	float deltaY = newDataY - oldDataY;
	
	uint32_t 	stepsX, 
						stepsY, 
						deltaSteps,
						sideSteps,
						middleSteps;
		
//LENGHT CONVERTING TO STEPS
//	stepsX = uint32_t (deltaX / ActualDistance);
//	stepsY = uint32_t (deltaY / ActualDistance);
	
	if ( (newDataX == NO_DATA) && (newDataY != NO_DATA) ) {
		// move only along Y
	// return success
	}

	if ( (newDataY == NO_DATA) && (newDataX != NO_DATA) ) {
		// move only along X
	// return success
	}
	
	if ( (newDataY == NO_DATA) && (newDataX == NO_DATA) ) {
		// nothing to move
	// return success
	}
	
	if ( newDataY == newDataX ) {
		// move simetrically along X and Y
	// return success
	}

// here the algorithm	
	if(stepsX > stepsY){
		middleSteps = stepsX / (stepsY - 1);
	} else {
		middleSteps = stepsY / (stepsX - 1);
	}
	sideSteps = middleSteps >> 2;							// i need divide it by 2

	

	oldDataX = newDataX;
	oldDataY = newDataY;
}

void moveZ(float newDataZ)
{

	
	 
}

void DriverBoard_Init(void)
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




*/
