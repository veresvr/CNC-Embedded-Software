/*			discription:
		 		
				
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

// defines for state of return by functions
#define SUCCESS 				0
#define UNKNOWN_COMMAND 1
 

uint8_t decode_exellon_data(char * Text);
							


uint8_t decode_exellon_data(char * Text)
{

	
	
	
	
	
	
	
	return 0;
}

/*
1. in function TIMER2_waitSec(uint32_t msec) : variable sec must be the one of [1 .. 65 535] range. 

% 				Rewind and Stop
X#Y# 			Move and Drill
T# 				Tool Selection
M30 			End of Program
M00 			End of Program
M25 			Beginning of Pattern
M31 			Beginning of Pattern
M01 			End of Pattern
M02 X#Y# 	Repeat Pattern
R#M02X#Y# Multiple Repeat Pattern
M02 X#Y# 	M70 Swap Axis
M02 X#Y# 	M80 Mirror Image X Axis
M02 X#Y# 	M90 Mirror Image Y Axis
M08 			End of Step and Repeat
N# 				Block Sequence Number
/ 				Block Delete
R#X#Y# 		Repeat Hole
G05, G81 	Select Drill Mode
G90 			Absolute Mode
G91 			Incremental Mode
G92 X#Y# 	Set Zero
G93 X#Y# 	Set Zero
M48 			Program Header to first "%"
M47 			Operator Message CRT Display
M71 			Metric Mode
M72 			English-Imperial Mode
Snn 			Spindle Speed (RPM)
Fnn 			Z axis feed speed (IPM)



*/
