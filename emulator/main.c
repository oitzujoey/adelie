/*
 * Adelie_Emulator.c
 *
 * Created: 6/6/2019 6:02:27 PM
 * Author : Joey
 */ 

#include <stdlib.h>
#include <avr/io.h>

#define ADELIE_VERSION_0_2

#if defined ADELIE_VERSION_0_1
#include "inc/Adelie_v0.1.h"
#elif defined ADELIE_VERSION_0_2
#include "inc/Adelie_v0.2.h"
#endif

#define MEMSIZE 64


adelieState_t state;

void setMem16(uint32_t instruction)
{
	static uint16_t index = 0;
	
	if ((index % 2) == 0)
	{
		state.memory[index / 2] = instruction << 16;
	}
	else
	{
		state.memory[index / 2] |= instruction;
	}
	
	index++;
}

int main(void)
{
	
#ifdef ADELIE_VERSION_0_1

	state.memory = calloc(MEMSIZE, sizeof(uint32_t));
	state.registers = calloc(ADELIE_REG_NUM, sizeof(uint32_t));
	
	while ((NULL == state.memory) || (NULL == state.registers));
	
	state.memBottom = 0;
	state.memTop = 64;
	
	//                  ||{op}{des}{r1 }{  immediate   }
	state.memory[0] = 0b11010010000100000000000000000001;
	state.memory[1] = 0b00000000000000000000000000000000;
	
	while (1)
	{
		AdelieExecuteInstructionWord_0_1(&state);
	}
	
#elif defined ADELIE_VERSION_0_2
	
	DDRD = 0xFF;
	
	state.memory = calloc(MEMSIZE, sizeof(uint32_t));
	
	while (NULL == state.memory);
	
	state.memBottom = (uint16_t) 0;
	state.memTop = (uint16_t) MEMSIZE - 1;
	
	state.registers[ADELIE_PC] = state.memBottom;
	
	adelieReset_0_2(&state, 0);
	
	/*Fib
	
	*/
	
	/*
		.def a = r3
		.def b = r4
		.def i = r5
		.def t0 = r6
		.def fib = r7
		.def PORTB = r31
		
		ldi a 0
		ldi b 1
		ldi i 0
		
	loop:
		t0 = i
		t0 = t0 >= 100
		br t0 end
			
			fib = a
			a = b
			fib += b;
			b = fib;
		
			PORTB = fib;
		
			i += 1
		
			jmp loop
		
	end:
		jmp end
	*/
	
	//mov r3 r0		{1'0,1'1,4'h1,5'rd,5'rs}	0
	setMem16(0b0100010001100000);
	//ldi r4 1		{1'1,1'1,4'h1,5'rd,21'#}	1
	setMem16(0b1100010010000000);
	setMem16(0b0000000000000001);
	//mov r5 r0		{1'0,1'1,4'h1,5'rd,5'rs}	3
	setMem16(0b0100010010100000);
	
	//loop: 0000_0004
	//mov r6 r5		{1'0,1'1,4'h1,5'rd,5'rs}	4
	setMem16(0b0100010011000101);
	//gte r6 0x1FFFFF	{1'1,1'1,4'hd,5'rd,21'#}	5
	setMem16(0b1111010011001111);
	setMem16(0b1111111111111111);
	//br r6 end		{1'1,1'0,4'h1,5'rd,21'#}	7
	setMem16(0b1000010011000000);
	setMem16(0b0000000000001000);
	//mov r7 r3		{1'0,1'1,4'h1,5'rd,5'rs}	9
	setMem16(0b0100010011100011);
	//mov r3 r4		{1'0,1'1,4'h1,5'rd,5'rs}	a
	setMem16(0b0100010001100100);
	//add r4 r7		{1'0,1'1,4'h4,5'rd,5'rs}	b
	setMem16(0b0101000010000111);
	//mov r31 r7	{1'0,1'1,4'h1,5'rd,5'rs}	c
	setMem16(0b0100011111100111);
	//add r5 1		{1'1,1'1,4'h4,5'rd,21'#}	d
	setMem16(0b1101000010100000);
	setMem16(0b0000000000000001);
	//jmp loop		{1'1,1'0,4'h0,5'x,21'#}		f
	setMem16(0b1000001111111111);
	setMem16(0b1111111111110011);
	
	//end: 0000_0011
	//jmp end		{1'1,1'0,4'h0,5'x,21'#}		11
	setMem16(0b1000001111111111);
	setMem16(0b1111111111111110);
	
	while (1)
	{
		if (AdelieExecuteInstructionWord_0_2(&state))
		{
			state.error = 0;
			//Might as well do a reset.
			adelieReset_0_2(&state, state.memBottom);
		}
		
		PORTD = ~state.registers[ADELIE_REG_NUM - 1];
	}
	
#endif
}

