	.text
.globl zfunction
zfunction:
	push %ebp
	movl %esp, %ebp
	movl 8(%ebp), %eax
	andl $0xffc01fff, %eax
	shl $4, %eax
	leave
	ret

