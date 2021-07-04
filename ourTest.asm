.data
FirstNumMessage:	.asciz	"Please enter the first number: "
SecondNumMessage:	.asciz	"Please enter the second number: "
ResultMessage:		.asciz	"The Result: "


.text
.globl	main
main:
	call getInput
	call Loop
	call FixStack
	call Exit

getInput:
	la	a0, FirstNumMessage
	li	a7, 4
	ecall
	li	a7, 5
	ecall
	mv	a2, a0		#a2 = n  (first number)
	
	la	a0, SecondNumMessage
	li	a7, 4
	ecall
	li	a7, 5
	ecall
	mv	a3, a0		#a3 = m (second number)
	
	### handling "ans" and "count" ###
	add	sp, zero, zero
	
	addi	sp, sp, -8
	sw	s0, 4(sp)
	sw	s1, 0(sp)
	add	s0, zero, zero	# s0 = ans = 0
	add	s1, zero, zero	# s1 = count = 0
	ret
	
Loop:
	beq	a3, zero, PrintResult	# if(m == 0) exit
	andi	t0, a3, 0x001		#calculating m % 2
	beq	t0, zero, ContinueLoop
	sll	t1, a2, s1		# n << count
	add	s0, s0, t1		# ans += n << count

ContinueLoop:
	addi	s1, s1, 1	#count++
	srli	a3, a3, 1	# m /= 2, (we did not use srai because the values are unsigned)
	j	Loop

PrintResult:
	la	a0, ResultMessage
	li	a7, 4
	ecall
	mv	a0, s0
	li	a7, 1
	ecall
	ret

FixStack:
	lw	s1, 0(sp)
	addi	sp, sp, 4
	lw	s0, 0(sp)
	addi	sp, sp, 4
	ret
Exit:
	li	a7, 10
	ecall
	
