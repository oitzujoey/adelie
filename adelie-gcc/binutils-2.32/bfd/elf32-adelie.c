/* Adelie-specific support for 32-bit ELF.
   Copyright (C) 2009-2019 Free Software Foundation, Inc.

   Copied from elf32-moxie.c which is..
   Copyright (C) 1998-2019 Free Software Foundation, Inc.

   Copied from elf32-fr30.c which is..
   Copyright (C) 1998-2019 Free Software Foundation, Inc.

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
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street - Fifth Floor, Boston,
   MA 02110-1301, USA.  */

// ADELIE_TODO

#include "sysdep.h"
#include "bfd.h"
#include "libbfd.h"
#include "elf-bfd.h"
// #include "elf/adelie.h"

#define ELF_ARCH		bfd_arch_adelie
#define ELF_MACHINE_CODE	EM_ADELIE
#define ELF_MAXPAGESIZE		0x1

// #define TARGET_BIG_SYM		adelie_elf32_be_vec
#define TARGET_BIG_SYM		bfd_elf32_adelie_vec
#define TARGET_BIG_NAME		"elf32-adelie"
// #define TARGET_LITTLE_SYM	adelie_elf32_le_vec
// #define TARGET_LITTLE_NAME	"elf32-littleadelie"

// #define elf_info_to_howto_rel			NULL
#define elf_info_to_howto			_bfd_elf_no_info_to_howto
// #define elf_backend_relocate_section		moxie_elf_relocate_section
// #define elf_backend_gc_mark_hook		moxie_elf_gc_mark_hook
// #define elf_backend_check_relocs		moxie_elf_check_relocs

// #define elf_backend_can_gc_sections		1
// #define elf_backend_rela_normal			1

#define bfd_elf32_bfd_reloc_type_lookup		bfd_default_reloc_type_lookup
#define bfd_elf32_bfd_reloc_name_lookup		_bfd_norelocs_bfd_reloc_name_lookup

#include "elf32-target.h"
