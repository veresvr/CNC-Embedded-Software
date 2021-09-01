/*			discription:
				Program is an example of receive data packets.
				First of all u send first 8bit data, MCU is receive it and save to array. Next step - turn on timer2 for
				a DELAY_OF_DATA milliseconds. 
				Secondary, if new 8bit data coming faster than Timer2_interrupt is being - timer turn off, data will
				save in another array unit and timer start from 0.
				In another way, if 8bit data coming later than timer made interrupt - it will be first 8bit data in data array.
				
				
				if baudrate is 9600, then time to send 8bit data is 1 msec. so DELAY_OF_DATA=10 msec is good.
				programm was tested and it works perfect!
				
				Tyrkin Vladimir, veres.pcb@gmail.com
*/


#include "stm32f10x.h"
#include "lib\veres_debug_via_UART.h"
#include "lib\veres_timers.h"
#include "D:\development\my_libraries\veres_crc8.h"
#include "D:\development\my_libraries\veres_defines_list.h"
#include "D:\development\my_libraries\veres_shpindle_laser.h"
#include "lib\veres_DriverBoard.h"


#define	DELAY_OF_DATA			10		// time for delaying before next data packet is coming
#define DATA_INC_READY		1
#define DATA_INC_NOREADY	0
#define PACKET_DATA_SIZE_MAX	30
#define PACKET_DATA_SIZE_MIN	5


// variables
uint8_t lenghtOfDataPacket = 0,
				receive_array[PACKET_DATA_SIZE_MAX],
				statusIncomingData = DATA_INC_NOREADY,
				lenghtData = 0,
				err_code = NO_ERROR,
				crcResult = 0;



// interrupt
void TIM2_IRQHandler(void);
void USART1_IRQHandler(void);
void TIM3_IRQHandler(void);



void TIM2_IRQHandler(void)
{
	TIM2->SR &= ~TIM_SR_UIF;					// clear flag of event
	TIM2->SR &= ~TIM_SR_TIF;
		
	TIMER2_stop();
	
	UART_sendString("Time is gone. Data receive is ");
	if (lenghtOfDataPacket == 0) {
		UART_sendString("zero");  
		statusIncomingData = DATA_INC_NOREADY;
		lenghtData = lenghtOfDataPacket;
	} else {
		UART_sendOnlyNumber(lenghtOfDataPacket+48);
		statusIncomingData = DATA_INC_READY;
		lenghtData = lenghtOfDataPacket;
	}
	
	UART_sendString("  !   ");
	
	lenghtOfDataPacket = 0;
}

void USART1_IRQHandler(void)
{
	uint8_t received_data = 0;
	
	TIMER2_stop();
	received_data = USART1->DR;
	receive_array[lenghtOfDataPacket] = received_data;
	lenghtOfDataPacket++;
	
	TIMER2_wait_msec(DELAY_OF_DATA);
}

void InitAll(void)
{
	// before next code, this stuff is config auto:  RCC_CR = HSION | HSIRDY | HSITRIM[1<<4]
	
	RCC->CR |= RCC_CR_HSEON;										// ENABLE EXTERNALL CLOCK
	while ((RCC->CR & RCC_CR_HSERDY) == 0);			// WAIT UNTILL CLOCK WAS NOT STARTED
	RCC->CFGR |= RCC_CFGR_SW_0;									// TURN ON HSE
	RCC->APB2ENR |= RCC_APB2ENR_IOPAEN |				// PORT A
									RCC_APB2ENR_USART1EN;				// USART
	
	
 /*  GPIOA->CRL	&= ~GPIO_CRL_CNF3;							// General purpose output push-pull
 
   GPIOA->CRL   |= GPIO_CRL_MODE3_0;					// General purpose output push-pull
	*/
	USART1->CR1 |= USART_CR1_RXNEIE;
	
	// for led blink
	RCC->APB2ENR	|= RCC_APB2ENR_IOPAEN;
	GPIOA->CRL	&= ~GPIO_CRL_CNF0;							// General purpose output push-pull
  GPIOA->CRL   |= GPIO_CRL_MODE0_0;						// General purpose output push-pull
	
   return;   
}
void TIM3_IRQHandler(void){
/*	TIM2->SR &= ~TIM_SR_UIF;					// clear flag of event
	TIM2->SR &= ~TIM_SR_TIF;
		
	TIMER2_stop();
	

*/
// here we need to check the steps and decrement it. when steps == 0 - stop the timer
	
	
}

int main(void)
{ 
		int i = 0;
	InitAll();
	UART_Init();
	TIMER2_Init();
	SH_LASER_Init();
//	UART_sendString("hi! ");
	DriverBoard_Init();
	
	NVIC_EnableIRQ(TIM2_IRQn);
	NVIC_EnableIRQ(TIM3_IRQn);				// TIM1 Capture Compare Interrupt
	NVIC_EnableIRQ(USART1_IRQn);
	
// for what?	TIMER2_wait_msec(DELAY_OF_DATA);
	
	// DEFAULT SETTINGS
//	shpindle(SH_MODE_ON_RIGHT);
	

	while(1)	{ 						// other action makes in an interrupt
		
UART_sendString("hi! "); 
		
		if (statusIncomingData == DATA_INC_READY)
		{ 
			statusIncomingData = DATA_INC_NOREADY;
			// UART interrupt must be OFF, for parsing reseived data packet
			if(lenghtData < PACKET_DATA_SIZE_MIN){ 
				err_code = COMMAND_LESS_5_CHAR;
				UART_sendString("COMMAND_LESS_5_CHAR  ");				// these and next transmissions only for debug
				continue;
			}
			
			if((receive_array[0] == '[') && (receive_array[lenghtData] == ']'))		//check first and last char of data
			{
				err_code = START_STOP_UNCORR;
				UART_sendString("START_STOP_UNCORR  ");
				continue;
			}			
			UART_sendString("GO TO CRC8  ");
			crcResult = CRC8(&receive_array[1], lenghtData-2);
			UART_sendString("RETURN FROM CRC8  ");
			
//----------------------------------------------------------------------------------------			
			
			
/*			if (crcResult != 0)												// here we need to compare incoming and calculated data! NO WITH ZERO!
			{
				err_code = CRC_UNCORR;
				UART_sendString("CRC_UNCORR  ");
				continue;
			} */
			
			
// HERE STARTING TO ANALYSE THE DATA			-----------------------------------------------------------------------------------------------------------
			
			
			if (receive_array[INSTRUCTION] == MOV_STEPS_X_PLUS){
				UART_sendString("MOV_STEPS_X_PLUS  ");
			//	stepsX( (receive_array[2] << 8) | (receive_array[3]) );
			}
			
			
			
//----------------------------------------------------------------------------------------------------------------------------------------------------			
		/*  its temporary!!!!! change it when i done testing below		
			
			if (receive_array[1] == 'M')
			{
				if (receive_array[2] == 'S')
				{ 
					if (shpindle(SH_MODE_OFF) == NO_ERROR) UART_sendString("SH_MODE_OFF OK  ");					// stop shpindle, send ACK to PC
						else UART_sendString("ERROR!  ");				// NACK
				}
				
				if (receive_array[2] == 'L')
				{ // shpindle to left
					if (shpindle(SH_MODE_ON_LEFT) == NO_ERROR) UART_sendString("SH_MODE_ON_LEFT OK  ");					// stop shpindle, send ACK to PC
						else UART_sendString("ERROR!  ");				// NACK					
				}
				
				if (receive_array[2] == 'R')
				{ // shpindle to right
					if (shpindle(SH_MODE_ON_RIGHT) == NO_ERROR) UART_sendString("SH_MODE_ON_RIGHT OK  ");					// stop shpindle, send ACK to PC
						else UART_sendString("ERROR!  ");
				}				
			}			
		
			if ((receive_array[1] == 'E') && (receive_array[2] == 'R'))
			{	// error request
			}				
			
			if ((receive_array[1] == 'T') && (receive_array[2] == 'D'))
			{	// time delay
			}				

			if (receive_array[1] == 'S')
			{
				if (receive_array[2] == 'T')
				{ // stop programm
				}
				
				if (receive_array[2] == 'X')
				{ // shift to number of steps on X
				}
				
				if (receive_array[2] == 'Y')
				{ // shift to number of steps on Y
				}		

				if (receive_array[2] == 'Z')
				{ // shift to number of steps on Z
				}				
			}	

			if (receive_array[1] == 'Z')
			{	// move of Z axiss
				if (receive_array[2] == 'P')
				{ // to plus end
				}
				
				if (receive_array[2] == 'M')
				{ // to minus end
				}

				if (receive_array[2] == 'Z')
				{ // to zero of machine
				}				
			}	

			if (receive_array[1] == 'X')
			{	// move of X axiss
				if (receive_array[2] == 'P')
				{ // to plus end
				}
				
				if (receive_array[2] == 'M')
				{ // to minus end
				}

				if (receive_array[2] == 'Z')
				{ // to zero of machine
				}				
			}

			if (receive_array[1] == 'Y')
			{	// move of Z axiss
				if (receive_array[2] == 'P')
				{ // to plus end
				}
				
				if (receive_array[2] == 'M')
				{ // to minus end
				}

				if (receive_array[2] == 'Z')
				{ // to zero of machine
				}				
			}

			if (receive_array[1] == 'P')
			{	// pause
				if (receive_array[2] == 'E')
				{ // ..enable
				}
				
				if (receive_array[2] == 'D')
				{ // ..disable
				}			
			}

			if (receive_array[1] == 'A')
			{	// ADDITIONALLY
				if (receive_array[2] == 'L')
				{ // ..light
					if (receive_array[3] == 'N');		// ON
					else 
						if (receive_array[3] == 'F');		// OFF
				}
				
				if (receive_array[2] == 'F')
				{ // ..fan
					if (receive_array[3] == 'N');		// ON
					else
						if (receive_array[3] == 'F');		// OFF
				}			
			}			
			
			
		
		
		
		}
		
		*/
		
		
		
		
		
		}
		
		GPIOA->BSRR =GPIO_BSRR_BS0; 		
		for (i = 0; i < 500000; i++);
		
   	GPIOA->BSRR =GPIO_BSRR_BR0;
		for (i = 0; i < 100000; i++); 
		
		
		
	}
}
