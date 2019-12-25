				.global _start
.text
_start:			ldi.m $r1, 0x555
				ldi.m $r3, 0x111+0x222
				# The line below is causing an issue where the 0x10 is inserted into the instruction, but _start is not added as it should.
				ldi.m $r4, _start+0x10
