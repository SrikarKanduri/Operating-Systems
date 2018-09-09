/* sleep100.c - sleep100 */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <sleep.h>
#include <stdio.h>

/*------------------------------------------------------------------------
 * sleep100  --  delay the caller for a time specified in 1/100 of seconds
 *------------------------------------------------------------------------
 */

extern int currpid;
extern int inblock;
extern unsigned long ctr1000;

SYSCALL sleep100(int n)
{
	STATWORD ps;    

        int id = 20;
        struct pentry *curr = &proctab[currpid];
        unsigned long start, end;
        if(inblock) {
                curr->sys_count[id]++;
                curr->isstarted = 1;
                start = ctr1000;
        }

	if (n < 0  || clkruns==0) {
                if(inblock) {
                        end = ctr1000;
                        curr->sys_runtime[id] += end - start;
                }
	         return(SYSERR);
	}
	disable(ps);
	if (n == 0) {		/* sleep100(0) -> end time slice */
	        ;
	} else {
		insertd(currpid,clockq,n*10);
		slnempty = TRUE;
		sltop = &q[q[clockq].qnext].qkey;
		proctab[currpid].pstate = PRSLEEP;
	}
	resched();
        restore(ps);

        if(inblock) {
		end = ctr1000;
                curr->sys_runtime[id] += end - start;
        }

	return(OK);
}
