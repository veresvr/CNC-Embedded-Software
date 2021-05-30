/*			discription:
		 		library configure timer2 for generate an interrupt on timer counter overflow
				
				was create:			26.05.2015
				rewrite to stm	
				update:					
				status: 				WORK

				need make:			

				other notes:        
				Tyrkin Vladimir, ru.veres@yandex.ru
*/
#include "stdint.h"
#include "stm32f10x.h"

#define FIRST_CRC_CALCULATE_CHAR	1
#define FIRST_CRC_NUMB						3
#define LOW_CRC_NUMB							4

// variables
uint8_t			valueLowPart = 0,
						valueHighPart = 0;


void crc8(uint8_t *calc_arr, uint8_t lenght);						// lenght its the last value, which must be calculated
uint8_t decodeMsg(uint8_t *msg_arr, uint8_t lenght);
						



void crc8(uint8_t *calc_arr, uint8_t lenght)
{
	uint8_t		i = 0,
						crcResult = calc_arr[FIRST_CRC_CALCULATE_CHAR];

        for (i = FIRST_CRC_CALCULATE_CHAR+1; i < lenght+1; i++)              									
        {
                crcResult = crcResult ^ calc_arr[i];
        }
				
        valueLowPart = (crcResult & ~((1<<4)|(1<<5)|(1<<6)|(1<<7)) ) + 48;     // +48 to numbers
        if (valueLowPart > 57) valueLowPart += 7;                             // if not number, goto to chars

        valueHighPart = (crcResult >> 4) + 48;
        if (valueHighPart > 57) valueHighPart += 7;	

}

uint8_t decodeMsg(uint8_t *msg_arr, uint8_t lenght)
{
// first of all
		crc8(msg_arr, lenght-FIRST_CRC_NUMB);																// lenght-FIRST_CRC_NUMB its a shift to the last calculate value
	
		if ( (msg_arr[lenght-FIRST_CRC_NUMB] != valueHighPart) 
				& (msg_arr[lenght-FIRST_CRC_NUMB+1] != valueLowPart) )
		{
				return 1;				// if numbers is not equal - crc error
		}

// secondary - chek other numbers of packet		
	
	
	
	
	
	
		return 0; 			// return if success
}

/*
1. 	Function crc8(receive_arr, lenght)	makes two 8bit values from original "receive_arr".
		Value "lenght" mean a char which is the last in calculate procedure.
		Example: if you want to calculate crc in array "12345678" to char 6, you must write like that: crc8(receive_arr, 5).
		Two 8bit registers are difined as global variables, and called valueLowPart and valueHighPart respectively.

2.	Function decodeMsg(uint8_t *msg_arr) check received array and call corresponding functions.
		If any error occured - returns number of error; zero if all correct.

		List of errors:
		0 - no error
		1 - crc calculate error (receive values and calculated are not the same)

*/
