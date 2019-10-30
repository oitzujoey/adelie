#!/bin/bash

BFDINT='/home/joey/adelie/adelie-gcc/binutils-2.32/bfd/doc/bfdint.texi'
LDINT='/home/joey/adelie/adelie-gcc/binutils-2.32/ld/ldint.texi'
INTERNALS='/home/joey/adelie/adelie-gcc/binutils-2.32/gas/doc/internals.texi'

$EDITOR $BFDINT $LDINT $INTERNALS &!
