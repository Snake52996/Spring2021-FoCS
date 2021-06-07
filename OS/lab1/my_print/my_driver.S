#include <asm/regdef.h>
#include <asm/cp0regdef.h>
#include <asm/asm.h>
.text
LEAF(_my_putchar)
	.set mips2
	.set reorder
	lui t0,0xB000
	sb a0,0(t0)
	jr ra
	nop
END(_my_putchar)

LEAF(_my_exit)
	.set mips2
	.set reorder
	lui t0,0xB000
	ori t0,t0,0x0010
	lb t0,0(t0)
	nop
	nop
	nop
	nop
	nop
	jr ra
	nop
END(_my_exit)
