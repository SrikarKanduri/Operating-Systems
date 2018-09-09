/* gettime.c - gettime */

#include <conf.h>
#include <kernel.h>
#include <date.h>
#include <proc.h>

extern int getutim(unsigned long *);

/*------------------------------------------------------------------------
 *  gettime  -  get local time in seconds past Jan 1, 1970
 *------------------------------------------------------------------------
 */

extern int currpid;
extern int inblock;
extern unsigned long ctr1000;

SYSCALL	gettime(long *timvar)
{
    /* long	now; */

	/* FIXME -- no getutim */
	
	int id = 4;
        struct pentry *curr = &proctab[currpid];
        unsigned long start, end;

        if(inblock) {
		start = ctr1000;
		end = ctr1000;
                curr->sys_runtime[id] += end - start;
                curr->sys_count[id]++;
                curr->isstarted = 1;
        }
	
	return OK;
}
