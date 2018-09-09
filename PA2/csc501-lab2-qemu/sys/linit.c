/* linit.c - linit */

#include <conf.h>
#include <kernel.h>
#include <lock.h>
#include <q.h>
#include <stdio.h>

/*------------------------------------------------------------------------
 * linit - initializes lock queue
 *------------------------------------------------------------------------
 */
struct  lentry  locks[NLOCKS];	/* locks table */

void linit() {
	int	i, j;
	struct  lentry  *lptr;
	
	for (i=0 ; i<NLOCKS ; i++) {      /* initialize locks */
		(lptr = &locks[i])->lstate = LFREE;
		lptr->lqtail = 1 + (lptr->lqhead = newqueue());
		lptr->nr = 0;
		lptr->lprio = 0;
		for (j=0; j<NPROC; j++)
			lptr->lholders[j] = 0;
        }
}
