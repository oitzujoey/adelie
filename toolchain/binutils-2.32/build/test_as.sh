#!/bin/bash

echo "--> AS <--"
gas/as-new -o add-test.o ../../../asm/add-test.s
echo ""
echo "--> LD <--"
ld/ld-new -o add-test add-test.o
echo ""
echo "--> OBJDUMP: -r -d *.o <--"
binutils/objdump -r -d add-test.o
echo ""
echo "--> OBJDUMP: -r -d * <--"
binutils/objdump -r -d add-test
