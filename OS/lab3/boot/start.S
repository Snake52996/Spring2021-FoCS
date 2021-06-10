NESTED(except_vec3, 0, sp)
	.set noat
	.set noreorder
1:
	mfc0	k1, CP0_CAUSE
	la	k0, exception_handlers
	andi	k1, 0x7c
	addu	k0, k1
	lw	k0, (k0)
	nop
	jr	k0
	nop
END(except_vec3)
.set at
