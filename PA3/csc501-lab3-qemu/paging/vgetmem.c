/* vgetmem.c - vgetmem */

#include <conf.h>
#include <kernel.h>
#include <mem.h>
#include <proc.h>
#include <paging.h>

extern struct pentry proctab[];
/*------------------------------------------------------------------------
 * vgetmem  --  allocate virtual heap storage, returning lowest WORD address
 *------------------------------------------------------------------------
 */
WORD	*vgetmem(nbytes)
	unsigned nbytes;
{
    STATWORD ps;
    struct mblock *p, *q, *leftover;
    disable(ps);
    
    struct mblock *vptr = proctab[currpid].vmemlist;
    if (nbytes <= 0 || proctab[currpid].vmemlist->mnext == (struct mblock *)NULL) {
        restore(ps);
        return( (WORD *)SYSERR );
    }
    
    nbytes = (unsigned int) roundmb(nbytes);
    for (q = vptr, p = vptr->mnext; p != (struct mblock *)NULL; q = p, p = p->mnext) {
        if (q->mlen == nbytes) {
            q->mnext = p->mnext;
            restore(ps);
            return( (WORD *)p );
        } else if (q->mlen > nbytes) {
            leftover = (struct mblock *)( (unsigned)p + nbytes );
            q->mnext = leftover;
            q->mlen = q->mlen - nbytes;
            restore(ps);
            return( (WORD *)p );
        }
    }
    restore(ps);
    return( (WORD *)SYSERR );
}


