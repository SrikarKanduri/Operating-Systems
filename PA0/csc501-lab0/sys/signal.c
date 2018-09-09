/* signal.c - signal */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <sem.h>
#include <stdio.h>

/*------------------------------------------------------------------------
 * signal  --  signal a semaphore, releasing one waiting process
 *------------------------------------------------------------------------
 */

extern int currpid;
extern int inblock;
extern unsigned long ctr1000;

SYSCALL signal(int sem)
{
	STATWORD ps;    
	register struct	sentry	*sptr;

        int id = 16;
        struct pentry *curr = &proctab[currpid];
        unsigned long start, end;
        if(inblock) {
                curr->sys_count[id]++;
                curr->isstarted = 1;
                start = ctr1000;
        }

	disable(ps);
	if (isbadsem(sem) || (sptr= &semaph[sem])->sstate==SFREE) {
		restore(ps);
                if(inblock) {
                        end = ctr1000;
                        curr->sys_runtime[id] += end - start;
                }
		return(SYSERR);
	}
	if ((sptr->semcnt++) < 0)
		ready(getfirst(sptr->sqhead), RESCHYES);
	restore(ps);

        if(inblock) {
		end = ctr1000;
                curr->sys_runtime[id] += end - start;
        }
	
	return(OK);
}
