/* Definitions for decoding the raisin64 opcode table.
   Copyright (C) 2009-2018 Free Software Foundation, Inc.
   Based on moxie by Anthony Green (green@moxielogic.com).

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

/* 64S type instructions use different fields in the instruction and almost
   all are available in their respective 32R and 32I format as well:

    Some have no arguments                          (RAISIN64_64S_32R_NONE)
    Many use Rd, Rs1 and Rs2                        (RAISIN64_64S_32R_DS1S2)
    Many use Rd, Rs1, and the immediate field       (RAISIN64_64S_32I_DS1I)
    Some use Rs1                                    (RAISIN64_64S_32R_S1)
    Some use Rd1 and the immediate field            (RAISIN64_64S_DI)
    Some use Rd, Rd2, Rs1, and Rs2                  (RAISIN64_64S_32R_DD2S1S2)

  64J type instructions are uniform in their field use:

    Used for JALI and JI                            (RAISIN64_64J)  */

//16-bit fixed-size forms

//32-bit fixed-size forms

//64-bit fixed-size forms
#define RAISIN64_JI       0x210
#define RAISIN64_DS1S2I   0x200

//Automatically sized forms
#define RAISIN64_NONE     0x300
#define RAISIN64_DS1S2    0x301
#define RAISIN64_S1       0x302
#define RAISIN64_DD2S1S2  0x303
#define RAISIN64_DS1I     0x304
#define RAISIN64_JDS1I    0x305
#define RAISIN64_DI       0x306

/*
  #define RAISIN64_64S_32R_NONE     0x100
  #define RAISIN64_64S_32R_DS1S2    0x101
  #define RAISIN64_64S_32R_S1       0x102
  #define RAISIN64_64S_32R_DD2S1S2  0x103
  #define RAISIN64_64S_32I_DS1I     0x110
  #define RAISIN64_64S_DI           0x200
  #define RAISIN64_64J              0x210
*/

#define RAISIN64_BAD      0x400

//It is not necessary to have a field denoting the availability of a 64-bit
//format as all instructions have a 64-bit representation.
typedef struct raisin64_opc_info_t
{
  unsigned char opcode;
  unsigned int  itype;
  int           is16;
  unsigned char opcode_16;
  int           is32;
  const char*   name;
} raisin64_opc_info_t;

extern const raisin64_opc_info_t raisin64_opc_info[64];
