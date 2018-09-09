/* receive.c - receive */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <stdio.h>

/*------------------------------------------------------------------------
 *  receive  -  wait for a message and return it
 *------------------------------------------------------------------------
 */

extern int currpid;
extern int inblock;
extern unsigned long ctr1000;

SYSCALL	receive()
{
	STATWORD ps;    
	struct	pentry	*pptr;
	WORD	msg;

        int id = 6;
        struct pentry *curr = &proctab[currpid];
        unsigned long start, end;
        if(inblock) {
                curr->sys_count[id]++;
                curr->isstarted = 1;
                start = ctr1000;
        }

	disable(ps);
	pptr = &proctab[currpid];
	if ( !pptr->phasmsg ) {		/* if no message, wait for one	*/
		pptr->pstate = PRRECV;
		resched();
	}
	msg = pptr->pmsg;		/* retrieve message		*/
	pptr->phasmsg = FALSE;
	restore(ps);

        if(inblock) {
                end = ctr1000;
                curr->sys_runtime[id] += end - start;
        }

	return(msg);
}
