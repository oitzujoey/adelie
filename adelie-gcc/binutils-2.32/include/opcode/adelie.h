/* Definitions for decoding the Adelie opcode table.

   Copied from moxie.h which is..
   Copyright (C) 2009-2019 Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street - Fifth Floor, Boston, MA
   02110-1301, USA.  */

/*  
*/

#define ADELIE_LEN_MASK     0b11000000

//  Length of 8
#define ADELIE_F0           0b00000000
// #define ADELIE_F0_NARG      0b00000000  //  No arguments, only option

//  Length of 16
#define ADELIE_F1           0b01000000
#define ADELIE_F1_1REG      0b01000000  //  1 register
#define ADELIE_F1_IMM       0b01100000  //  8-bit immediate

//  Length of 24
#define ADELIE_F2           0b10000000
#define ADELIE_F2_3REG      0b10000000  //  3 registers
#define ADELIE_F2_2REG_IMM  0b10010000  //  2 registers + 6-bit immediate
#define ADELIE_F2_1REG_IMM  0b10100000  //  1 register + 11-bit immediate
#define ADELIE_F2_IMM       0b10110000  //  16-bit immediate

//  Length of 32
#define ADELIE_F3           0b11000000
#define ADELIE_F3_1REG_IMM  0b11000000  //  1 register + 19-bit immediate
#define ADELIE_F3_IMM       0b11100000  //  24-bit immediate

#define ADELIE_BAD          0b11111111  //  Bad instruction

typedef struct adelie_opc_info_t
{
  short         opcode;
  unsigned      itype;
  const char *  name;
} adelie_opc_info_t;

extern const adelie_opc_info_t adelie_form0_opc_info[64];
extern const adelie_opc_info_t adelie_form1_opc_info[64];
extern const adelie_opc_info_t adelie_form2_opc_info[64];
extern const adelie_opc_info_t adelie_form3_opc_info[64];
