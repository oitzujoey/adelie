/*
 * Adelie_v0.2.c
 *
 * Created: 6/7/2019 2:14:17 PM
 *  Author: Joey
 */ 

#include <stdint.h>
#include "Adelie_v0.2.h"

#define ADELIE_INST_ALU_BR			30
#define ADELIE_INST_REG_IMM			31
#define ADELIE_INST_OP				26
#define ADELIE_INST_RD				21
#define ADELIE_INST_RS				16
#define ADELIE_INST_BR_IMM			0
#define ADELIE_INST_BR_IMM_SIGN		20
#define ADELIE_INST_ALU_IMM			0
#define ADELIE_INST_ALU_IMM_SIGN	20

#define ADELIE_INST_OP_MASK			0xFUL
#define ADELIE_INST_REG_MASK		0x1FUL
#define ADELIE_INST_BR_IMM_MASK		0x1FFFFFUL
#define ADELIE_INST_ALU_IMM_MASK	0x1FFFFFUL

typedef struct {
	int32_t brImm;
	int32_t aluImm;
	uint8_t rd;
	uint8_t rs;
	uint8_t aluBr : 1;
	uint8_t immReg : 1;
	uint8_t opcode : 4;
} instData_t;

void adelieReset_0_2(adelieState_t *state, uint32_t pc)
{
	for (uint8_t i = 0; i < ADELIE_REG_NUM; i++)
	{
		state->registers[i] = 0;
	}
	
	state->registers[ADELIE_PC] = pc;
}

uint8_t AdelieExecuteInstructionWord_0_2(adelieState_t* state)
{
	//The program jumped outside of allocated memory.
	if (((state->registers[ADELIE_PC] >> 1) > state->memTop) || ((state->registers[ADELIE_PC] >> 1) < state->memBottom))
	{
		state->error = ADELIE_ERROR_MISS;
		
		return 1;
	}
	
	//Parse instruction word.
	instData_t instData;
	
	//Get an instruction to execute.
	uint32_t instruction;
	
	if ((state->registers[ADELIE_PC] % 2) == 0)
	{
		//The PC is aligned with the beginning of a word, the simple case.
		instruction = state->memory[state->registers[ADELIE_PC] >> 1];
		
		instData.immReg = (instruction >> ADELIE_INST_REG_IMM) & 0x1;
		
		if (instData.immReg == 0)
		{
			state->registers[ADELIE_PC]++;
		}
		else
		{
			state->registers[ADELIE_PC] += 2;
		}
	}
	else
	{
		//Load the high 16 bits of the instruction.
		instruction = state->memory[state->registers[ADELIE_PC] >> 1];
		instruction <<= 16;
		
		instData.immReg = (instruction >> ADELIE_INST_REG_IMM) & 0x1;
		
		state->registers[ADELIE_PC]++;
		
		if (instData.immReg == 0)
		{
			//That was all we needed.
		}
		else
		{
			//Load the low byte of the instruction with the high byte of memory.
			//Don't you love variable length instructions?
			instruction |= state->memory[state->registers[ADELIE_PC] >> 1] >> 16;
			state->registers[ADELIE_PC]++;
		}
	}
	
	instData.aluBr = (instruction >> ADELIE_INST_ALU_BR) & 0x1;
	instData.opcode = (instruction >> ADELIE_INST_OP) & ADELIE_INST_OP_MASK;
	
	instData.rd = (instruction >> ADELIE_INST_RD) & ADELIE_INST_REG_MASK;
	instData.rs = (instruction >> ADELIE_INST_RS) & ADELIE_INST_REG_MASK;
	
	if (instData.aluBr == 0)
	{
		if (instData.immReg == 0)
		{
			//Register-branch
			
			switch (instData.opcode)
			{
			case 0x0:
				//jmp rd
				state->registers[ADELIE_PC] = state->registers[instData.rs];
				break;
			
			case 0x1:
				//br rd rs
				if (state->registers[instData.rd])
				{
					state->registers[ADELIE_PC] = state->registers[instData.rs];
				}
				break;
			
			case 0x2:
				//jal rs
				state->registers[ADELIE_LR] = state->registers[ADELIE_PC];
				state->registers[ADELIE_PC] = state->registers[instData.rs];
				break;
			
			case 0x3:
				//bal rd rs
				if (state->registers[instData.rd])
				{
					state->registers[ADELIE_LR] = state->registers[ADELIE_PC];
					state->registers[ADELIE_PC] = state->registers[instData.rs];
				}
				break;
			
			default:
				state->error = ADELIE_ERROR_ILLEGAL_INSTRUCION;
				return 1;
			}
		}
		else
		{
			//Immediate-branch
			
			//Convert 21 bit unsigned immediate to 32 bit signed
			instData.brImm = (instruction >> ADELIE_INST_BR_IMM) & ADELIE_INST_BR_IMM_MASK;
			
			uint8_t signBit = instData.brImm & ADELIE_INST_BR_IMM_SIGN;
			
			//sign extend.
			instData.brImm = (signBit == 0) ? instData.brImm : instData.brImm | ~ADELIE_INST_BR_IMM_MASK;
			
			switch (instData.opcode)
			{
			case 0x0:
				//jmp #
				state->registers[ADELIE_PC] += instData.brImm;
				break;
				
			case 0x1:
				//br rd #
				if (state->registers[instData.rd])
				{
					state->registers[ADELIE_PC] += instData.brImm;
				}
				break;
				
			case 0x2:
				//jal #
				state->registers[ADELIE_LR] = state->registers[ADELIE_PC];
				state->registers[ADELIE_PC] += instData.brImm;
				break;
				
			case 0x3:
				//bal rd #
				if (state->registers[instData.rd])
				{
					state->registers[ADELIE_LR] = state->registers[ADELIE_PC];
					state->registers[ADELIE_PC] += instData.brImm;
				}
				break;
				
			default:
				state->error = ADELIE_ERROR_ILLEGAL_INSTRUCION;
				return 1;
			}
		}
	}
	else
	{
		if (instData.immReg == 0)
		{
			//Register-ALU
			switch (instData.opcode)
			{
			case 0x0:
				//shr rd rs
				state->registers[instData.rd] = state->registers[instData.rs] >> 1;
				break;
				
			case 0x1:
				//mov rd rs
				state->registers[instData.rd] = state->registers[instData.rs];
				break;
				
			case 0x2:
				//ld rd [rs]
				
				state->registers[instData.rd] = state->memory[state->registers[instData.rs] >> 1];
				break;
				
			case 0x3:
				//st [rd] rs
				state->memory[state->registers[instData.rd] >> 1] = state->registers[instData.rs];
				break;
				
			case 0x4:
				//add rd rs
				state->registers[instData.rd] += (int32_t) state->registers[instData.rs];
				break;
				
			case 0x5:
				//sub rd rs
				state->registers[instData.rd] -= (int32_t) state->registers[instData.rs];
				break;
				
			case 0x6:
				//xor rd rs
				state->registers[instData.rd] ^= state->registers[instData.rs];
				break;
				
			case 0x7:
				//and rd rs
				state->registers[instData.rd] &= state->registers[instData.rs];
				break;
				
			case 0x8:
				//or rd rs
				state->registers[instData.rd] |= state->registers[instData.rs];
				break;
				
			case 0x9:
				//eq rd rs
				state->registers[instData.rd] = (int32_t) state->registers[instData.rd] == (int32_t) state->registers[instData.rs];
				break;
				
			case 0xA:
				//neq rd rs
				state->registers[instData.rd] = (int32_t) state->registers[instData.rd] != (int32_t) state->registers[instData.rs];
				break;
				
			case 0xB:
				//gtr rd rs
				state->registers[instData.rd] = (int32_t) state->registers[instData.rd] > (int32_t) state->registers[instData.rs];
				break;
				
			case 0xC:
				//les rd rs
				state->registers[instData.rd] = (int32_t) state->registers[instData.rd] < (int32_t) state->registers[instData.rs];
				break;
				
			case 0xD:
				//gte rd rs
				state->registers[instData.rd] = (int32_t) state->registers[instData.rd] >= (int32_t) state->registers[instData.rs];
				break;
				
			case 0xE:
				//lse rd rs
				state->registers[instData.rd] = (int32_t) state->registers[instData.rd] <= (int32_t) state->registers[instData.rs];
				break;
				
			default:
				state->error = ADELIE_ERROR_ILLEGAL_INSTRUCION;
				return 1;
			}
		}
		else
		{
			//Immediate-ALU
			
			//Convert 16 bit unsigned immediate to 32 bit signed
			instData.aluImm = (instruction >> ADELIE_INST_ALU_IMM) & ADELIE_INST_ALU_IMM_MASK;
			
			uint8_t signBit = (instData.aluImm >> ADELIE_INST_ALU_IMM_SIGN) & 1;
			
			//sign extend.
			instData.aluImm = (signBit == 0) ? instData.aluImm : instData.aluImm | ~ADELIE_INST_ALU_IMM_MASK;
			
			switch (instData.opcode)
			{
			case 0x0:
				//shr rd #
				state->registers[instData.rd] = (uint32_t) instData.aluImm >> 1;
				break;
				
			case 0x1:
				//ldi rd #
				state->registers[instData.rd] = instData.aluImm;
				break;
				
			case 0x2:
				//ld rd [#]
				state->registers[instData.rd] = state->memory[instData.aluImm >> 1];
				break;
				
			case 0x3:
				//st [#] rs
				state->memory[instData.aluImm >> 1] = state->registers[instData.rd];
				break;
				
			case 0x4:
				//add rd #
				state->registers[instData.rd] = (int32_t) state->registers[instData.rd] + instData.aluImm;
				break;
				
			case 0x5:
				//sub rd #
				state->registers[instData.rd] = (int32_t) state->registers[instData.rd] - instData.aluImm;
				break;
				
			case 0x6:
				//xor rd #
				state->registers[instData.rd] = state->registers[instData.rd] ^ (uint32_t) instData.aluImm;
				break;
				
			case 0x7:
				//and rd #
				state->registers[instData.rd] = state->registers[instData.rd] & (uint32_t) instData.aluImm;
				break;
				
			case 0x8:
				//or rd #
				state->registers[instData.rd] = state->registers[instData.rd] | (uint32_t) instData.aluImm;
				break;
				
			case 0x9:
				//eq rd #
				state->registers[instData.rd] = (int32_t) state->registers[instData.rd] == instData.aluImm;
				break;
				
			case 0xA:
				//neq rd #
				state->registers[instData.rd] = (int32_t) state->registers[instData.rd] != instData.aluImm;
				break;
				
			case 0xB:
				//gtr rd #
				state->registers[instData.rd] = (int32_t) state->registers[instData.rd] > instData.aluImm;
				break;
				
			case 0xC:
				//les rd #
				state->registers[instData.rd] = (int32_t) state->registers[instData.rd] < instData.aluImm;
				break;
				
			case 0xD:
				//gte rd #
				state->registers[instData.rd] = (int32_t) state->registers[instData.rd] >= instData.aluImm;
				break;
				
			case 0xE:
				//lse rd #
				state->registers[instData.rd] = (int32_t) state->registers[instData.rd] <= instData.aluImm;
				break;
				
			default:
				state->error = ADELIE_ERROR_ILLEGAL_INSTRUCION;
				return 1;
			}
		}
	}
	
	state->registers[ADELIE_ZERO] = 0x0;
	
	return 0;
}