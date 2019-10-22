/* BFD support for the Adelie processor.
   Copyright (C) 2009-2019 Free Software Foundation, Inc.

   Copied from cpu-moxie.c which is..
   Copyright (C) 2009-2019 Free Software Foundation, Inc.

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

#include "sysdep.h"
#include "bfd.h"
#include "libbfd.h"


const bfd_arch_info_type bfd_adelie_arch =
  {
    32,		      /* 32 bits in a word.  */
    32,		      /* 32 bits in an address.  */
    8,		      /* 8 bits in a byte.  */
    bfd_arch_adelie,   /* enum bfd_architecture arch.  */
    bfd_mach_adelie,    /* Machine value, used to distinguish between adelie variants.  */
    "adelie",	      /* Arch name.  */
    "adelie",	      /* Printable name.  */
    2,	/* log2 of unsigned int section alignment in bytes */
    TRUE,             /* True if this is the default machine for the architecture.  */
    bfd_default_compatible,
    bfd_default_scan, /* Function to call to determine if two different architectures are compatible.  */
    bfd_arch_default_fill, /* Function to call to determine if a given string matches this architecture.  */ 
    NULL       /*  Next in list. Other "bfd_arch_info"s would normally go here */
  };
