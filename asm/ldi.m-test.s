				.global _start
.text
_start:			ldi.m $r1, 0x555
				ldi.m $r3, 0x111+0x222
				ldi.m $r4, _start+0x10
				ldi.m $r01, 0
				ldi.m $r11, 0
				ldi.m $r21, 0
				ldi.m $r31, 0
