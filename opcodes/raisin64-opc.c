/* raisin64-opc.c -- Definitions for Raisin64 opcodes.
   Copyright (C) 2009-2018 Free Software Foundation, Inc.
   Based on the moxie port by Anthony Green (green@moxielogic.com).

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
   along with this file; see the file COPYING.  If not, write to the
   Free Software Foundation, 51 Franklin Street - Fifth Floor, Boston,
   MA 02110-1301, USA.  */

#include "sysdep.h"
#include "opcode/raisin64.h"

const int NO16 = 0, IS16 = 1;
const int NO32 = 0, IS32 = 1;

//Base opcode without leading size field.
//Again, all instructions are available in 64-bit format, most are available in
//32-bit format (the opcode is the same, only the size prefix changes), and a
//lookup table is used for the few available in 16-bit format.
const raisin64_opc_info_t raisin64_opc_info[64] =
  {
    { 0x00, RAISIN64_DS1S2,   IS16, 0, IS32, "add" },
    { 0x01, RAISIN64_DS1S2,   IS16, 1, IS32, "sub" },
    { 0x02, RAISIN64_BAD,     NO16, 0, IS32, "bad" },
    { 0x03, RAISIN64_BAD,     NO16, 0, IS32, "bad" },
    { 0x04, RAISIN64_DS1S2,   NO16, 0, IS32, "slt" },
    { 0x05, RAISIN64_DS1S2,   NO16, 0, IS32, "sltu" },
    { 0x06, RAISIN64_DS1S2,   NO16, 0, IS32, "sgt" },
    { 0x07, RAISIN64_DS1S2,   NO16, 0, IS32, "sgtu" },
    { 0x08, RAISIN64_DS1S2,   NO16, 0, IS32, "sll" },
    { 0x09, RAISIN64_DS1S2,   NO16, 0, IS32, "sra" },
    { 0x0a, RAISIN64_DS1S2,   NO16, 0, IS32, "srl" },
    { 0x0b, RAISIN64_BAD,     NO16, 0, IS32, "bad" },
    { 0x0c, RAISIN64_DS1S2,   NO16, 0, IS32, "and" },
    { 0x0d, RAISIN64_DS1S2,   NO16, 0, IS32, "nor" },
    { 0x0e, RAISIN64_DS1S2,   NO16, 0, IS32, "or" },
    { 0x0f, RAISIN64_DS1S2,   NO16, 0, IS32, "xor" },
    { 0x10, RAISIN64_DD2S1S2, NO16, 0, IS32, "mul" },
    { 0x11, RAISIN64_DD2S1S2, NO16, 0, IS32, "mulu" },
    { 0x12, RAISIN64_DD2S1S2, NO16, 0, IS32, "div" },
    { 0x13, RAISIN64_DD2S1S2, NO16, 0, IS32, "divu" },
    { 0x14, RAISIN64_BAD,     NO16, 0, IS32, "bad" },
    { 0x15, RAISIN64_BAD,     NO16, 0, IS32, "bad" },
    { 0x16, RAISIN64_BAD,     NO16, 0, IS32, "bad" },
    { 0x17, RAISIN64_BAD,     NO16, 0, IS32, "bad" },
    { 0x18, RAISIN64_BAD,     NO16, 0, IS32, "bad" },
    { 0x19, RAISIN64_BAD,     NO16, 0, IS32, "bad" },
    { 0x1a, RAISIN64_BAD,     NO16, 0, IS32, "bad" },
    { 0x1b, RAISIN64_BAD,     NO16, 0, IS32, "bad" },
    { 0x1c, RAISIN64_NONE,    IS16, 4, IS32, "syscall" },
    { 0x1d, RAISIN64_DS1S2I,  NO16, 0, NO32, "f*" },
    { 0x1e, RAISIN64_S1,      IS16, 6, IS32, "jal" },
    { 0x1f, RAISIN64_S1,      IS16, 5, IS32, "j" },
    { 0x20, RAISIN64_DS1I,    IS16, 2, IS32, "addi" },
    { 0x21, RAISIN64_DS1I,    IS16, 3, IS32, "subi" },
    { 0x22, RAISIN64_BAD,     NO16, 0, IS32, "bad" },
    { 0x23, RAISIN64_BAD,     NO16, 0, IS32, "bad" },
    { 0x24, RAISIN64_DS1I,    NO16, 0, IS32, "slti" },
    { 0x25, RAISIN64_DS1I,    NO16, 0, IS32, "sltiu" },
    { 0x26, RAISIN64_DS1I,    NO16, 0, IS32, "sgti" },
    { 0x27, RAISIN64_DS1I,    NO16, 0, IS32, "sgtiu" },
    { 0x28, RAISIN64_DS1I,    NO16, 0, IS32, "slli" },
    { 0x29, RAISIN64_DS1I,    NO16, 0, IS32, "srai" },
    { 0x2a, RAISIN64_DS1I,    NO16, 0, IS32, "srli" },
    { 0x2b, RAISIN64_BAD,     NO16, 0, IS32, "bad" },
    { 0x2c, RAISIN64_DS1I,    NO16, 0, IS32, "andi" },
    { 0x2d, RAISIN64_DS1I,    NO16, 0, IS32, "nori" },
    { 0x2e, RAISIN64_DS1I,    NO16, 0, IS32, "ori" },
    { 0x2f, RAISIN64_DS1I,    NO16, 0, IS32, "xori" },
    { 0x30, RAISIN64_DS1I,    NO16, 0, IS32, "lw" },
    { 0x31, RAISIN64_DS1I,    NO16, 0, IS32, "l32" },
    { 0x32, RAISIN64_DS1I,    NO16, 0, IS32, "l16" },
    { 0x33, RAISIN64_DS1I,    NO16, 0, IS32, "l8" },
    { 0x34, RAISIN64_DI,      NO16, 0, NO32, "lui" },
    { 0x35, RAISIN64_DS1I,    NO16, 0, IS32, "l32s" },
    { 0x36, RAISIN64_DS1I,    NO16, 0, IS32, "l16s" },
    { 0x37, RAISIN64_DS1I,    NO16, 0, IS32, "l8s" },
    { 0x38, RAISIN64_DS1I,    NO16, 0, IS32, "sw" },
    { 0x39, RAISIN64_DS1I,    NO16, 0, IS32, "s32" },
    { 0x3a, RAISIN64_DS1I,    NO16, 0, IS32, "s16" },
    { 0x3b, RAISIN64_DS1I,    NO16, 0, IS32, "s8" },
    { 0x3c, RAISIN64_JDS1I,   NO16, 0, IS32, "beq" },
    { 0x3d, RAISIN64_JDS1I,   NO16, 0, IS32, "beqal" },
    { 0x3e, RAISIN64_JI,      NO16, 0, NO32, "jali" },
    { 0x3f, RAISIN64_JI,      NO16, 0, NO32, "ji" }
};
