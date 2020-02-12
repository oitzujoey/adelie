/* Disassemble Adelie instructions.

   Copied from moxie-dis.c which is..
   Copyright (C) 2009-2019 Free Software Foundation, Inc.

   This file is part of the GNU opcodes library.

   This library is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3, or (at your option)
   any later version.

   It is distributed in the hope that it will be useful, but WITHOUT
   ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
   or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
   License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street - Fifth Floor, Boston,
   MA 02110-1301, USA.  */

#include "sysdep.h"
#include <stdio.h>

#define STATIC_TABLE
#define DEFINE_TABLE

#include "opcode/adelie.h"
#include "disassemble.h"

static fprintf_ftype fpr;
static void *stream;

/* Macros to extract operands from the instruction word.  */
#define OP_R0(i) ((i >> 19) & 0x1F)
#define OP_R1(i) ((i >> 14) & 0x1F)
#define OP_R2(i) ((i >> 9) & 0x1F)
// #define INST2OFFSET(o) ((((signed short)((o & ((1<<10)-1))<<6))>>6)<<1)

// static const char * reg_names[16] =
//   { "$fp", "$sp", "$r0", "$r1", "$r2", "$r3", "$r4", "$r5",
//     "$r6", "$r7", "$r8", "$r9", "$r10", "$r11", "$r12", "$r13" };

//  
int
print_insn_adelie (bfd_vma addr, struct disassemble_info * info)
{
  //  Instruction length in bytes
  int length = 0;
  int status;
  stream = info->stream;
  //  Temporary buffer to store bytes from memory
  bfd_byte buffer[3];
  //  Opcode information
  const adelie_opc_info_t * opcode;
  //  Instruction word
  unsigned long iword;
  
  fpr = info->fprintf_func;

  //  Read opcode from memory
  if ((status = info->read_memory_func (addr, buffer, 1, info))) {
    goto fail;
  }

  //  Put opcode into the instruction word.
  iword = buffer[0];

  //  Disassemble instructions with length of 1 byte
  if ((iword & ADELIE_LEN_MASK) == ADELIE_F0) {

    opcode = &adelie_form0_opc_info[buffer[0] & 0x3F];
    length = 1;
  }

  //  Disassemble instructions with length of 2 bytes
  if ((iword & ADELIE_LEN_MASK) == ADELIE_F1) {
    
    //  Get the operands
    opcode = &adelie_form1_opc_info[buffer[0] & 0x3F];
    length = 2;

    if ((status = info->read_memory_func (addr, buffer, 1, info))) {
      goto fail;
    }

    switch (opcode->itype)
    {
    case ADELIE_F1_1REG:
      
      break;
    
    case ADELIE_F1_IMM:
      
      break;
    
    default:
      abort();
    }
  }

  //  Disassemble instructions with length of 3 bytes
  if ((iword & ADELIE_LEN_MASK) == ADELIE_F2) {

    opcode = &adelie_form2_opc_info[buffer[0] & 0x3F];
    length = 3;

    if ((status = info->read_memory_func (addr, buffer, 2, info))) {
      goto fail;
    }

    switch (opcode->itype)
    {
    case ADELIE_F2_3REG:
      
      break;
    
    case ADELIE_F2_2REG_IMM:
      
      break;
    
    case ADELIE_F2_1REG_IMM:
      
      break;
    
    case ADELIE_F2_IMM:
      
      break;
    
    default:
      abort();
    }
  }

  //  Disassemble instructions with length of 4 bytes
  if ((iword & ADELIE_LEN_MASK) == ADELIE_F3) {

    opcode = &adelie_form3_opc_info[buffer[0] & 0x3F];
    length = 4;

    if ((status = info->read_memory_func (addr, buffer, 3, info))) {
      goto fail;
    }

    switch (opcode->itype)
    {
    case ADELIE_F3_1REG_IMM:
      
      break;
    
    case ADELIE_F3_IMM:
      
      break;
    
    default:
      abort();
    }
  }

  if (0 == length) {
    abort();
  }

  fpr(stream, "%s", opcode->name);

  return length;

 fail:
  info->memory_error_func (status, addr, info);
  return -1;
}
