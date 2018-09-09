/* getpid.c - getpid */

#include <conf.h>
#include <kernel.h>
#include <proc.h>

/*------------------------------------------------------------------------
 * getpid  --  get the process id of currently executing process
 *------------------------------------------------------------------------
 */

extern int currpid;
extern int inblock;
extern unsigned long ctr1000;

SYSCALL getpid()
{
        int id = 2;
        struct pentry *curr = &proctab[currpid];
        unsigned long start, end;

        if(inblock) {
		start = ctr1000;
		end = ctr1000;                
		curr->sys_runtime[id] += end - start;
                curr->sys_count[id]++;
                curr->isstarted = 1;
        }

	return(currpid);
}
