/* raisin64 ELF support for BFD.
   Copyright (C) 2009-2018 Free Software Foundation, Inc.

   This file is part of BFD, the Binary File Descriptor library.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software Foundation,
   Inc., 51 Franklin Street - Fifth Floor, Boston, MA 02110-1301, USA.  */

#ifndef _ELF_RAISIN64_H
#define _ELF_RAISIN64_H

#include "elf/reloc-macros.h"

/* Relocation types.  */
START_RELOC_NUMBERS (elf_raisin64_reloc_type)
  RELOC_NUMBER (R_RAISIN64_NONE, 0)
  RELOC_NUMBER (R_RAISIN64_56, 1)
  RELOC_NUMBER (R_RAISIN64_PCREL32, 2)
  RELOC_NUMBER (R_RAISIN64_PCREL12, 3)
  RELOC_NUMBER (R_RAISIN64_DATA32, 4)
END_RELOC_NUMBERS (R_RAISIN64_max)

#endif /* _ELF_RAISIN64_H */
