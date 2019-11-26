    			.global _start
.text
_start:         ldi.m $r1, 0x555
				ldi.m $r2, 0x111+0x222
				; The line below is causing the linker to not output anything.
				; I think that this is a relocation issue.
				ldi.m $r3, _start+0x10
