/*			discription:
		 		library configure and driving a shift register SNx4HC595 and stepper motor drivers LV8729.
				
				datasheet: https://www.ti.com/lit/ds/symlink/sn74hc595.pdf?ts=1627877923531&ref_url=https%253A%252F%252Fwww.google.com%252F
				default state of the chip:
					output enable (!OE pin13) - hight (output disable, all pins are in Hi-z)
					direct overriding clear (!SRCLR) - hight (output pins has its own states)
					typedef enum {DISABLE = 0, ENABLE = !DISABLE} Subdivisions;
					
					
				Max frequency for STEP pin = 50kHz. -> 0.00002s -> 0.00001s for high level and 0.00001s for low.
				If uC runs with 72MHz and timer has no divider then 0.00001s = 720 tics.
				Lets set the timer prescaler for 72: PSC[15:0] = 71. so then 0.00001s = 10 tics.
				If PWM frequency will be 50kHz -> 0.004s for full turn -> 24mm lenght -> 6 m/s max speed -> 360 m/min max.
				
				This library uses Bresenham line algorithm: the code was taken from here http://rosettacode.org/wiki/Bitmap/Bresenham%27s_line_algorithm#C
				and modified for my needs.
				

				other notes:       
				clock must be 8 MHz.				
				Tyrkin Vladimir, ru.veres@yandex.ru
*/
#include "stdint.h"
#include "stm32f10x.h"
#include "D:\development\my_libraries\veres_defines_list.h"

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

#define AXIS_X						1	
#define AXIS_Y						2
#define AXIS_Z						3

#define MINIMUM_TICS			10

#define LENGTH_PER_ONE_STEP_XY(SUBDIVISION)	((PULLEY_TEETH_XY*BELT_PITCH_XY_MM)/STEPS_PER_REV_XY)/(SUBDIVISION)

const double 	DistanceXYPerDiv1 = 0.12,
							DistanceXYPerDiv2 = 0.06,
							DistanceXYPerDiv4 = 0.03,
							DistanceXYPerDiv8 = 0.015,
							DistanceXYPerDiv16 = 0.0075,
							DistanceXYPerDiv32 = 0.00375,
							DistanceXYPerDiv64 = 0.001875,
							DistanceXYPerDiv128 = 0.0009375;

const double ActualResolution = DistanceXYPerDiv1;

// location where we are
float oldDataX = 0,
			oldDataY = 0,
			oldDataZ = 0;

uint8_t speedValueMoveXY = 20,
				speedValueWorkXY = 200,
				speedValueMoveZ = 20,
				speedValueWorkZ = 200;

uint32_t	counterStepsX = 0, 
					counterStepsY = 0, 
					counterStepsZ = 0;

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
 
 
uint32_t moveLineXY(float newDataX, float newDataY);
void moveLineZ(float newDataZ);
uint8_t stepsX(int32_t value);
uint8_t stepsY(int32_t value);
uint8_t setSpeedXY(uint16_t value);
uint8_t setSpeedZ(uint16_t value);
void stepsZ(int32_t value);
void DriverBoard_Init(void);
void REGISTER_setData(uint8_t dataXY, uint8_t dataZ);
void REGISTER_state(FunctionalState state);		
int abs(int number);																			// from stdlib.c
uint8_t setDirection(uint8_t axis, int32_t value);

uint32_t moveLineXY(float newDataX, float newDataY){	
	if ( (newDataY == NO_DATA) && (newDataX == NO_DATA) ) {
		// nothing to move
	return OK;
	}
	
	
	float lenghtOfX = newDataX - oldDataX;				// oldData = 0
	float lenghtOfY = newDataY - oldDataY;
	
	int32_t numberOfStepsX = lenghtOfX / ActualResolution,
					numberOfStepsY = lenghtOfY / ActualResolution;	
	
	
	uint32_t 	deltaSteps,
						sideSteps,
						middleSteps;
		
//LENGHT CONVERTING TO STEPS
//	stepsX = uint32_t (deltaX / ActualDistance);
//	stepsY = uint32_t (deltaY / ActualDistance);
	
	if ( (newDataX == NO_DATA) && (newDataY != NO_DATA) ) {
		// move only along Y
		stepsY(numberOfStepsY);
	return OK;
	}

	if ( (newDataY == NO_DATA) && (newDataX != NO_DATA) ) {
		// move only along X
	return OK;
	}
	

	
	if ( newDataY == newDataX ) {
		// move simetrically along X and Y
	return OK;
	}

// here the algorithm	
	if ( newDataY != newDataX ){
		
	int32_t numberOfStepsX = newDataX / ActualResolution,
					numberOfStepsY = newDataY / ActualResolution,
					oldNumberOfStepsX = 0,
					oldNumberOfStepsY = 0;
		
  int32_t	dx = abs(numberOfStepsX),
					sx = 0 < newDataX ? 1 : -1;

  int32_t	dy = abs(numberOfStepsY),
					sy = 0 < newDataY ? 1 : -1;

  int32_t	err = (dx>dy ? dx : -dy) >> 1,
					e2 = 0;

	int32_t x0 = 0,
					y0 = 0;
	
  for(;;){
    if ( (x0 == numberOfStepsX) && (y0 == numberOfStepsY) ) break;
		stepsX(numberOfStepsX);
		stepsY(numberOfStepsY);
    e2 = err;
    if (e2 >-dx) { err -= dy; x0 += sx; }
    if (e2 < dy) { err += dx; y0 += sy; }
		}
	
	oldDataX = newDataX;
	oldDataY = newDataY;	
	return OK;	
	}
}

void moveLineZ(float newDataZ){

	
	 
}

void DriverBoard_Init(void){	
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

void REGISTER_setData(uint8_t dataXY, uint8_t dataZ){
	//REGISTER_stop();
	TIM2->CNT = dataXY | (dataZ << 3);
	//TIM2->ARR = (uint16_t)msec;		// 65535 max
	
	//REGISTER_state(); 
}
void REGISTER_state(FunctionalState state){
	if (state == DISABLE){}											// set register to Hi-z state
		else {};
	
}		

int abs(int number) {
  return number >= 0 ? number : -number;
}

uint8_t stepsX(int32_t value){
// set direction
	setDirection(AXIS_X, value);
	
	TIM2->RCR = value - 1;										// coz update event (UEV) is generated after upcounting is (TIMx_RCR+1)
// start counter
	counterStepsX = (uint32_t)value;
	
}
uint8_t stepsY(int32_t value){
	

	if (value > 0) GPIOA->BSRR =GPIO_BSRR_BS3; 
		else GPIOA->BSRR =GPIO_BSRR_BS3; 
	// here need to start the timer
	
	
	
}
uint8_t setSpeedXY(uint16_t value){
	
	uint16_t tics = value >> 1;
	if (tics < MINIMUM_TICS) return 1; //ERR
	TIM1->ARR = value;
	TIM1->CCR1 = tics;
	TIM1->CNT = 0;											// reset the counter
	return 0;
}
uint8_t setSpeedZ(uint16_t value){
	
}

uint8_t setDirection(uint8_t axis, int32_t value){
	if (axis == AXIS_X){
		if (value > 0) GPIOA->BSRR =GPIO_BSRR_BS3;
			else{
				GPIOA->BSRR =GPIO_BSRR_BR3;
				return OK;
			}
	}
	
	if (axis == AXIS_Y){
		if (value > 0) GPIOA->BSRR =GPIO_BSRR_BS3;
			else{
				GPIOA->BSRR =GPIO_BSRR_BR3;
				return OK;
			}
	}	
	
	if (axis == AXIS_Z){
		if (value > 0) GPIOA->BSRR =GPIO_BSRR_BS3;
			else{
				GPIOA->BSRR =GPIO_BSRR_BR3;
				return OK;
			}
	}	
	
	return SET_DIR_ERROR;		// none of these 3 axes	
}

/*		Example of use this library.



void line(int x0, int x1, int y0, int y1) {

  int   dx = abs(x1-x0),
        sx = x0<x1 ? 1 : -1;

  int   dy = abs(y1-y0),
        sy = y0<y1 ? 1 : -1;

  int   err = (dx>dy ? dx : -dy)/2,
        e2 = 0;

  for(;;){
    if (x0==x1 && y0==y1) break;
    e2 = err;
    if (e2 >-dx) { err -= dy; x0 += sx; }
    if (e2 < dy) { err += dx; y0 += sy; }
  }
}




*/
