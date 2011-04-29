.text

main:
	li   $4, 32
	subi $5, $4, 1
	addi $3, $4, 2
	la   $6, seg
	b exit
	subi $5, $4, 1
	addi $3, $4, 2
	lb   $7, 1($6)
	nop
	nop

exit:
	li $2, 10
	syscall

.data

seg:
	.asciiz "atest\n"