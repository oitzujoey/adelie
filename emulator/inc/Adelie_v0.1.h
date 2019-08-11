/*
 * Adelie.h
 *
 * Created: 6/6/2019 6:03:00 PM
 *  Author: Joey
 */ 


#ifndef ADELIE_H_
#define ADELIE_H_



//CPU registers
#define ADELIE_ZERO	0
#define ADELIE_PC	1
#define ADELIE_LR	2

//Number of CPU registers
#define ADELIE_REG_NUM	32

#define ADELIE_ERROR_MISS				1<<0
#define ADELIE_ERROR_ILLEGAL_INSTRUCION	1<<1

typedef struct {
	uint32_t *memory;
	uint32_t *registers;
	uint32_t memBottom;
	uint32_t memTop;
	uint8_t error;
} adelieState_t;

extern uint8_t AdelieExecuteInstructionWord_0_1(adelieState_t*);


#endif /* ADELIE_H_ */