#!/bin/bash

echo "--> AS <--"
gas/as-new -o ldi.m-test.o ~/adelie/asm/ldi.m-test.s
echo ""
echo "--> LD <--"
ld/ld-new -o ldi.m-test ldi.m-test.o
echo ""
echo "--> OBJDUMP: -r -d *.o <--"
binutils/objdump -r -d ldi.m-test.o
echo ""
echo "--> OBJDUMP: -r -d * <--"
binutils/objdump -r -d ldi.m-test
