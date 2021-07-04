.data

w:	.asciz	"Hello, this is a test sentence!"
.text
.globl main
main:
	li	t0, 5
	li	t1, 130
	sltu	t2, t0, t1
	xor	t3, t0, t1
	sra	t4, t1, t0
	and	t5, t0, t1
	or	t6, t0, t1
	
	li	a0, 6
	li	a1, 100
	slti	a2, a1, -4
	sltiu	a2, a0, 6
	xori	a3, a0, 3
	srai	a4, a1, 2
	ori	a5, a1, 7
	andi	a5, a1, 15
	
	la	t1, w
	lb	a0, 0(t1)
	lh	a0, 0(t1)
	lbu	a0, 0(t1)
	lhu	a0, 0(t1)
	
	addi	sp, sp, -2
	sh	a0, 1(sp)
	sb	a0, 0(sp)
	
	lui	s0, 0x88888
	
	
	
