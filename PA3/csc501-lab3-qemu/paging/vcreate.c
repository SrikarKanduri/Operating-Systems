/* vcreate.c - vcreate */
    
#include <conf.h>
#include <i386.h>
#include <kernel.h>
#include <proc.h>
#include <sem.h>
#include <mem.h>
#include <io.h>
#include <paging.h>

/*
static unsigned long esp;
*/

LOCAL	newpid();
/*------------------------------------------------------------------------
 *  create  -  create a process to start running a procedure
 *------------------------------------------------------------------------
 */
SYSCALL vcreate(procaddr,ssize,hsize,priority,name,nargs,args)
	int     *procaddr;		/* procedure address		*/
	int     ssize;			/* stack size in words		*/
	int     hsize;			/* virtual heap size in pages	*/
	int     priority;		/* process priority > 0		*/
	char    *name;			/* name (for debugging)		*/
	int     nargs;			/* number of args that follow	*/
	long    args;			/* arguments (treated like an	*/
					/* array in the code)		*/
{
    STATWORD ps;
    disable(ps);
    
    int source;
    int pid;
    
    if((get_bsm(&source)) == SYSERR || invalid_pg(hsize)) {
        restore(ps);
        return SYSERR;
    }
    
    //Creates a process (with stack)
    pid = create(procaddr, ssize, priority, name, nargs, args);

    //Map to backing store
    if (bsm_map(pid, VIRTUAL_BASE, source, hsize) == SYSERR) {
        restore(ps);
        return SYSERR;
    }
    
    //Create private heap
    proctab[pid].vhpnpages = hsize;
    proctab[pid].vmemlist->mnext = VIRTUAL_BASE * NBPG;
    proctab[pid].vmemlist->mlen = hsize * VIRTUAL_BASE;
    bsm_tab[source].bs_sem = 0;
    bsm_tab[source].bs_prvt = 1;
    
    restore(ps);
	return pid;
}

/*------------------------------------------------------------------------
 * newpid  --  obtain a new (free) process id
 *------------------------------------------------------------------------
 */
LOCAL	newpid()
{
	int	pid;			/* process id to return		*/
	int	i;

	for (i=0 ; i<NPROC ; i++) {	/* check all NPROC slots	*/
		if ( (pid=nextproc--) <= 0)
			nextproc = NPROC-1;
		if (proctab[pid].pstate == PRFREE)
			return(pid);
	}
	return(SYSERR);
}
