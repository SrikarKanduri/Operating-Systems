/* ldelete.c -  */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <lock.h>
#include <stdio.h>

/*------------------------------------------------------------------------
 * ldelete  --  deletes the lock specified by the descriptor
 *------------------------------------------------------------------------
 */
int ldelete(int ld)
{
	STATWORD ps;    
	register struct	lentry	*lptr;
	int	pid;

	disable(ps);
	if (isbadlock(ld) || (lptr= &locks[ld])->lstate == LFREE) {
		restore(ps);
		return(SYSERR);
	}

	lptr->lstate = LFREE;
	if (nonempty(lptr->lqhead)) {
		while( (pid=getfirst(lptr->lqhead)) != EMPTY) {
			proctab[pid].plwaitret = DELETED;
			ready(pid,RESCHNO);
		}
		resched();
	}

	restore(ps);
        return(DELETED);
}
