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
#include "D:\development\GitHub\CNC-Embedded-Software\Software\lib\veres_defines_list.h"
#include "lib\veres_DriverBoard.h"

//#define DEBUG_MODE

  #ifdef DEBUG_MODE
  #endif

#define	DELAY_OF_DATA							10		// time for delaying before next data packet is coming
#define DATA_INC_READY						1
#define DATA_INC_NOREADY					0
#define PACKET_DATA_SIZE_MAX			30
#define PACKET_DATA_SIZE_MIN			5

// const
const uint8_t CNC_ID = 86;


// variables
uint8_t lenghtOfDataPacket = 0,
				receive_array[PACKET_DATA_SIZE_MAX],
				statusIncomingData = DATA_INC_NOREADY,
				lenghtData = 0,
				err_code = SUCCESS,
				crcResult = 0;
				
uint32_t 	globalCoordinateX = 0,
					globalCoordinateY = 0,
					globalCoordinateZ = 0,
					localCoordinateX = 0,
					localCoordinateY = 0,
					localCoordinateZ = 0,
					changeToolCoordinateX = 0,
					changeToolCoordinateY = 0,
					changeToolCoordinateZ = 0;
					
					
const	char ACK[5] = { '[', ACKNOWLEDGE, 1, ']', '\0' };
char NACK[6] = { '[', NEGATIVE_ACKNOWLEDGE, 0, 1, ']', '\0' }; 

// interrupts
void TIM2_IRQHandler(void);
void USART1_IRQHandler(void);
void TIM3_IRQHandler(void);
void TIM1_UP_IRQHandler(void);

// functions
void sendACK(void);
void sendNACK(uint8_t errno);


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
void TIM1_UP_IRQHandler(void){
	
	if (counterStepsX > REPETITION_VALUE_MAX){
		TIM1->RCR = REPETITION_VALUE_MAX - 1;
		counterStepsX -= REPETITION_VALUE_MAX;
	}else{
		if (counterStepsX != 0){											
			TIM1->RCR = counterStepsX - 1;
			counterStepsX = 0;
		}else{ // counterStepsX == 0 right now
			// set success for moving status
		}
	}
	
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
		

		
					TIM1->SR &= ~TIM_SR_UIF;
			TIM1->CR1 |= TIM_CR1_CEN; 									// Enable the TIM peripheral 
		
		
		
		if (statusIncomingData == DATA_INC_READY)
		{ /*
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
		*/	
//----------------------------------------------------------------------------------------			
			
			
/*			if (crcResult != 0)												// here we need to compare incoming and calculated data! NO WITH ZERO!
			{
				err_code = CRC_UNCORR;
				UART_sendString("CRC_UNCORR  ");
				continue;
			} */
			
			
// HERE STARTING TO ANALYSE THE DATA			-----------------------------------------------------------------------------------------------------------
			
			if (receive_array[INSTRUCTION] == GET_END_SENSORS_STATUS){
				#ifdef DEBUG_MODE
				UART_sendString("GET_END_SENSORS_STATUS");
				#endif
				
				char temp[3] = {GET_END_SENSORS_STATUS,
												settings.endSensorZminus<<5 | settings.endSensorZplus<<4 | settings.endSensorYminus<<3 | settings.endSensorYplus<<2 | settings.endSensorXminus<<1 | settings.endSensorXplus<<0,
												CRC8(temp, 2)};
				sendData(temp);
			}
			
			if (receive_array[INSTRUCTION] == SET_LOCAL_COORDINATES_TO_ZERO){
				#ifdef DEBUG_MODE
				UART_sendString("SET_LOCAL_COORDINATES_TO_ZERO");
				#endif
				localCoordinateX = localCoordinateY = localCoordinateZ = 0;
				sendACK();
			}
			

			if (receive_array[INSTRUCTION] == MAKE_BELL){
				#ifdef DEBUG_MODE
				UART_sendString("MAKE_BELL");
				#endif

				sendACK();
			}
	/*		
			
			
			if (receive_array[INSTRUCTION] == MOV_STEPS_X_PLUS){
				UART_sendString("MOV_STEPS_X_PLUS  ");
			//	stepsX( (receive_array[2] << 8) | (receive_array[3]) );
			}
		
			if (receive_array[INSTRUCTION] == MOV_STEPS_X_MINUS){
				UART_sendString("MOV_STEPS_X_MINUS  ");
			//	stepsX( (receive_array[2] << 8) | (receive_array[3]) );
			}

			if (receive_array[INSTRUCTION] == MOV_STEPS_Y_PLUS){
				UART_sendString("MOV_STEPS_Y_PLUS  ");
			//	stepsX( (receive_array[2] << 8) | (receive_array[3]) );
			}

			if (receive_array[INSTRUCTION] == MOV_STEPS_Y_MINUS){
				UART_sendString("MOV_STEPS_Y_MINUS  ");
			//	stepsX( (receive_array[2] << 8) | (receive_array[3]) );
			}

			if (receive_array[INSTRUCTION] == MOV_STEPS_Z_PLUS){
				UART_sendString("MOV_STEPS_Z_PLUS  ");
			//	stepsX( (receive_array[2] << 8) | (receive_array[3]) );
			}

			if (receive_array[INSTRUCTION] == MOV_STEPS_Z_MINUS){
				UART_sendString("MOV_STEPS_Z_MINUS  ");
			//	stepsX( (receive_array[2] << 8) | (receive_array[3]) );
			}			
			
			if (receive_array[INSTRUCTION] == MOV_CARRIAGE_FREE){
				UART_sendString("MOV_CARRIAGE_FREE");
				// here we convert % to uint16_t value and set it to settings
				settings.freeMove = 0;
//				stepsX( (receive_array[DATA] << 8) | (receive_array[3]) );
			}

			if (receive_array[INSTRUCTION] == MOV_CARRIAGE_WORK){
				UART_sendString("MOV_CARRIAGE_WORK");
				settings.workMove = 0;
//				stepsX( (receive_array[2] << 8) | (receive_array[3]) );
			}			
			
			if (receive_array[INSTRUCTION] == MOV_CARRIAGE_AT_Z_FREE){
				UART_sendString("MOV_CARRIAGE_AT_Z_FREE");
				// here we convert % to uint16_t value and set it to settings
				settings.freeMoveAtZ = 0;
			}

			if (receive_array[INSTRUCTION] == MOV_CARRIAGE_AT_Z_WORK){
				UART_sendString("MOV_CARRIAGE_AT_Z_WORK");
				settings.workMoveAtZ = 0;
			}	
			
			if (receive_array[INSTRUCTION] == SHPINDLE_MODE){
				UART_sendString("SHPINDLE_MODE");
				shpindleMode(receive_array[DATA]);
			}			
			
			if (receive_array[INSTRUCTION] == GOTO_COMMAND){
				UART_sendString("SHPINDLE_MODE");
				
				switch (receive_array[DATA]) {
					case GOTO_HOME: 
						
						break;
					
					case GOTO_LOCAL_ZERO: 
						
						break;		

					case GOTO_X_PLUS: 
						
						break;

					case GOTO_X_MINUS: 
						
						break;

					case GOTO_Y_PLUS: 
						
						break;

					case GOTO_Y_MINUS: 
						
						break;

					case GOTO_Z_PLUS: 
						
						break;

					case GOTO_Z_MINUS: 
						
						break;		
					default:
						UART_sendString("error in goto data");
				}
							
			}	
			
			if (receive_array[INSTRUCTION] == END_SONSORS_STATUS){
				UART_sendString("END_SONSORS_STATUS");
				
			}	
			*/
//----------------------------------------------------------------------------------------------------------------------------------------------------			
		/*  its temporary!!!!! change it when i done testing below		
			
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
		for (i = 0; i < 500000; i++); 
		
		
		
	}
}

void sendACK(void)
{
	sendData(ACK);
}

void sendNACK(uint8_t errno)
{
	NACK[2] = errno;
	NACK[3] = CRC8(NACK[1], 2);
	sendData(NACK);
}

