/* printtos.c - printtos */

#include<stdio.h>

static unsigned long	*bp;
static unsigned long	*sp;

/*------------------------------------------------------------------------
 *  printtos  --  Prints TOS contents before and after printtos()
 *------------------------------------------------------------------------
 */

void printtos() {
	int  offset = 0;
	kprintf("\nvoid printtos()\n");

	asm("movl %ebp, bp");
	kprintf("Before[0x%08x]: 0x%08x\n", bp+2, *(bp+2));
        kprintf("After[0x%08x]: 0x%08x\n", bp, *bp);

	asm("movl %esp, sp");
	
	int count = (bp - sp - 1) >= 4? 4 : (bp - sp - 1);
	while(offset++ < count)
		kprintf("\telement[0x%08x]: 0x%08x\n",sp + offset, *(sp + offset));  
}

