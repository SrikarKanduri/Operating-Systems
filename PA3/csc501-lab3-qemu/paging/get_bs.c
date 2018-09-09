#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <paging.h>

bs_map_t bsm_tab[NBS];

int get_bs(bsd_t bs_id, unsigned int npages) {
    STATWORD ps;
    disable(ps);
    
    if(invalid_bs(bs_id) || invalid_pg(npages) || bsm_tab[bs_id].bs_prvt == 1) {
        restore(ps);
        return SYSERR;
    }
    
    /* requests a new mapping of npages with ID map_id */
    if(BSM_UNMAPPED == bsm_tab[bs_id].bs_status) {
        if(bsm_map(currpid, VIRTUAL_BASE, bs_id, npages) == SYSERR) {
            restore(ps);
            return SYSERR;
        }
    }
    
    restore(ps);
    return bsm_tab[bs_id].bs_maxpages;
}


