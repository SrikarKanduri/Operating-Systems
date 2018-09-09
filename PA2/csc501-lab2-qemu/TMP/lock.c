/* lock.c - lock, canGainLock, maxprio, dfs */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <lock.h>
#include <stdio.h>

long 	ctr1000;
/*------------------------------------------------------------------------
 *  canGainLock  --  has a high-priority/longest waiting writer or not
 *------------------------------------------------------------------------
 */
int canGainLock(int ld, int prio) {
	struct  lentry  *lptr = &locks[ld];
	int pid = lptr->lqtail;

	while( (pid = q[pid].qprev) != lptr->lqhead ) {
		if(proctab[pid].lockreq[ld] == WRITE) {
			if(q[pid].qkey >= prio) return 0;
			else return 1;
		}
	}
	return 1;
}

/*------------------------------------------------------------------------
 * maxprio  --  recalculate max pprio for priority inheritance
 *------------------------------------------------------------------------
 */
int maxprio(int ld) {
        int mx = 0, proc = -1;
        struct  lentry  *lptr = &locks[ld];
        int pid = lptr->lqtail;
        lptr->lprio = 0;
        while( (pid = q[pid].qprev) != lptr->lqhead ) {
                if(proctab[pid].pprio > mx) {
                        mx = proctab[pid].pprio;
                        proc = pid;
                }
        }
        return proc;
}

/*------------------------------------------------------------------------
 * dfs  --  recursive priority inheritence
 *------------------------------------------------------------------------
 */
void dfs(int pid, int ld, int kf) { 	/* kf: kill flag */
	int	i;

	if( (ld == -1) || (kf == 1 && pid != maxprio(ld)) ) return;
	if(proctab[pid].pprio > locks[ld].lprio) 
		locks[ld].lprio = proctab[pid].pprio;

	for (i=0; i<NPROC; i++) {
		if( (locks[ld].lholders[i] == 1) && 
			((kf == 0 && proctab[i].pprio < locks[ld].lprio) ||
				(kf == 1 && (proctab[i].pprio = proctab[i].oldprio) < locks[ld].lprio)) ) {
				proctab[i].pprio = locks[ld].lprio;
				dfs(i, proctab[i].lockid, kf);
		}
	}
}

/*------------------------------------------------------------------------
 *  lock  --  wait for the lock
 *------------------------------------------------------------------------
 */
int lock(int ld, int type, int prio)
{
	STATWORD ps;    
	struct	lentry	*lptr;
	struct	pentry	*pptr = &proctab[currpid];

	disable(ps);
	if (isbadlock(ld) || ((lptr = &locks[ld])->lstate == LFREE)) {
		restore(ps);
		return(SYSERR);
	}
	
	pptr->lockreq[ld] = type;
	
	if(lptr->lstate == LUSED) {	/* lock is used for the first time */
		lptr->lstate = type;
		lptr->lholders[currpid] = 1;
		if(type == READ) lptr->nr = 1;
	}
	else if (lptr->lstate == READ && type == READ && canGainLock(ld, prio)) {
			lptr->lholders[currpid] = 1;
			lptr->nr++;
	}
	else {				/* block process */
		pptr->lockid = ld;
		pptr->lwaittime = ctr1000;
		pptr->pstate = PRWAIT;
		pptr->plwaitret = OK;
		insert(currpid, lptr->lqhead, prio);
		dfs(currpid, ld, 0);
		resched();
		restore(ps);
		return pptr->plwaitret;
	}
	restore(ps);
	return(pptr->plwaitret);
}
