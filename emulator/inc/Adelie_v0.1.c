/*
 * Adelie.c
 *
 * Created: 6/6/2019 6:03:19 PM
 *  Author: Joey
 */ 

#include <stdint.h>
#include "Adelie_v0.1.h"

#define ADELIE_INST_ALU_BR			31
#define ADELIE_INST_REG_IMM			30
#define ADELIE_INST_OP				26
#define ADELIE_INST_RD				21
#define ADELIE_INST_R1				16
#define ADELIE_INST_BR_IMM			0
#define ADELIE_INST_BR_IMM_SIGN		20
#define ADELIE_INST_R2				11
#define ADELIE_INST_ALU_IMM			0
#define ADELIE_INST_ALU_IMM_SIGN	15

#define ADELIE_INST_OP_MASK			0xFUL
#define ADELIE_INST_REG_MASK		0x1FUL
#define ADELIE_INST_BR_IMM_MASK		0x1FFFFFUL
#define ADELIE_INST_ALU_IMM_MASK	0xFFFF

typedef struct {
	int32_t brImm;
	int32_t aluImm;
	uint8_t rd;
	uint8_t r1;
	uint8_t r2;
	uint8_t aluBr : 1;
	uint8_t immReg : 1;
	uint8_t opcode : 4;
} instData_t;

uint8_t AdelieExecuteInstructionWord_0_1(adelieState_t* state)
{
	//The program jumped outside of memory.
	if ((state->registers[ADELIE_PC] > state->memTop) || (state->registers[ADELIE_PC] < state->memBottom))
	{
		state->error = ADELIE_ERROR_MISS;
		
		return 1;
	}
	
	//Get an instruction to execute.
	uint32_t instruction = state->memory[state->registers[ADELIE_PC]++];
	
	instData_t instData;
	
	instData.aluBr = (instruction >> ADELIE_INST_ALU_BR) & 0x1;
	instData.immReg = (instruction >> ADELIE_INST_REG_IMM) & 0x1;
	instData.opcode = (instruction >> ADELIE_INST_OP) & ADELIE_INST_OP_MASK;
	
	instData.rd = (instruction >> ADELIE_INST_RD) & ADELIE_INST_REG_MASK;
	instData.r1 = (instruction >> ADELIE_INST_R1) & ADELIE_INST_REG_MASK;
	instData.r2 = (instruction >> ADELIE_INST_R2) & ADELIE_INST_REG_MASK;
	
	instData.aluImm = (instruction >> ADELIE_INST_ALU_IMM) & ADELIE_INST_ALU_IMM_MASK;
	
	if (instData.aluBr == 0)
	{
		if (instData.immReg == 0)
		{
			//Register-branch
			if (instData.opcode == 0x0)
			{
				//jmp rd
				state->registers[ADELIE_LR] = state->registers[ADELIE_PC];
				state->registers[ADELIE_PC] = state->registers[instData.r1];
			}
			else if (instData.opcode == 0x1)
			{
				//br rd r1
				if (state->registers[instData.rd])
				{
					state->registers[ADELIE_LR] = state->registers[ADELIE_PC];
					state->registers[ADELIE_PC] = state->registers[instData.r1];
				}
			}
			else
			{
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
			
			if (0x0 == instData.opcode)
			{
				//jmp #
				state->registers[ADELIE_PC] += instData.brImm;
			}
			else if (0x1 == instData.opcode)
			{
				//br rd #
				if (state->registers[instData.rd])
				{
					state->registers[ADELIE_PC] += state->registers[instData.brImm];
				}
			}
			else
			{
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
				//shr rd r1
				state->registers[instData.rd] = state->registers[instData.r1] >> 1;
				break;
			
			case 0x1:
				//mov rd r1
				state->registers[instData.rd] = state->registers[instData.r1];
				break;
			
			case 0x2:
				//ld rd [r1]
				state->registers[instData.rd] = state->memory[state->registers[instData.r1]];
				break;
			
			case 0x3:
				//sr [rd] r1
				state->memory[state->registers[instData.rd]] = state->registers[instData.r1];
				break;
			
			case 0x4:
				//add rd r1 r2
				state->registers[instData.rd] = (int32_t) state->registers[instData.r1] + (int32_t) state->registers[instData.r2];
				break;
			
			case 0x5:
				//sub rd r1 r2
				state->registers[instData.rd] = (int32_t) state->registers[instData.r1] - (int32_t) state->registers[instData.r2];
				break;
			
			case 0x6:
				//xor rd r1 r2
				state->registers[instData.rd] = state->registers[instData.r1] ^ state->registers[instData.r2];
				break;
			
			case 0x7:
				//and rd r1 r2
				state->registers[instData.rd] = state->registers[instData.r1] & state->registers[instData.r2];
				break;
			
			case 0x8:
				//or rd r1 r2
				state->registers[instData.rd] = state->registers[instData.r1] | state->registers[instData.r2];
				break;
			
			case 0x9:
				//eq rd r1 r2
				state->registers[instData.rd] = (int32_t) state->registers[instData.r1] == (int32_t) state->registers[instData.r2];
				break;
			
			case 0xA:
				//neq rd r1 r2
				state->registers[instData.rd] = (int32_t) state->registers[instData.r1] != (int32_t) state->registers[instData.r2];
				break;
			
			case 0xB:
				//gtr rd r1 r2
				state->registers[instData.rd] = (int32_t) state->registers[instData.r1] > (int32_t) state->registers[instData.r2];
				break;
			
			case 0xC:
				//les rd r1 r2
				state->registers[instData.rd] = (int32_t) state->registers[instData.r1] < (int32_t) state->registers[instData.r2];
				break;
			
			case 0xD:
				//gte rd r1 r2
				state->registers[instData.rd] = (int32_t) state->registers[instData.r1] >= (int32_t) state->registers[instData.r2];
				break;
			
			case 0xE:
				//lse rd r1 r2
				state->registers[instData.rd] = (int32_t) state->registers[instData.r1] <= (int32_t) state->registers[instData.r2];
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
			
			uint8_t signBit = instData.aluImm & ADELIE_INST_ALU_IMM_SIGN;
			
			//sign extend.
			instData.aluImm = (signBit == 0) ? instData.aluImm : instData.aluImm | ~ADELIE_INST_ALU_IMM_MASK;
			
			switch (instData.opcode)
			{
			case 0x0:
				//shr rd #
				state->registers[instData.rd] = instData.aluImm >> 1;
				break;
			
			case 0x1:
				//ldi rd #
				state->registers[instData.rd] = instData.aluImm;
				break;
			
			case 0x2:
				//ld rd [#]
				state->registers[instData.rd] = state->memory[(uint32_t) instData.aluImm];
				break;
			
			case 0x3:
				//st [#] r1
				state->memory[(uint32_t) instData.aluImm] = state->registers[instData.r1];
				break;
			
			case 0x4:
				//add rd r1 #
				state->registers[instData.rd] = (int32_t) state->registers[instData.r1] + instData.aluImm;
				break;
			
			case 0x5:
				//sub rd r1 #
				state->registers[instData.rd] = (int32_t) state->registers[instData.r1] - instData.aluImm;
				break;
			
			case 0x6:
				//xor rd r1 #
				state->registers[instData.rd] = state->registers[instData.r1] ^ instData.aluImm;
				break;
			
			case 0x7:
				//and rd r1 #
				state->registers[instData.rd] = state->registers[instData.r1] & instData.aluImm;
				break;
			
			case 0x8:
				//or rd r1 #
				state->registers[instData.rd] = state->registers[instData.r1] | instData.aluImm;
				break;
			
			case 0x9:
				//eq rd r1 #
				state->registers[instData.rd] = (int32_t) state->registers[instData.r1] == instData.aluImm;
				break;
			
			case 0xA:
				//neq rd r1 #
				state->registers[instData.rd] = (int32_t) state->registers[instData.r1] != instData.aluImm;
				break;
			
			case 0xB:
				//gtr rd r1 #
				state->registers[instData.rd] = (int32_t) state->registers[instData.r1] > instData.aluImm;
				break;
			
			case 0xC:
				//les rd r1 #
				state->registers[instData.rd] = (int32_t) state->registers[instData.r1] < instData.aluImm;
				break;
			
			case 0xD:
				//gte rd r1 #
				state->registers[instData.rd] = (int32_t) state->registers[instData.r1] >= instData.aluImm;
				break;
			
			case 0xE:
				//lse rd r1 #
				state->registers[instData.rd] = (int32_t) state->registers[instData.r1] <= instData.aluImm;
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