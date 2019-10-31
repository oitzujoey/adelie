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
#include "elf/adelie.h"

static bfd_reloc_status_type adelie_elf_reloc (
    bfd *abfd,
    arelent *reloc_entry,
    asymbol *symbol_in,
    void *data,
    asection *input_section,
    bfd *output_bfd,
    char **error_message ATTRIBUTE_UNUSED
) {
    unsigned long insn;
    bfd_vma sym_value;
    enum elf_adelie_reloc_type r_type;
    bfd_vma addr = reloc_entry->address;
    bfd_byte *hit_data = addr + (bfd_byte *) data;

    r_type = (enum elf_adelie_reloc_type) reloc_entry->howto->type;

    if (NULL != output_bfd) {
        reloc_entry->address += input_section->output_offset;
        return bfd_reloc_ok;
    }

    if (NULL != symbol_in && bfd_is_und_section(symbol_in->section)) {
        return bfd_reloc_undefined;
    }

    if (bfd_is_com_section(symbol_in->section)) {
        sym_value = 0;
    } else {
        sym_value = (symbol_in->value + symbol_in->section->output_section->vma + symbol_in->section->output_offset);
    }

    switch (r_type)
    {
    case R_ADELIE_DIR32:
        insn = bfd_get_32(abfd, hit_data);
        insn += sym_value + reloc_entry->addend;
        bfd_put_32(abfd, (bfd_vma) insn, hit_data);
        break;
    
    default:
        abort();
    }

    return bfd_reloc_ok;
}

static reloc_howto_type adelie_elf_howto_table[] = {
    /* No relocation */
    HOWTO(
        R_ADELIE_NONE,              //  Type
        0,                          //  Rightshift
        0,                          //  Size (0 = byte, 1 = short, 2 = long)
        0,                          //  Bitsize
        FALSE,                      //  PC relative
        0,                          //  Bitpos
        complain_overflow_dont,     //  complain_on_overflow
        adelie_elf_reloc,           //  special_function
        "R_ADELIE_NONE",            //  Name
        FALSE,                      //  partial_inplace
        0,                          //  src_mask
        0,                          //  dst_mask
        FALSE                       //  pcrel_offset
    ),

    HOWTO(
        R_ADELIE_DIR32,             //  Type
        0,                          //  Rightshift
        2,                          //  Size (0 = byte, 1 = short, 2 = long)
        19,                         //  Bitsize
        FALSE,                      //  PC relative
        0,                          //  Bitpos
        complain_overflow_bitfield, //  complain_on_overflow
        adelie_elf_reloc,           //  special_function
        "R_ADELIE_DIR32",           //  Name
        FALSE,                      //  partial_inplace
        0x7ffff,                    //  src_mask
        0x7ffff,                    //  dst_mask
        FALSE                       //  pcrel_offset
    )
};

struct elf_reloc_map {
    bfd_reloc_code_real_type bfd_reloc_val;
    unsigned char elf_reloc_val;
};

static const struct elf_reloc_map adelie_reloc_map[] = {
    {BFD_RELOC_NONE,    R_ADELIE_NONE},
    {BFD_RELOC_32,      R_ADELIE_DIR32}
};

static reloc_howto_type *adelie_elf_reloc_type_lookup(bfd *abfd ATTRIBUTE_UNUSED, bfd_reloc_code_real_type code) {
    
    unsigned int i;

    for (i = 0; i < sizeof(adelie_reloc_map) / sizeof(struct elf_reloc_map); i++) {
        if (adelie_reloc_map[i].bfd_reloc_val == code) {
            return &adelie_elf_howto_table[(int) adelie_reloc_map[i].elf_reloc_val];
        }
    }

    return NULL;
}

static reloc_howto_type *adelie_elf_reloc_name_lookup(bfd *abfd ATTRIBUTE_UNUSED, const char *r_name) {

    unsigned int i;

    for (i = 0; i < sizeof(adelie_elf_howto_table) / sizeof(adelie_elf_howto_table[0]); i++) {
        if (adelie_elf_howto_table[i].name != NULL && strcasecmp(adelie_elf_howto_table[i].name, r_name) == 0) {
            return &adelie_elf_howto_table[i];
        }
    }
    return NULL;
}

static bfd_boolean adelie_elf_info_to_howto(bfd *abfd ATTRIBUTE_UNUSED, arelent *cache_ptr, Elf_Internal_Rela *dst) {
    
    unsigned int r;

    r = ELF32_R_TYPE(dst->r_info);

    BFD_ASSERT(r < (unsigned int) R_ADELIE_max);

    cache_ptr->howto = &adelie_elf_howto_table[r];

    return 0;
}

#define ELF_ARCH		bfd_arch_adelie
#define ELF_MACHINE_CODE	EM_ADELIE
#define ELF_MAXPAGESIZE		0x1

// #define TARGET_BIG_SYM		adelie_elf32_be_vec
#define TARGET_BIG_SYM		adelie_elf32_vec
#define TARGET_BIG_NAME		"elf32-adelie"
// #define TARGET_LITTLE_SYM	adelie_elf32_le_vec
// #define TARGET_LITTLE_NAME	"elf32-littleadelie"

// #define elf_info_to_howto_rel			NULL
#define elf_info_to_howto			adelie_elf_info_to_howto
// #define elf_backend_relocate_section		moxie_elf_relocate_section
// #define elf_backend_gc_mark_hook		moxie_elf_gc_mark_hook
// #define elf_backend_check_relocs		moxie_elf_check_relocs

// #define elf_backend_can_gc_sections		1
// #define elf_backend_rela_normal			1

#define bfd_elf32_bfd_reloc_type_lookup		adelie_elf_reloc_type_lookup
#define bfd_elf32_bfd_reloc_name_lookup		adelie_elf_reloc_name_lookup

#include "elf32-target.h"
