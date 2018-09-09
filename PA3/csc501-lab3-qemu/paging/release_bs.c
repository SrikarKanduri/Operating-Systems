#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <paging.h>

SYSCALL release_bs(bsd_t bs_id) {
    STATWORD ps;
    disable(ps);

    /* release the backing store with ID bs_id */
    if(free_bsm(bs_id) == SYSERR) {
        restore(ps);
        return SYSERR;
    }
    restore(ps);
    return OK;

}

