/*			discription:
		 		library using for debug via UART. firstly was create to 1986ve1t.
				
				was create:			28.07.14
				rewrite to stm	08.09.14
				update:					07.10.14
				status: 				WORK

				need make:			

				other notes:        use as #include "debugUART.h"
				baud rate = 9600
				data bits 8 
				parity = none
				stop bit = 1
				handshaking = none
*/
#include "stdint.h"
#include "stm32f10x.h"


void UART_sendOnlyNumber(uint32_t message);
void UART_Init(void);
void UART_sendString(char * Text);
void UART_sendNumber(int32_t number);	
uint8_t sendData(char * Data);


/*
    EXAMPLE OF USE
#define debugOnUART

#ifdef  debugOnUART
sendMSG(message);
#endif

*/

uint8_t sendData(char * Data)
{
	
	return 0;
}
void UART_sendOnlyNumber(uint32_t message)
{
	while(((USART1->SR) & USART_SR_TC) != USART_SR_TC);	    //wait until Transmission is complete			  
	USART1->DR = (unsigned char)message; 
}

void UART_Init(void)
{	
	// clock 
	RCC->APB2ENR	|= RCC_APB2ENR_IOPAEN;						/*!< I/O port A clock enable */
  RCC->APB2ENR	|= RCC_APB2ENR_USART1EN;					/*!< USART1 clock enable */
	
	// pins
	GPIOA->CRH |= (0x03 << 4);						// MODE9[1:0]=11
	GPIOA->CRH &= ~(0x03 << 6);
	GPIOA->CRH |= (1 << 7);								// CNF9[1:0]=10,  it all mean: TX = alternate func. push-pull
	
	GPIOA->CRH &= ~(0x03 << 8);						// MODE10[1:0]=00 		clear mode10
	GPIOA->CRH &= ~(0x03 << 10);					// clear cnf10
	GPIOA->CRH |= (1 << 10);								// CNF10[1:0]=01,  it all mean: RX = input floating	
	
	// configure USART
	USART1->BRR = 0x341;									// for baud = 9600
	USART1->CR1 |= USART_CR1_TE | USART_CR1_RE;				/*!<Receiver Enable */ /*!<Transmitter Enable */ 
//	USART1->CR1
	
	USART1->CR1 |= USART_CR1_UE;					/*!<USART Enable */
}


void UART_sendString(char * Text)
{
	char i = 0;
     
  for (i = 0; Text[i] != 0; i++)
  {		
		while(((USART1->SR) & USART_SR_TC) != USART_SR_TC);	    //wait until Transmission is complete		  
	 	USART1->DR = Text[i];   
  }
		while(((USART1->SR) & USART_SR_TC) != USART_SR_TC);	    //wait until Transmission is complete		  
	 	USART1->DR = '\r';	
}		

void UART_sendNumber(int32_t number)
{
	char 	array[10], 				// 10 units i think are enought
				i = 0;	
	
	// erase array
	for (i = 0; i < 10; i++)
  {
			array[i] = 0;      
  }
	
	// main algorithm
  i = 0;
  while (number > 0)
  {
		  array[i] = number % 10;		 		  
      i++;
      number = number / 10;      
  }	
	
	// send to UART
  for (; i != 0; i--)
  {
			while(((USART1->SR) & USART_SR_TC) != USART_SR_TC);	    //wait until Transmission is complete			  
			USART1->DR = (array[i-1])+48;     	
	}	
}



/*
		if (((USART1->SR) & USART_SR_RXNE) != 0)   - if we receive something












*/
