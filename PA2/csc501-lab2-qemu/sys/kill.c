/* kill.c - kill */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <sem.h>
#include <mem.h>
#include <io.h>
#include <q.h>
#include <lock.h>
#include <stdio.h>

void handleInh(int pid) {
        int     proc;
        int     i;
	int	ld = proctab[pid].lockid;

	dequeue(pid);
        if(ld != -1) { // has transitivity
                if( (proc = maxprio(ld)) != -1)
			dfs(proc, ld, 1);
		else {
			locks[ld].lprio = 0;
			for (i=0; i<NPROC; i++) {
				if(locks[ld].lholders[i] == 1) resetprio(i);
				dfs(i, proctab[i].lockid, 1);
			}
		}
                for (i=0; i<NLOCKS; i++)
                        if(proctab[pid].lockreq[i] != LFREE) releaseall(1, i);
        }
}

/*------------------------------------------------------------------------
 * kill  --  kill a process and remove it from the system
 *------------------------------------------------------------------------
 */
SYSCALL kill(int pid)
{
	STATWORD ps;    
	struct	pentry	*pptr;		/* points to proc. table for pid*/
	int	dev;

	disable(ps);
	if (isbadpid(pid) || (pptr= &proctab[pid])->pstate==PRFREE) {
		restore(ps);
		return(SYSERR);
	}
	if (--numproc == 0)
		xdone();

	dev = pptr->pdevs[0];
	if (! isbaddev(dev) )
		close(dev);
	dev = pptr->pdevs[1];
	if (! isbaddev(dev) )
		close(dev);
	dev = pptr->ppagedev;
	if (! isbaddev(dev) )
		close(dev);

	handleInh(pid);

	send(pptr->pnxtkin, pid);

	freestk(pptr->pbase, pptr->pstklen);
	switch (pptr->pstate) {

	case PRCURR:	pptr->pstate = PRFREE;	/* suicide */
			resched();

	case PRWAIT:	semaph[pptr->psem].semcnt++;

	case PRREADY:	dequeue(pid);
			pptr->pstate = PRFREE;
			break;

	case PRSLEEP:
	case PRTRECV:	unsleep(pid);
						/* fall through	*/
	default:	pptr->pstate = PRFREE;
	}
	restore(ps);
	return(OK);
}
