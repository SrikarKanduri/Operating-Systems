/* bsm.c - manage the backing store mapping*/

#include <conf.h>
#include <kernel.h>
#include <paging.h>
#include <proc.h>

/*-------------------------------------------------------------------------
 * init_bsm- initialize bsm_tab
 *-------------------------------------------------------------------------
 */
SYSCALL init_bsm()
{
    STATWORD ps;
    disable(ps);
    
    int i, j;
    for(i = 0; i < NBS; i++) {
        bsm_tab[i].bs_status = BSM_UNMAPPED;
        for(j = 0; j < 50; j++) {
            bsm_tab[i].bs_pid[j] = 0;
            bsm_tab[i].bs_vpno[j] = VIRTUAL_BASE;
            bsm_tab[i].bs_npages[j] = 0;
        }
        bsm_tab[i].bs_sem = 0;
        bsm_tab[i].bs_prvt = 0;
        bsm_tab[i].bs_maxpages = 0;
    }
    restore(ps);
    return OK;
}

/*-------------------------------------------------------------------------
 * get_bsm - get a free entry from bsm_tab 
 *-------------------------------------------------------------------------
 */
SYSCALL get_bsm(int* avail)
{
    STATWORD ps;
    disable(ps);

    int i;
    for(i = 0; i < NBS; i++) {
        if( BSM_UNMAPPED == bsm_tab[i].bs_status ) {
            *avail = i;
            restore(ps);
            return OK;
        }
    }
    restore(ps);
    return SYSERR;
}


/*-------------------------------------------------------------------------
 * free_bsm - free an entry from bsm_tab 
 *-------------------------------------------------------------------------
 */
SYSCALL free_bsm(int i)
{
    int j;
    STATWORD ps;
    disable(ps);
    
    if(invalid_bs(i)) {
        restore(ps);
        return SYSERR;
    }
    
    for(j = 0; j < 50; j++) {
        if(bsm_tab[i].bs_pid[j] == 1) {
            restore(ps);
            return OK;
        }
    }

    bsm_tab[i].bs_status = BSM_UNMAPPED;
    bsm_tab[i].bs_sem = 0;
    bsm_tab[i].bs_prvt = 0;
    for(j = 0; j < 50; j++) {
        bsm_tab[i].bs_pid[j] = 0;
        bsm_tab[i].bs_vpno[j] = VIRTUAL_BASE;
        bsm_tab[i].bs_npages[j] = 0;
    }
    
    restore(ps);
    return OK;
}

/*-------------------------------------------------------------------------
 * bsm_lookup - lookup bsm_tab and find the corresponding entry
 *-------------------------------------------------------------------------
 */
SYSCALL bsm_lookup(int pid, long vaddr, int* store, int* pageth)
{
    STATWORD ps;
    disable(ps);
    
    int i;
    for(i = 0; i < NBS; i++) {
        if(bsm_tab[i].bs_pid[pid]) {
            *store = i;
            *pageth = (vaddr / NBPG) - bsm_tab[i].bs_vpno[pid];
            restore(ps);
            return OK;
        }
    }
    restore(ps);
    return SYSERR;
}


/*-------------------------------------------------------------------------
 * bsm_map - add a mapping into bsm_tab
 *-------------------------------------------------------------------------
 */
SYSCALL bsm_map(int pid, int vpno, int source, int npages)
{
    STATWORD ps;
    disable(ps);
    
    if(vpno < NBPG || invalid_bs(source) || invalid_pg(npages)) {
        restore(ps);
        return SYSERR;
    }
    if(BSM_UNMAPPED == bsm_tab[source].bs_status) {
        bsm_tab[source].bs_maxpages = npages;
    }
    bsm_tab[source].bs_status = BSM_MAPPED;
    bsm_tab[source].bs_pid[pid] = 1;
    bsm_tab[source].bs_vpno[pid] = vpno;
    bsm_tab[source].bs_npages[pid] = npages;
    bsm_tab[source].bs_sem = 1;
    bsm_tab[source].bs_prvt = 0;
    proctab[pid].vhpno = vpno;
    proctab[pid].store = source;
    restore(ps);
    return OK;
}

/*-------------------------------------------------------------------------
 * bsm_unmap - delete an mapping from bsm_tab
 *-------------------------------------------------------------------------
 */
SYSCALL bsm_unmap(int pid, int vpno, int flag)
{
    STATWORD ps;
    disable(ps);
    
    if(vpno < NBPG) {
        restore(ps);
        return SYSERR;
    }
    
    int i, store, pageth;
    bsm_lookup(pid, vpno * NBPG, &store, &pageth);

    for(i = 0; i < NFRAMES; i++) {
        if( (pid == frm_tab[i].fr_pid) && (FR_PAGE == frm_tab[i].fr_type) )
//            free_frm(i);
            write_bs( NBPG * (NFRAMES + i), store, pageth);
    }
    bsm_tab[store].bs_pid[pid] = 0;
    bsm_tab[store].bs_vpno[pid] = VIRTUAL_BASE;
    bsm_tab[store].bs_npages[pid] = 0;
    bsm_tab[store].bs_maxpages = 0;
    
    //Recalculate bs_maxpages
    for(i = 0; i < 50; i++) {
        if(bsm_tab[store].bs_npages[i] > bsm_tab[store].bs_maxpages) {
            bsm_tab[store].bs_maxpages = bsm_tab[store].bs_npages[i];
        }
    }
    free_bsm(store);
    
    restore(ps);
    return OK;
}


