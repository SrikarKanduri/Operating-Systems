/* xm.c = xmmap xmunmap */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <paging.h>


/*-------------------------------------------------------------------------
 * xmmap - xmmap
 *-------------------------------------------------------------------------
 */
SYSCALL xmmap(int virtpage, bsd_t source, int npages)
{
    STATWORD ps;
    disable(ps);
    
    if((bsm_map(currpid, virtpage, source, npages) == SYSERR) || npages > bsm_tab[source].bs_maxpages) {
        restore(ps);
        return SYSERR;
    }
    restore(ps);
    return OK;
}

/*-------------------------------------------------------------------------
 * xmunmap - xmunmap
 *-------------------------------------------------------------------------
 */
SYSCALL xmunmap(int virtpage)
{
    STATWORD ps;
    disable(ps);
    
    if(bsm_unmap(currpid, virtpage, 0) == SYSERR) {
        restore(ps);
        return SYSERR;
    }
    restore(ps);
    return OK;
}
