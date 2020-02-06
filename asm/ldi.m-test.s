				.global _start
.text
_start:			ldi.m $r1, 0x555
				ldi.m $r3, 0x111+0x222
				# LD is getting an error: 
				#	ld/ld-new: ldi.m-test.o: in function `_start':
				#	(.text+0x9): warning: internal error: out of range error
				ldi.m $r4, _start+0x10
				ldi.m $r4, 0
