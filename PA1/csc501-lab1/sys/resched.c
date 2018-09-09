/* resched.c  -  resched, randsched, linuxsched */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <lab1.h>
#include <stdio.h>

unsigned long currSP;	/* REAL sp of current process */
extern int ctxsw(int, int, int, int);
static int sched;

int randsched();
int linuxsched();
int defaultsched();

/*-----------------------------------------------------------------------
 * resched  --  reschedule processor to a random process or quantum-wise
 *
 * Notes:	Upon entry, currpid gives current process id.
 *		Proctab[currpid].pstate gives correct NEXT state for
 *			current process if other than PRREADY.
 *------------------------------------------------------------------------
 */

void setschedclass(int sched_class) {
	sched = sched_class;
}

int getschedclass() {
	return sched;
}

int resched()
{
	if (getschedclass() == RANDOMSCHED)
		return randsched();
	else if (getschedclass() == LINUXSCHED)
		return linuxsched();
	
	return defaultsched();	
}

int prio_sum() {
        int sum = 0;
        int proc = q[rdyhead].qnext;
        while (proc != rdytail) {
                sum += q[proc].qkey;
                proc = q[proc].qnext;
        }
        return sum;
}

int randsched()
{
        register struct pentry  *optr;  /* pointer to old process entry */
        register struct pentry  *nptr;  /* pointer to new process entry */

        /* insert curr process into ready queue */
        if ((optr= &proctab[currpid])->pstate == PRCURR) {
                optr->pstate = PRREADY;
                insert(currpid,rdyhead,optr->pprio);
        }

	int rand_proc = q[rdytail].qprev;
	int sumprio = prio_sum();

	/* If NULL isn't the only process in the q */
	if(sumprio != 0) {
		int rand_num = rand() % sumprio;
	
		while (rand_num > q[rand_proc].qkey) {
               		rand_num -= q[rand_proc].qkey;
               		rand_proc = q[rand_proc].qprev;
       		}
	}
	
        /* remove randomly chosen process */
        nptr = &proctab[ (currpid = rand_proc) ];
        dequeue(rand_proc);
        nptr->pstate = PRCURR;          /* mark it currently running    */
	
#ifdef  RTCLOCK
        preempt = QUANTUM;              /* reset preemption counter     */
#endif
        ctxsw((int)&optr->pesp, (int)optr->pirmask, (int)&nptr->pesp, (int)nptr->pirmask);

        /* The OLD process returns here when resumed. */
        return OK;
}

int is_new_epoch() {
        register struct pentry  *ptr;
        int proc = q[rdyhead].qnext;
        while (proc != rdytail) {
		ptr = &proctab[proc];

		/* process is in curr epoch and not exhausted */
		if(ptr->timequantum > 0)
			return 0;

		proc = q[proc].qnext;
        }
	return 1;
}

int maxgoodproc() {
        register struct pentry  *ptr;
        int goodness, max_good = 0, resproc;
        int proc = q[rdyhead].qnext;

        while (proc != rdytail) {
                ptr = &proctab[proc];
		if(ptr->timequantum > 0)
	                goodness = ptr->timequantum + ptr->oldprio;
		else
			goodness = 0;
		if(goodness >= max_good) {
			max_good = goodness;
			resproc = proc;
		}
                proc = q[proc].qnext;
        }
        
	return resproc;
}

int linuxsched()
{
        register struct pentry  *optr = &proctab[currpid];  /* pointer to old process entry */
        register struct pentry  *nptr;  /* pointer to new process entry */
        
	if (optr->pstate == PRCURR) {
                optr->pstate = PRREADY;
                insert(currpid,rdyhead,optr->pprio);
        }

	optr->timequantum = preempt;

	/* If new epoch starts, recalculate quantums */
	if( is_new_epoch() ) {
		int i;
		for(i = 0; i < NPROC; i++) {
			register struct pentry  *ptr = &proctab[i];
			if(ptr->pstate != PRFREE) {
				ptr->oldprio = ptr->pprio; /* save prio to avoid prio changes in middle of an epoch */
				int counter = ptr->timequantum;

				/* reset quantum for process that exhausted its old quantum or wasn't executed */
				if( counter == 0 || ptr->timequantum == ptr->oldquant )
					ptr->timequantum = ptr->pprio;
				else
					ptr->timequantum = (counter/2) + ptr->pprio;

				ptr->oldquant = ptr->timequantum; /* save quantum to check if exhausted in next epoch */
			}
		}
	}
	
        /* remove process with highest goodness */
        nptr = &proctab[ (currpid = maxgoodproc()) ];
        dequeue(currpid);
	nptr->pstate = PRCURR;          /* mark it currently running    */
#ifdef  RTCLOCK
	if(nptr->pprio == 0)		/* NULL process */
		preempt = QUANTUM;
	else
        	preempt = nptr->timequantum;              /* reset preemption counter     */
#endif
        ctxsw((int)&optr->pesp, (int)optr->pirmask, (int)&nptr->pesp, (int)nptr->pirmask);

        /* The OLD process returns here when resumed. */
        return OK;
}

int defaultsched()
{
        register struct pentry  *optr;  /* pointer to old process entry */
        register struct pentry  *nptr;  /* pointer to new process entry */

        /* no switch needed if current process priority higher than next*/

        if ( ( (optr= &proctab[currpid])->pstate == PRCURR) &&
           (lastkey(rdytail)<optr->pprio)) {
                return(OK);
        }

        /* force context switch */

        if (optr->pstate == PRCURR) {
                optr->pstate = PRREADY;
                insert(currpid,rdyhead,optr->pprio);
        }

        /* remove highest priority process at end of ready list */

        nptr = &proctab[ (currpid = getlast(rdytail)) ];
        nptr->pstate = PRCURR;          /* mark it currently running    */
#ifdef  RTCLOCK
        preempt = QUANTUM;              /* reset preemption counter     */
#endif

        ctxsw((int)&optr->pesp, (int)optr->pirmask, (int)&nptr->pesp, (int)nptr->pirmask);

        /* The OLD process returns here when resumed. */
        return OK;
}
