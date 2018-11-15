/* Disassemble raisin64 instructions.
   Copyright (C) 2009-2018 Free Software Foundation, Inc.

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

#include "opcode/raisin64.h"
#include "disassemble.h"

static fprintf_ftype fpr;
static void *stream;

/* Macros to extract operands from the instruction word.  */
#define OP_RD(i) ((i >> 50) & 0x3F)
#define OP_RD2(i) ((i >> 44) & 0x3F)
#define OP_RS1(i) ((i >> 38) & 0x3F)
#define OP_RS2(i) ((i >> 32) & 0x3F)
#define OP_IMM32(i) (i & 0xFFFFFFFF)

#define OP_RS1_32I(i) ((i >> 44) & 0x3F)
#define OP_IMM12(i) ((i >> 32) & 0xFFF)

#define OP_RDRS1_16(i) ((i>>54) & 0x3F)
#define OP_RS2_16R(i) ((i>>48) & 0x3F)
#define OP_IMM6(i) ((i>>48) & 0x3F)

static const char * reg_names[64] =
  { "$zero", "$r1", "$r2", "$r3", "$r4", "$r5", "$r6", "$r7", "$r8", "$r9",
    "$r10", "$r11", "$r12", "$r13", "$r14", "$r15", "$r16", "$r17", "$r18", "$r19",
    "$r20", "$r21", "$r22", "$r23", "$r24", "$r25", "$r26", "$r27", "$r28", "$r29",
    "$r30", "$r31", "$r32", "$r33", "$r34", "$r35", "$r36", "$r37", "$r38", "$r39",
    "$r40", "$r41", "$r42", "$r43", "$r44", "$r45", "$r46", "$r47", "$r48", "$r49",
    "$r50", "$r51", "$r52", "$r53", "$r54", "$r55", "$r56", "$r57", "$r58", "$r59",
    "$r60", "$r61", "$sp", "$lr"
};

int
print_insn_raisin64 (bfd_vma addr, struct disassemble_info * info)
{
  int length; //TODO variable size instructions can override this
  int status;
  stream = info->stream;
  const raisin64_opc_info_t * opcode;
  bfd_byte buffer[8];
  volatile unsigned long long iword;
  fpr = info->fprintf_func;

  if ((status = info->read_memory_func (addr, buffer, 8, info)))
    goto fail;

  iword = bfd_getb64 (buffer);

  //TODO 64-bit only
  if(iword>>62 == 3)
  {
    length = 8;
    opcode = &raisin64_opc_info[(iword >> 56) & 0x3F];
    switch (opcode->itype)
    {
    case RAISIN64_NONE:
      fpr (stream, "%s", opcode->name);
      break;
    case RAISIN64_DS1S2:
      fpr (stream, "%s\t%s, %s, %s", opcode->name, reg_names[OP_RD(iword)], reg_names[OP_RS1(iword)], reg_names[OP_RS2(iword)]);
      break;
    case RAISIN64_S1:
      fpr (stream, "%s\t%s", opcode->name, reg_names[OP_RS1(iword)]);
      break;
    case RAISIN64_DD2S1S2:
      fpr (stream, "%s\t%s, %s, %s, %s", opcode->name, reg_names[OP_RD(iword)], reg_names[OP_RD2(iword)], reg_names[OP_RS1(iword)], reg_names[OP_RS2(iword)]);
      break;
    case RAISIN64_JDS1I: //TODO relative jump back to label?
      fpr (stream, "%s\t%s, %s, 0x%llx", opcode->name, reg_names[OP_RD(iword)], reg_names[OP_RS1(iword)], OP_IMM32(iword)<<1);
      break;
    case RAISIN64_DS1I:
      fpr (stream, "%s\t%s, %s, 0x%llx", opcode->name, reg_names[OP_RD(iword)], reg_names[OP_RS1(iword)], OP_IMM32(iword));
      break;
    case RAISIN64_DI:
      fpr (stream, "%s\t%s, 0x%llx", opcode->name, reg_names[OP_RD(iword)], OP_IMM32(iword));
      break;
    case RAISIN64_JI:
      fpr (stream, "%s\t0x%llx", opcode->name, (iword & 0xFFFFFFFFFFFFFF)<<1);
      break;
    case RAISIN64_DS1S2I: //TODO FPU call
      fpr (stream, "%s\t%s, %s, %s, func:0x%llx", opcode->name, reg_names[OP_RD(iword)], reg_names[OP_RS1(iword)], reg_names[OP_RS2(iword)], OP_IMM32(iword));
      break;
    case RAISIN64_BAD:
      fpr (stream, "bad");
      break;
    default:
      abort();
    }
  }
  else if(iword>>62 == 2)
  {
    length = 4;
    opcode = &raisin64_opc_info[(iword >> 56) & 0x3F];
    switch (opcode->itype)
    {
    case RAISIN64_NONE:
      fpr (stream, "%s", opcode->name);
      break;
    case RAISIN64_DS1S2:
      fpr (stream, "%s\t%s, %s, %s", opcode->name, reg_names[OP_RD(iword)], reg_names[OP_RS1(iword)], reg_names[OP_RS2(iword)]);
      break;
    case RAISIN64_S1:
      fpr (stream, "%s\t%s", opcode->name, reg_names[OP_RS1(iword)]);
      break;
    case RAISIN64_DD2S1S2:
      fpr (stream, "%s\t%s, %s, %s, %s", opcode->name, reg_names[OP_RD(iword)], reg_names[OP_RD2(iword)], reg_names[OP_RS1(iword)], reg_names[OP_RS2(iword)]);
      break;
    case RAISIN64_JDS1I: //TODO relative jump back to label?
      fpr (stream, "%s\t%s, %s, 0x%llx", opcode->name, reg_names[OP_RD(iword)], reg_names[OP_RS1(iword)], OP_IMM32(iword)<<1);
      break;
    case RAISIN64_DS1I:
      fpr (stream, "%s\t%s, %s, 0x%llx", opcode->name, reg_names[OP_RD(iword)], reg_names[OP_RS1_32I(iword)], OP_IMM12(iword));
      break;
    case RAISIN64_DI:
      fpr (stream, "%s\t%s, 0x%llx", opcode->name, reg_names[OP_RD(iword)], OP_IMM12(iword));
      break;
    case RAISIN64_JI:
      fpr (stream, "bad");
      break;
    case RAISIN64_DS1S2I:
      fpr (stream, "bad");
      break;
    case RAISIN64_BAD:
      fpr (stream, "bad");
      break;
    default:
      abort();
    }
  }
  else
  {
    length = 2;
    unsigned int opNum;
    opNum = raisin64_16op_map[(iword >> 60) & 0x7];
    opcode = &raisin64_opc_info[opNum];

    switch (opcode->itype)
    {
    case RAISIN64_NONE:
      fpr (stream, "%s", opcode->name);
      break;
    case RAISIN64_DS1S2:
      fpr (stream, "%s\t%s, %s, %s", opcode->name, reg_names[OP_RDRS1_16(iword)], reg_names[OP_RDRS1_16(iword)], reg_names[OP_RS2_16R(iword)]);
      break;
    case RAISIN64_DS1I:
      fpr (stream, "%s\t%s, %s, 0x%llx", opcode->name, reg_names[OP_RDRS1_16(iword)], reg_names[OP_RDRS1_16(iword)], OP_IMM6(iword));
      break;
    case RAISIN64_S1:
      fpr (stream, "%s\t%s", opcode->name, reg_names[OP_RDRS1_16(iword)]);
      break;
    case RAISIN64_BAD:
      fpr (stream, "bad");
      break;
    default:
      abort();
    }
  }

  return length;

 fail:
  info->memory_error_func (status, addr, info);
  return -1;
}
