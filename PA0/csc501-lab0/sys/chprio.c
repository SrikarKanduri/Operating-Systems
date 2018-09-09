/* chprio.c - chprio */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <stdio.h>

/*------------------------------------------------------------------------
 * chprio  --  change the scheduling priority of a process
 *------------------------------------------------------------------------
 */

extern int currpid;
extern int inblock;
extern unsigned long ctr1000;

SYSCALL chprio(int pid, int newprio)
{
	STATWORD ps;    
	struct	pentry	*pptr;

        int id = 1;
        struct pentry *curr = &proctab[currpid];
	unsigned long start, end;
        if(inblock) {
                curr->sys_count[id]++;
                curr->isstarted = 1;
                start = ctr1000;
        }

	disable(ps);
	if (isbadpid(pid) || newprio<=0 ||
	    (pptr = &proctab[pid])->pstate == PRFREE) {
		restore(ps);
                if(inblock) {
                        end = ctr1000;
                        curr->sys_runtime[id] += end - start;
                }
		return(SYSERR);
	}
	pptr->pprio = newprio;
	restore(ps);

	if(inblock) {
		end = ctr1000;
		curr->sys_runtime[id] += end - start;
	}
	return(newprio);
}
