/* printprocstks.c - printprocstks */

#include <conf.h>
#include <kernel.h>
#include <stdio.h>
#include <proc.h>

static unsigned long	*sp;

/*------------------------------------------------------------------------
 *   printprocstks  --  Prints about processes with priorities > priority
 *------------------------------------------------------------------------
 */

void printprocstks (int priority){
	int p;
	kprintf("\nvoid printprocstks(int priority)\n");
	
	for(p = 0; p < NPROC; p++){
		struct pentry *curr = &proctab[p];
		if(curr->pprio > priority){
			kprintf("Process [%s]\n", curr->pname);
			kprintf("\tpid: %d\n",p);		
			kprintf("\tpriority: %d\n", curr->pprio);
                        kprintf("\tbase: 0x%08x\n", curr->pbase);
                        kprintf("\tlimit: 0x%08x\n", curr->plimit);
                        kprintf("\tlen: %d\n", curr->pstklen);
                        if(curr->pstate != PRCURR)
				sp = curr->pesp;
			else
				asm("movl %esp, sp");
			kprintf("\tpointer: 0x %08x\n",sp);
		}
	}
}
