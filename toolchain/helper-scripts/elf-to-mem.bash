#!/bin/bash

../binutils-2.34/build/binutils/objcopy -O binary $1 /tmp/elf-to-mem.temp

hexdump -ve '1/1 "%.2x\n"' /tmp/elf-to-mem.temp > $2

rm /tmp/elf-to-mem.temp
