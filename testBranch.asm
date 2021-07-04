.data

.text
.globl main
main:
	li	s1, 13
	li	s2, 28
	blt	s1, s2, L1
	bltu	s1, s2, L1
	bgeu	s1, s2, L1
	bgt	s1, s2, L1
L1:

