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
        bfd_elf_generic_reloc,      //  special_function
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
        bfd_elf_generic_reloc,      //  special_function
        "R_ADELIE_DIR32",           //  Name
        FALSE,                      //  partial_inplace
        0x7ffff,                    //  src_mask
        0x7ffff,                    //  dst_mask
        FALSE                       //  pcrel_offset
    ),

    /* A 19 bit Immediate relocation.  */
    HOWTO(
        R_ADELIE_IMM19,             /* type.  */
        0,                          /* rightshift.  */
        2,                          /* size (0 = byte, 1 = short, 2 = long).  */
        19,                         /* bitsize.  */
        FALSE,                      /* pc_relative.  */
        0,                          /* bitpos.  */
        complain_overflow_signed,   /* complain_on_overflow.  */
        bfd_elf_generic_reloc,      /* special_function.  */
        "R_ADELIE_IMM19",           /* name.  */
        FALSE,                      /* partial_inplace.  */
        0,                          /* src_mask.  */
        0x0007FFFF,                 /* dst_mask.  */
        FALSE                       /* pcrel_offset.  */
    ),
};

struct adelie_reloc_map {
    bfd_reloc_code_real_type bfd_reloc_val;
    unsigned int adelie_reloc_val;
};

static const struct adelie_reloc_map adelie_reloc_map[] = {
    {BFD_RELOC_NONE,            R_ADELIE_NONE},
    {BFD_RELOC_32,              R_ADELIE_DIR32},
    {BFD_RELOC_ADELIE_19_IMM,   R_ADELIE_IMM19}
};

static reloc_howto_type *adelie_reloc_type_lookup(bfd *abfd ATTRIBUTE_UNUSED, bfd_reloc_code_real_type code) {
    
    unsigned int i;

    for (i = sizeof(adelie_reloc_map) / sizeof(adelie_reloc_map[0]); i--;) {
        if (adelie_reloc_map[i].bfd_reloc_val == code) {
            return &adelie_elf_howto_table[(int) adelie_reloc_map[i].adelie_reloc_val];
        }
    }

    return NULL;
}

static reloc_howto_type *adelie_reloc_name_lookup(bfd *abfd ATTRIBUTE_UNUSED, const char *r_name) {

    unsigned int i;

    for (i = 0; i < sizeof(adelie_elf_howto_table) / sizeof(adelie_elf_howto_table[0]); i++) {
        if (adelie_elf_howto_table[i].name != NULL && strcasecmp(adelie_elf_howto_table[i].name, r_name) == 0) {
            return &adelie_elf_howto_table[i];
        }
    }
    return NULL;
}

/* Set the howto pointer for an ADELIE ELF reloc.  */

static bfd_boolean adelie_info_to_howto_rela(bfd *abfd, arelent *cache_ptr, Elf_Internal_Rela *dst) {
    
    unsigned int r_type;

    r_type = ELF32_R_TYPE (dst->r_info);

    if (r_type >= (unsigned int) R_ADELIE_max) {

        /* xgettext:c-format */
        _bfd_error_handler (_("%pB: unsupported relocation type %#x"), abfd, r_type);
        bfd_set_error (bfd_error_bad_value);

        return FALSE;
    }

    cache_ptr->howto = &adelie_elf_howto_table [r_type];

    return TRUE;
}

/* Perform a single relocation.  By default we use the standard BFD
   routines, but a few relocs, we have to do them ourselves.  */

static bfd_reloc_status_type
adelie_final_link_relocate (reloc_howto_type *howto,
                bfd *input_bfd,
                asection *input_section,
                bfd_byte *contents,
                Elf_Internal_Rela *rel,
                bfd_vma relocation)
{
    bfd_reloc_status_type r = bfd_reloc_ok;

    switch (howto->type) {
        default:
            r = _bfd_final_link_relocate (howto, input_bfd, input_section,
            contents, rel->r_offset,
            relocation, rel->r_addend);
    }

    return r;
}

static bfd_boolean adelie_elf_relocate_section (bfd *output_bfd,
                struct bfd_link_info *info,
                bfd *input_bfd,
                asection *input_section,
                bfd_byte *contents,
                Elf_Internal_Rela *relocs,
                Elf_Internal_Sym *local_syms,
                asection **local_sections)
{
    Elf_Internal_Shdr *symtab_hdr;
    struct elf_link_hash_entry **sym_hashes;
    Elf_Internal_Rela *rel;
    Elf_Internal_Rela *relend;

    symtab_hdr = & elf_tdata (input_bfd)->symtab_hdr;
    sym_hashes = elf_sym_hashes (input_bfd);
    relend     = relocs + input_section->reloc_count;

    for (rel = relocs; rel < relend; rel ++) {

        reloc_howto_type *howto;
        unsigned long r_symndx;
        Elf_Internal_Sym *sym;
        asection *sec;
        struct elf_link_hash_entry *h;
        bfd_vma relocation;
        bfd_reloc_status_type r;
        const char *name;
        int r_type;

        r_type = ELF32_R_TYPE (rel->r_info);
        r_symndx = ELF32_R_SYM (rel->r_info);
        howto  = adelie_elf_howto_table + r_type;
        h      = NULL;
        sym    = NULL;
        sec    = NULL;

        if (r_symndx < symtab_hdr->sh_info) {

            sym = local_syms + r_symndx;
            sec = local_sections [r_symndx];
            relocation = _bfd_elf_rela_local_sym (output_bfd, sym, &sec, rel);

            name = bfd_elf_string_from_elf_section
                (input_bfd, symtab_hdr->sh_link, sym->st_name);
            name = (name == NULL) ? bfd_section_name (sec) : name;

        } else {

            bfd_boolean unresolved_reloc, warned, ignored;

            RELOC_FOR_GLOBAL_SYMBOL (info, input_bfd, input_section, rel,
                            r_symndx, symtab_hdr, sym_hashes,
                            h, sec, relocation,
                            unresolved_reloc, warned, ignored);

            name = h->root.root.string;
        }

        if (sec != NULL && discarded_section (sec))
            RELOC_AGAINST_DISCARDED_SECTION (info, input_bfd, input_section,
                            rel, 1, relend, howto, 0, contents);

        if (bfd_link_relocatable (info))
            continue;

        r = adelie_final_link_relocate (howto, input_bfd, input_section,
                        contents, rel, relocation);

        if (r != bfd_reloc_ok) {

            const char * msg = NULL;

            switch (r) {

                case bfd_reloc_overflow:
                (*info->callbacks->reloc_overflow)
                (info, (h ? &h->root : NULL), name, howto->name,
                (bfd_vma) 0, input_bfd, input_section, rel->r_offset);
                break;

                case bfd_reloc_undefined:
                (*info->callbacks->undefined_symbol)
                (info, name, input_bfd, input_section, rel->r_offset, TRUE);
                break;

                case bfd_reloc_outofrange:
                msg = _("internal error: out of range error");
                break;

                case bfd_reloc_notsupported:
                msg = _("internal error: unsupported relocation error");
                break;

                case bfd_reloc_dangerous:
                msg = _("internal error: dangerous relocation");
                break;

                default:
                msg = _("internal error: unknown error");
                break;
            }

            if (msg)
                (*info->callbacks->warning) (info, msg, name, input_bfd,
                                input_section, rel->r_offset);
        }
    }

    return TRUE;
}

#define ELF_ARCH                        bfd_arch_adelie
#define ELF_MACHINE_CODE                EM_ADELIE
#define ELF_MAXPAGESIZE                 0x1

// #define TARGET_BIG_SYM		adelie_elf32_be_vec
#define TARGET_BIG_SYM                  adelie_elf32_vec
#define TARGET_BIG_NAME                 "elf32-adelie"
// #define TARGET_LITTLE_SYM	adelie_elf32_le_vec
// #define TARGET_LITTLE_NAME	"elf32-littleadelie"

// #define elf_info_to_howto_rel			NULL
#define elf_info_to_howto               adelie_info_to_howto_rela
#define elf_backend_relocate_section    adelie_elf_relocate_section
// #define elf_backend_gc_mark_hook		moxie_elf_gc_mark_hook
// #define elf_backend_check_relocs		moxie_elf_check_relocs

// #define elf_backend_can_gc_sections		1
// #define elf_backend_rela_normal			1

#define bfd_elf32_bfd_reloc_type_lookup adelie_reloc_type_lookup
#define bfd_elf32_bfd_reloc_name_lookup adelie_reloc_name_lookup

#include "elf32-target.h"
