/* releaseall.c - releaseall */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <lock.h>
#include <stdio.h>

int releaselock(int);

/*------------------------------------------------------------------------
 * releaseall  --  releases all locks specified by the descriptors
 *------------------------------------------------------------------------
 */
int releaseall(int nl, long ldesc) {
	STATWORD ps;
	disable(ps);

	int i, ret = OK;
	for( i = 0; i < nl; ++i)
		ret = releaselock((int)*((&ldesc)+i));

	resched();
	restore(ps);

	return ret;
}
/*------------------------------------------------------------------------
 * rdyReader  --  schedules readers to hold the lock 
 *------------------------------------------------------------------------
 */
int rdyReader(int pid, struct lentry *lptr) {
	int prev;
	
	proctab[pid].lockid = -1;
	proctab[pid].pstate = PRREADY;
	prev = q[pid].qprev;
	lptr->lholders[pid] = 1;
	dequeue(pid);
	insert(pid,rdyhead,(&proctab[pid])->pprio);
	lptr->nr++;

	return prev;
}
/*------------------------------------------------------------------------
 * resetprio  --  resets pprio to maxprio(p_i)
 *------------------------------------------------------------------------
 */
void resetprio(int pid) {
	int i, mx = 0;

	for( i=0; i<NLOCKS; i++) {
		if(proctab[pid].lockreq[i] != LFREE && locks[i].lprio > mx)
			mx = locks[i].lprio;
	}
	proctab[pid].pprio = (proctab[pid].oldprio > mx) ? proctab[pid].oldprio : mx;
}
/*------------------------------------------------------------------------
 * releaselock  --  release a lock specified by the descriptor
 *------------------------------------------------------------------------
 */
int releaselock(int ld) {
	register struct	lentry	*lptr;

	if (isbadlock(ld) || (lptr = &locks[ld])->lstate == LFREE || proctab[currpid].lockreq[ld] == LFREE)
		return(SYSERR);

	lptr->lholders[currpid] = 0;
	proctab[currpid].lockreq[ld] = LFREE;
	resetprio(currpid);

	if(lptr->lstate == READ) {
		lptr->nr--;
	}
	
	if(q[lptr->lqtail].qprev == lptr->lqhead && lptr->nr == 0) {
		lptr->lstate = LFREE;
	}	
	else if(lptr->nr == 0) {
		int rd = 0;
		int pid = q[lptr->lqtail].qprev;

		if(proctab[pid].lockreq[ld] == READ) {
			rd = 1;
			while( pid != lptr->lqhead && proctab[pid].lockreq[ld] == READ )
				pid = rdyReader(pid, lptr);
		} 
		if(pid != lptr->lqhead && proctab[pid].lockreq[ld] == WRITE) { 
			long wtime = proctab[pid].lwaittime;
			int prio = q[pid].qkey;
			pid = q[pid].qprev;
			while( pid != lptr->lqhead && q[pid].qkey == prio ) {
				if( (wtime - proctab[pid].lwaittime) > 500L )
					break;
				if(proctab[pid].lockreq[ld] != WRITE) {
					rd = 1;
					pid = rdyReader(pid, lptr);
				} 
				else	pid = q[pid].qprev;
			}
		}
		if(rd == 1) {
			lptr->lstate = READ;
			resched();
		}
		else {
			lptr->lstate = WRITE;
			pid = getlast(lptr->lqtail);
			proctab[pid].lockid = -1;
			lptr->lholders[pid] = 1;
			ready(pid, RESCHYES);
		}
	}
	return(OK);
}
