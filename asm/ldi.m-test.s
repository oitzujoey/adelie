.data
    			.global foo
foo:            .long 0x123
bar:            .long 0x456
.text
    			.global _start
_start:         ldi.m $r6, 0xffff
				super
test:			ldi.m $r5, test+0x10
				user
