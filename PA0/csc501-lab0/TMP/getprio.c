/* getprio.c - getprio */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <stdio.h>

/*------------------------------------------------------------------------
 * getprio -- return the scheduling priority of a given process
 *------------------------------------------------------------------------
 */

extern int currpid;
extern int inblock;
extern unsigned long ctr1000;

SYSCALL getprio(int pid)
{
	STATWORD ps;    
	struct	pentry	*pptr;

        int id = 3;
        struct pentry *curr = &proctab[currpid];
        unsigned long start, end;
        if(inblock) {
                curr->sys_count[id]++;
                curr->isstarted = 1;
                start = ctr1000;
        }
	disable(ps);
	if (isbadpid(pid) || (pptr = &proctab[pid])->pstate == PRFREE) {
		restore(ps);
                if(inblock) {
                        end = ctr1000;
                        curr->sys_runtime[id] += end - start;
                }
		return(SYSERR);
	}
	restore(ps);

        if(inblock) {
                end = ctr1000;
                curr->sys_runtime[id] += end - start;
        }

	return(pptr->pprio);
}
