/* vfreemem.c - vfreemem */

#include <conf.h>
#include <kernel.h>
#include <mem.h>
#include <proc.h>
#include <paging.h>

extern struct pentry proctab[];
/*------------------------------------------------------------------------
 *  vfreemem  --  free a virtual memory block, returning it to vmemlist
 *------------------------------------------------------------------------
 */
SYSCALL	vfreemem(block, size)
	struct	mblock	*block;
	unsigned size;
{
    STATWORD ps;
    disable(ps);
    
    if(size <= 0 || block < VIRTUAL_BASE * NBPG) {
        restore(ps);
        return SYSERR;
    }
    
    struct mblock *vmem = proctab[currpid].vmemlist;
    struct mblock *prev = vmem;
    struct mblock *curr = vmem->mnext;
    
    while(curr != NULL && curr < block) {
        prev = curr;
        curr = curr->mnext;
    }
    
    block->mlen = size;
    block->mnext = curr;
    prev->mnext = block;
    
    restore(ps);
	return(OK);
}
