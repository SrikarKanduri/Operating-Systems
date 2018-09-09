/* scount.c - scount */

#include <conf.h>
#include <kernel.h>
#include <sem.h>
#include <proc.h>

/*------------------------------------------------------------------------
 *  scount  --  return a semaphore count
 *------------------------------------------------------------------------
 */

extern int currpid;
extern int inblock;
extern unsigned long ctr1000;

SYSCALL scount(int sem)
{
	extern	struct	sentry	semaph[];

        int id = 10;
        struct pentry *curr = &proctab[currpid];
        unsigned long start, end;
        if(inblock) {
                curr->sys_count[id]++;
                curr->isstarted = 1;
                start = ctr1000;
        }

	if (isbadsem(sem) || semaph[sem].sstate==SFREE) {
               if(inblock) {
                        end = ctr1000;
                        curr->sys_runtime[id] += end - start;
                }
		return(SYSERR);
	}


        if(inblock) {
		end = ctr1000;
                curr->sys_runtime[id] += end - start;
        }

	return(semaph[sem].semcnt);
}
