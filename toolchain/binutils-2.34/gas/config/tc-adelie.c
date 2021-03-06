/* tc-adelie.c -- Assemble code for Adelie

   Copied from tc-moxie.c which is..
   Copyright (C) 2009-2019 Free Software Foundation, Inc.

   This file is part of GAS, the GNU Assembler.

   GAS is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3, or (at your option)
   any later version.

   GAS is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with GAS; see the file COPYING.  If not, write to
   the Free Software Foundation, 51 Franklin Street - Fifth Floor,
   Boston, MA 02110-1301, USA.  */

/* Contributed by Anthony Green <green@moxielogic.com>.  */

#include "as.h"
#include "safe-ctype.h"
#include "opcode/adelie.h"
// #include "elf/adelie.h"

extern const adelie_opc_info_t adelie_opc_info[256];

/* This array holds the chars that only start a comment at the beginning of
   a line.  If the line seems to have the form '# 123 filename'
   .line and .file directives will appear in the pre-processed output.	*/
/* Note that input_file.c hand checks for '#' at the beginning of the
   first line of the input file.  This is because the compiler outputs
   #NO_APP at the beginning of its output.  */
/* Also note that comments like this one will always work.  */
const char comment_chars[]        = "#";
const char line_separator_chars[] = ";";
const char line_comment_chars[]   = "#";

static int pending_reloc;
static struct hash_control *opcode_hash_control;

/* This table describes all the machine specific pseudo-ops the assembler
   has to support.  The fields are:
     pseudo-op name without dot
     function to call to execute this pseudo-op
     Integer arg to pass to the function.  */

const pseudo_typeS md_pseudo_table[] =
{
  {0, 0, 0}
};

/* Chars that mean this number is a floating point constant.  */
/* As in 0f12.456  */
/* or  0d1.2345e12  */

const char FLT_CHARS[] = "rRsSfFdDxXpP";

/* Chars that can be used to separate mant
   from exp in floating point numbers.	*/
const char EXP_CHARS[] = "eE";

static valueT md_chars_to_number (char * buf, int n);

/* Byte order.  */
// extern int target_big_endian;

/* We handle all bad expressions here, so that we can report the faulty
   instruction in the error message.  */
void
md_operand (expressionS *op __attribute__((unused)))
{
  /* Empty for now. */
}

/* This function is called once, at assembler startup time.  It sets
   up the hash table with all the opcodes in it, and also initializes
   some aliases for compatibility with other assemblers.  */

void
md_begin (void)
{
  int count;
  const adelie_opc_info_t *opcode;
  opcode_hash_control = hash_new ();

  /* Insert names into hash table.  */
  for (count = 0, opcode = adelie_form0_opc_info; count++ < 64; opcode++)
    hash_insert (opcode_hash_control, opcode->name, (char *) opcode);

  for (count = 0, opcode = adelie_form1_opc_info; count++ < 64; opcode++)
    hash_insert (opcode_hash_control, opcode->name, (char *) opcode);

  for (count = 0, opcode = adelie_form2_opc_info; count++ < 64; opcode++)
    hash_insert (opcode_hash_control, opcode->name, (char *) opcode);

  for (count = 0, opcode = adelie_form3_opc_info; count++ < 64; opcode++)
    hash_insert (opcode_hash_control, opcode->name, (char *) opcode);

//   target_big_endian = TARGET_BYTES_BIG_ENDIAN;

  bfd_set_arch_mach (stdoutput, TARGET_ARCH, 0);
}

/* Parse an expression and then restore the input line pointer.  */

static char *
parse_exp_save_ilp (char *s, expressionS *op)
{
  char *save = input_line_pointer;

  input_line_pointer = s;
  expression (op);
  s = input_line_pointer;
  input_line_pointer = save;
  return s;
}

static int
parse_register_operand (char **ptr)
{
  int reg;
  char *s = *ptr;

  if (*s != '$')
  {
    as_bad (_("expecting register"));
    ignore_rest_of_line ();
    return -1;
  }
  // if (s[1] == 'f' && s[2] == 'p')
  // {
  //   *ptr += 3;
  //   return 0;
  // }
  // if (s[1] == 's' && s[2] == 'p')
  // {
  //   *ptr += 3;
  //   return 1;
  // }
  if (s[1] == 'r')
  {
    reg = s[2] - '0';
    if ((reg < 0) || (reg > 9))
    {
      as_bad (_("illegal register number"));
      ignore_rest_of_line ();
      return -1;
    }
    if (reg == 3)
    {
      int r2 = s[3] - '0';
      if ((r2 >= 0) && (r2 <= 1))
      {
        reg = 30 + r2;
        *ptr += 1;
      }
    }
    if ((0 <= reg) && (reg <= 2))
    {
      int r2 = s[3] - '0';
      if ((r2 >= 0) && (r2 <= 9))
      {
        reg = reg*10 + r2;
        *ptr += 1;
      }
    }
  }
  else
  {
    as_bad (_("illegal register number"));
    ignore_rest_of_line ();
    return -1;
  }

  *ptr += 3;

  return reg;
}

/* This is the guts of the machine-dependent assembler.  STR points to
   a machine dependent instruction.  This function is supposed to emit
   the frags/bytes it assembles to.  */

void
md_assemble (char *str)
{
  char *op_start;
  char *op_end;

  adelie_opc_info_t *opcode;
  char pend;
  int length = 0;
  unsigned long iword = 0;

  int nlen = 0;

  /* Drop leading whitespace.  */
  while (*str == ' ')
    str++;

  /* Find the op code end.  */
  op_start = str;
  for (op_end = str; *op_end && !is_end_of_line[*op_end & 0xff] && *op_end != ' '; op_end++)
    nlen++;

  pend = *op_end;
  *op_end = 0;

  if (nlen == 0)
    as_bad (_("can't find opcode "));
  
  opcode = (adelie_opc_info_t *) hash_find (opcode_hash_control, op_start);
  *op_end = pend;

  if (opcode == NULL)
  {
    as_bad (_("unknown opcode %s"), op_start);
    return;
  }

  //  Create the proper number of frags for the length.
  switch (opcode->itype & ADELIE_LEN_MASK)
  {
  case ADELIE_F0:
    length = 1;
    break;
  
  case ADELIE_F1:
    length = 2;
    break;
  
  case ADELIE_F2:
    length = 3;
    break;
  
  case ADELIE_F3:
    length = 4;
    break;
  
  default:
    abort();
  }

  //  Put opcode into the instruction word.
  iword = (opcode->opcode | (opcode->itype & ADELIE_LEN_MASK)) << (8*(length-1));

  // p = frag_more(1);


  expressionS arg;
  char *where = NULL;

  switch (opcode->itype)
  {
  case ADELIE_F0:

    iword = opcode->opcode;

    while (ISSPACE(*op_end)) {
      op_end++;
    }
    if (*op_end != 0) {
      as_warn("Extra stuff on line ignored.");
    }
    break;
  
  case ADELIE_F1_1REG:
    break;
  
  case ADELIE_F1_IMM:
    break;
  
  case ADELIE_F2_3REG:

    // if ((*op_end != '$') || (*(op_end+1) != 'r')) {
    //   as_bad("Expecting register.");
    //   ignore_rest_of_line();
    //   return;
    // }

    while (ISSPACE(*op_end))
      op_end++;
    {
      int r0, r1, r2;
      
      r0 = parse_register_operand(&op_end);
      if (*op_end != ',')
        as_bad("Expecting comma delimited operands");
      op_end++;

      r1 = parse_register_operand(&op_end);
      if (*op_end != ',')
        as_bad("Expecting comma delimited operands");
      op_end++;

      r2 = parse_register_operand(&op_end);

      while (ISSPACE(*op_end))
        op_end++;

      if (*op_end != 0) {
        as_warn("Extra stuff on line ignored");
      }

      iword += (r0<<11) + (r1<<6) + (r2<<1);
    }

    where = frag_more(3);
    
    break;
  
  case ADELIE_F2_2REG_IMM:
    break;
  
  case ADELIE_F2_1REG_IMM:
    break;
  
  case ADELIE_F2_IMM:
    break;
  
  case ADELIE_F3_1REG_IMM:

    while (ISSPACE(*op_end)) {
      op_end++;
    }

    while (ISSPACE(*op_end))
      op_end++;
    {
      int r0;
      
      r0 = parse_register_operand(&op_end);
      if (*op_end != ',')
        as_bad("Expecting comma delimited operands");
      op_end++;

      iword += r0 << 19;
    }

    while (ISSPACE(*op_end)) {
      op_end++;
    }

    op_end = parse_exp_save_ilp(op_end, &arg);
    where = frag_more(4);
    
    fix_new_exp(
      frag_now,
      (where - frag_now->fr_literal),
      4,
      &arg,
      0,
      BFD_RELOC_ADELIE_19_IMM
    );

    break;
  
  case ADELIE_F3_IMM:
    break;
  
  default:
    abort();
  }

  printf("iword: %lX\n", iword);

  number_to_chars_bigendian (where, iword, length);

//   dwarf2_emit_insn (2);

  while (ISSPACE (*op_end))
    op_end++;

  if (*op_end != 0)
    as_warn (_("extra stuff on line ignored"));

  if (pending_reloc)
    as_bad (_("Something forgot to clean up\n"));
}

/* Turn a string in input_line_pointer into a floating point constant
   of type type, and store the appropriate bytes in *LITP.  The number
   of LITTLENUMS emitted is stored in *SIZEP .  An error message is
   returned, or NULL on OK.  */

const char *
md_atof (int type, char *litP, int *sizeP)
{
  int prec;
  LITTLENUM_TYPE words[4];
  char *t;
  int i;

  switch (type)
    {
    case 'f':
      prec = 2;
      break;

    case 'd':
      prec = 4;
      break;

    default:
      *sizeP = 0;
      return _("bad call to md_atof");
    }

  t = atof_ieee (input_line_pointer, type, words);
  if (t)
    input_line_pointer = t;

  *sizeP = prec * 2;

  for (i = prec - 1; i >= 0; i--)
    {
      md_number_to_chars (litP, (valueT) words[i], 2);
      litP += 2;
    }

  return NULL;
}

enum options
{
  OPTION_EB = OPTION_MD_BASE,
  OPTION_EL,
};

struct option md_longopts[] =
{
  { NULL,          no_argument, NULL, 0}
};

size_t md_longopts_size = sizeof (md_longopts);

const char *md_shortopts = "";

int
md_parse_option (int c ATTRIBUTE_UNUSED, const char *arg ATTRIBUTE_UNUSED)
{
  return 0;
}

void
md_show_usage (FILE *stream ATTRIBUTE_UNUSED)
{
}

/* Apply a fixup to the object file.  */

void
md_apply_fix (fixS *fixP ATTRIBUTE_UNUSED,
        valueT * valP ATTRIBUTE_UNUSED, segT seg ATTRIBUTE_UNUSED)
{
  char *buf = fixP->fx_where + fixP->fx_frag->fr_literal;
  long val = *valP;
  long newval;
  long max, min;
  // int shift;

  max = min = 0;
  switch (fixP->fx_r_type)
    {
    case BFD_RELOC_ADELIE_19_IMM:
      if (val >= (1<<19))
        as_bad_where (fixP->fx_file, fixP->fx_line, _("imm too large BFD_RELOC_ADELIE_19"));
      
      newval = md_chars_to_number(buf, 4);
      newval += val & 0x0003ffff;
      md_number_to_chars(buf, newval, 4);
      
      break;

    default:
      abort ();
    }

  if (max != 0 && (val < min || val > max))
    as_bad_where (fixP->fx_file, fixP->fx_line, _("Offset out of range"));

  if (fixP->fx_addsy == NULL && fixP->fx_pcrel == 0)
    fixP->fx_done = 1;
}

/* Put number into target byte order.  */

void
md_number_to_chars (char * ptr, valueT use, int nbytes) {

  number_to_chars_bigendian (ptr, use, nbytes);
}

/* Convert from target byte order to host byte order.  */

static valueT md_chars_to_number (char * buf, int n) {

  valueT result = 0;
  unsigned char * where = (unsigned char *) buf;

  while (n--) {
    result <<= 8;
    result |= (*where++ & 255);
  }
  return result;
}

/* Generate a machine-dependent relocation.  */
arelent *
tc_gen_reloc (asection *section ATTRIBUTE_UNUSED, fixS *fixP)
{
  arelent *relP;
  bfd_reloc_code_real_type code;

  switch (fixP->fx_r_type) {

    case BFD_RELOC_ADELIE_19_IMM:
      code = fixP->fx_r_type;
      break;

    default:
      as_bad_where (fixP->fx_file, fixP->fx_line,
       _("Semantics error.  This type of operand can not be relocated, it must be an assembly-time constant"));
      return 0;
  }

  relP = XNEW (arelent);
  relP->sym_ptr_ptr = XNEW (asymbol *);
  *relP->sym_ptr_ptr = symbol_get_bfdsym (fixP->fx_addsy);
  relP->address = fixP->fx_frag->fr_address + fixP->fx_where;

  relP->addend = fixP->fx_offset;

  /* This is the standard place for KLUDGEs to work around bugs in
     bfd_install_relocation (first such note in the documentation
     appears with binutils-2.8).

     That function bfd_install_relocation does the wrong thing with
     putting stuff into the addend of a reloc (it should stay out) for a
     weak symbol.  The really bad thing is that it adds the
     "segment-relative offset" of the symbol into the reloc.  In this
     case, the reloc should instead be relative to the symbol with no
     other offset than the assembly code shows; and since the symbol is
     weak, any local definition should be ignored until link time (or
     thereafter).
     To wit:  weaksym+42  should be weaksym+42 in the reloc,
     not weaksym+(offset_from_segment_of_local_weaksym_definition)

     To "work around" this, we subtract the segment-relative offset of
     "known" weak symbols.  This evens out the extra offset.

     That happens for a.out but not for ELF, since for ELF,
     bfd_install_relocation uses the "special function" field of the
     howto, and does not execute the code that needs to be undone.  */

  if (OUTPUT_FLAVOR == bfd_target_aout_flavour
      && fixP->fx_addsy && S_IS_WEAK (fixP->fx_addsy)
      && ! bfd_is_und_section (S_GET_SEGMENT (fixP->fx_addsy)))
    {
      relP->addend -= S_GET_VALUE (fixP->fx_addsy);
    }

  relP->howto = bfd_reloc_type_lookup (stdoutput, code);
  if (! relP->howto)
    {
      const char *name;

      name = S_GET_NAME (fixP->fx_addsy);
      if (name == NULL)
      name = _("<unknown>");
        as_fatal (_("Cannot generate relocation type for symbol %s, code %s"),
            name, bfd_get_reloc_code_name (code));
    }

  return relP;
}

/* Decide from what point a pc-relative relocation is relative to,
   relative to the pc-relative fixup.  Er, relatively speaking.  */
long
md_pcrel_from (fixS *fixP)
{
    valueT addr = fixP->fx_where + fixP->fx_frag->fr_address;
    
    fprintf(stderr, "md_pcrel_from 0x%d\n", fixP->fx_r_type);

  switch (fixP->fx_r_type)
    {
    case BFD_RELOC_ADELIE_19_IMM:
      return addr + 4;
    default:
      abort ();
      return addr;
    }
}
