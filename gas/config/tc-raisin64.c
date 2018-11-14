/* tc-raisin64.c -- Assemble code for raisin64
   Copyright (C) 2009-2018 Free Software Foundation, Inc.

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

/* Based on the moxie code by Anthony Green <green@moxielogic.com>.  */

#include "as.h"
#include "safe-ctype.h"
#include "opcode/raisin64.h"
#include "elf/raisin64.h"

#define BAD_LINE(x) {\
  as_bad(_(#x));\
  ignore_rest_of_line ();\
  return;}
#define EAT_COMMA if(*op_end != ',') BAD_LINE("expecting comma delimited register operands"); op_end++;
#define EAT_SPACES while(ISSPACE(*op_end)) op_end++;

extern const raisin64_opc_info_t raisin64_opc_info[64];

const char comment_chars[]        = "#";
const char line_separator_chars[] = ";";
const char line_comment_chars[]   = "#";

static int pending_reloc;
static struct hash_control *opcode_hash_control;

const pseudo_typeS md_pseudo_table[] =
{
  {0, 0, 0}
};

const char FLT_CHARS[] = "rRsSfFdDxXpP";
const char EXP_CHARS[] = "eE";

static valueT md_chars_to_number (char * buf, int n);

typedef struct machine_op
{
  char *error;
  unsigned long opcode;
  expressionS exp;
  int pcrel;
  int size;
  int reloc_offset;		/* Offset of reloc within insn.  */
  bfd_reloc_code_real_type reloc;
  int HI;
  int LO;
} machine_op_t;

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
  const raisin64_opc_info_t *opcode;
  opcode_hash_control = hash_new ();

  /* Insert names into hash table.  */
  for (count = 0, opcode = raisin64_opc_info; count++ < 64; opcode++)
    hash_insert (opcode_hash_control, opcode->name, (char *) opcode);

  target_big_endian = TARGET_BYTES_BIG_ENDIAN;

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

static int match_name(const char *name, const char* src)
{
  for(int i = 0; name[i]!=NULL; i++)
  {
    if(name[i] != src[i]) return 0;
  }
  return 1;
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

  *ptr += 1;
  s += 1;

  if (match_name("lr", s))
  {
    *ptr += 2;
    return 63;
  }
  else if (match_name("sp", s))
  {
    *ptr += 2;
    return 62;
  }
  else if (match_name("zero", s))
  {
    *ptr += 4;
    return 0;
  }
  else if (s[0] == 'r')
  {
    char hi = s[1] - '0';
    char lo = s[2] - '0';

    if(lo >= 0 && lo <= 9)
    {
      if(hi >= 0 && hi <= 6)
      {
        reg = lo + 10*hi;
        *ptr += 3;
        return reg;
      }
      else
      {
        as_bad (_("illegal register number"));
        ignore_rest_of_line ();
        return -1;
      }
    }
    else if(hi >= 0 && hi <= 9)
    {
      *ptr += 2;
      return hi;
    }
    else
    {
      as_bad (_("illegal register number"));
      ignore_rest_of_line ();
      return -1;
    }
  }
  else
  {
    as_bad (_("illegal register number"));
    ignore_rest_of_line ();
    return -1;
  }

  as_abort(__FILE__,__LINE__,"Broke out of register ident loop");
  return -1;
}

/* This is the guts of the machine-dependent assembler.  STR points to
   a machine dependent instruction.  This function is supposed to emit
   the frags/bytes it assembles to.  */

void
md_assemble (char *str)
{
  char *op_start;
  char *op_end;

  raisin64_opc_info_t *opcode;
  char *p;
  char pend;

  unsigned long long iword = 0;

  int nlen = 0;

  const unsigned long long SIZE_MASK = 0xC000000000000000;
  const int SIZE_SHIFT = 56;
  const unsigned long long _64S_RD_MASK = 0xFC000000000000;
  const int _64S_RD_SHIFT = 50;
  const unsigned long long _64S_RD2_MASK = 0x3F00000000000;
  const int _64S_RD2_SHIFT = 44;
  const unsigned long long _64S_RS1_MASK = 0xFC000000000;
  const int _64S_RS1_SHIFT = 38;
  const unsigned long long _64S_RS2_MASK = 0x3F00000000;
  const int _64S_RS2_SHIFT = 32;
  const unsigned long long _64S_IMM_MASK = 0xFFFFFFFF;
  const int _64S_IMM_SHIFT = 0;

  /* Drop leading whitespace.  */
  while (*str == ' ')
    str++;

  /* Find the op code end.  */
  op_start = str;
  for (op_end = str;
       *op_end && !is_end_of_line[*op_end & 0xff] && *op_end != ' ';
       op_end++)
    nlen++;

  pend = *op_end;
  *op_end = 0;

  if (nlen == 0)
    as_bad (_("can't find opcode "));
  opcode = (raisin64_opc_info_t *) hash_find (opcode_hash_control, op_start);
  *op_end = pend;

  if (opcode == NULL)
    {
      as_bad (_("unknown opcode %s"), op_start);
      return;
    }

  p = frag_more(8);

  switch (opcode->itype)
    {
    case RAISIN64_DS1S2:
      iword = (unsigned long long)opcode->opcode << SIZE_SHIFT;
      iword |= SIZE_MASK;
      while (ISSPACE (*op_end))	op_end++;
      {
        int dest, src1, src2;
        dest = parse_register_operand (&op_end);
        iword |= _64S_RD_MASK & ((unsigned long long)dest << _64S_RD_SHIFT);
        EAT_COMMA;

        EAT_SPACES;
        src1  = parse_register_operand (&op_end);
        iword |= _64S_RS1_MASK & ((unsigned long long)src1 << _64S_RS1_SHIFT);
        EAT_COMMA;

        EAT_SPACES;
        src2  = parse_register_operand (&op_end);
        iword |= _64S_RS2_MASK & ((unsigned long long)src2 << _64S_RS2_SHIFT);

        if (ISPRINT(*op_end)) as_warn (_("extra stuff on line ignored"));
      }
      break;

    case RAISIN64_DD2S1S2:
      iword = (unsigned long long)opcode->opcode << SIZE_SHIFT;
      iword |= SIZE_MASK;
      while (ISSPACE (*op_end))	op_end++;
      {
        int dest, dest2, src1, src2;
        dest = parse_register_operand (&op_end);
        iword |= _64S_RD_MASK & ((unsigned long long)dest << _64S_RD_SHIFT);
        EAT_COMMA;

        EAT_SPACES;
        dest2  = parse_register_operand (&op_end);
        iword |= _64S_RD2_MASK & ((unsigned long long)dest2 << _64S_RD2_SHIFT);
        EAT_COMMA;

        EAT_SPACES;
        src1  = parse_register_operand (&op_end);
        iword |= _64S_RS1_MASK & ((unsigned long long)src1 << _64S_RS1_SHIFT);
        EAT_COMMA;

        EAT_SPACES;
        src2  = parse_register_operand (&op_end);
        iword |= _64S_RS2_MASK & ((unsigned long long)src2 << _64S_RS2_SHIFT);

        if (ISPRINT(*op_end)) as_warn (_("extra stuff on line ignored"));
      }
      break;

    case RAISIN64_NONE:
      iword = (unsigned long long)opcode->opcode << SIZE_SHIFT;
      iword |= SIZE_MASK;
      EAT_SPACES;
      if (ISPRINT(*op_end)) as_warn (_("extra stuff on line ignored"));
      break;

    case RAISIN64_JDS1I: //TODO
      iword = (unsigned long long)opcode->opcode << SIZE_SHIFT;
      iword |= SIZE_MASK;
      while (ISSPACE (*op_end))	op_end++;
      {
        int dest, src1;
        dest = parse_register_operand (&op_end);
        iword |= _64S_RD_MASK & ((unsigned long long)dest << _64S_RD_SHIFT);
        EAT_SPACES;

        EAT_COMMA;
        src1  = parse_register_operand (&op_end);
        iword |= _64S_RS1_MASK & ((unsigned long long)src1 << _64S_RS1_SHIFT);
        EAT_SPACES;

        EAT_COMMA;
        {
          expressionS arg;
          char *where;

          op_end = parse_exp_save_ilp (op_end, &arg);
          where = frag_more (4);
          fix_new_exp (frag_now, //Which fragment
                     (frag_now->fr_literal+4), //Location in current fragment (4 bytes into the 8-byte inst)
                     4,    //4-byte relocation
                     &arg, //Our Expression
                     TRUE, //PC-Relative
                     BFD_RELOC_32); //BFD Relocation type

          EAT_SPACES;
          if (ISPRINT(*op_end)) as_warn (_("extra stuff on line ignored"));
        }
      }
      break;

    case RAISIN64_DS1I:
      iword = (unsigned long long)opcode->opcode << SIZE_SHIFT;
      iword |= SIZE_MASK;
      while (ISSPACE (*op_end))	op_end++;
      {
        int dest, src1;
        dest = parse_register_operand (&op_end);
        iword |= _64S_RD_MASK & ((unsigned long long)dest << _64S_RD_SHIFT);
        EAT_COMMA;

        EAT_SPACES;
        src1  = parse_register_operand (&op_end);
        iword |= _64S_RS1_MASK & ((unsigned long long)src1 << _64S_RS1_SHIFT);
        EAT_COMMA;

        EAT_SPACES;
        {
          expressionS arg;
          op_end = parse_exp_save_ilp (op_end, &arg);

          EAT_SPACES;
          if (ISPRINT(*op_end)) as_warn (_("extra stuff on line ignored"));
        }
      }
      break;

    case RAISIN64_JI:
      iword = (unsigned long long)opcode->opcode << SIZE_SHIFT;
      iword |= SIZE_MASK;
      EAT_SPACES;

      {
        expressionS arg;

        op_end = parse_exp_save_ilp (op_end, &arg);
        fix_new_exp (frag_now, //Which fragment
                    (frag_now->fr_literal+1), //Location in current fragment
                    7, //7-byte relocation
                    &arg, //Expression
                    FALSE, //Not PC-Relative
                    BFD_RELOC_64); //BFD Relocation Type

        EAT_SPACES;
        if (ISPRINT(*op_end)) as_warn (_("extra stuff on line ignored"));

      }
      break;

    case RAISIN64_BAD:
      iword = 0;
      EAT_SPACES;
      if (ISPRINT(*op_end)) as_warn (_("extra stuff on line ignored"));
      break;

    default:
      abort ();
    }

  md_number_to_chars (p, iword, 8);
  dwarf2_emit_insn (8);

  EAT_SPACES;

  if (ISPRINT(*op_end)) as_warn (_("extra stuff on line ignored"));

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

struct option md_longopts[] =
{
  { NULL,          no_argument, NULL, 0}
};

size_t md_longopts_size = sizeof (md_longopts);
const char *md_shortopts = "";

int
md_parse_option (int c ATTRIBUTE_UNUSED, const char *arg ATTRIBUTE_UNUSED)
{
  return 1;
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

  max = min = 0;
  switch (fixP->fx_r_type)
    {
    case BFD_RELOC_64:
      buf[0] = val >> 40;
      buf[1] = val >> 32;
      buf[2] = val >> 24;
      buf[3] = val >> 16;
      buf[4] = val >> 8;
      buf[5] = val >> 0;
      buf += 6;
      break;

    case BFD_RELOC_16:
      buf[0] = val >> 8;
      buf[1] = val >> 0;
      buf += 2;
      break;

    case BFD_RELOC_8:
      *buf++ = val;
      break;

    case BFD_RELOC_RAISIN64_12_PCREL:
      if (!val) break;
      if (val < -4096 || val > 4097) as_bad_where (fixP->fx_file, fixP->fx_line,
                      _("pcrel too far BFD_RELOC_RAISIN64_12"));
      /* 11 bit offset even numbered, so we remove right bit.  */
      val >>= 1;
      newval = md_chars_to_number (buf, 2);
      newval |= val & 0x0fff;
      md_number_to_chars (buf, newval, 2);
      break;

    default:
      abort ();
    }

  if (max != 0 && (val < min || val > max))
    as_bad_where (fixP->fx_file, fixP->fx_line, _("offset out of range"));

  if (fixP->fx_addsy == NULL && fixP->fx_pcrel == 0)
    fixP->fx_done = 1;
}

/* Put number into target byte order.  */

void
md_number_to_chars (char * ptr, valueT use, int nbytes)
{
    number_to_chars_bigendian (ptr, use, nbytes);
}

/* Convert from target byte order to host byte order.  */

static valueT
md_chars_to_number (char * buf, int n)
{
  valueT result = 0;
  unsigned char * where = (unsigned char *) buf;

  while (n--)
  {
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

  switch (fixP->fx_r_type)
    {
    case BFD_RELOC_64:
      code = fixP->fx_r_type;
      break;
    case BFD_RELOC_RAISIN64_12_PCREL:
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

  switch (fixP->fx_r_type)
    {
    case BFD_RELOC_64:
      return addr + 0;
    case BFD_RELOC_RAISIN64_12_PCREL:
      /* Offset is from the beginning of the instruction.  */
      return addr + 0;
    default:
      abort ();
      return addr;
    }
}
