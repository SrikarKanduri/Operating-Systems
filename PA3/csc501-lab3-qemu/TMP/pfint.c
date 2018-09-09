/* pfint.c - pfint */

#include <conf.h>
#include <kernel.h>
#include <paging.h>
#include <proc.h>

extern int page_replace_policy;
extern int scqhead, scqtail, scqcnt;
extern int set_pt(void);
/*-------------------------------------------------------------------------
 * pfint - paging fault ISR
 *-------------------------------------------------------------------------
 */
SYSCALL pfint()
{
    STATWORD ps;
    disable(ps);
    
    int f, s, o;
    unsigned long a = read_cr2();
    virt_addr_t *vp = (virt_addr_t *) &a;
    unsigned int p = vp->pd_offset;
    pd_t *pd = (pd_t *)(proctab[currpid].pdbr + p * sizeof(pd_t));
    
    if(bsm_lookup(currpid, a, &s, &o) == SYSERR) {
        kprintf("Page Fault Error: Illegal Address\n");
        kill(currpid);
        restore(ps);
        return SYSERR;
    }
    
    if(pd->pd_pres == 0) {
        f = set_pt();
        pd->pd_pres = 1;
        pd->pd_write = 1;
        pd->pd_user = 1;
        pd->pd_pwt = 0;
        pd->pd_pcd = 0;
        pd->pd_acc = 0;
        pd->pd_mbz = 0;
        pd->pd_fmb = 0;
        pd->pd_global= 0;
        pd->pd_avail = 0;
        pd->pd_base = f + FRAME0;
        
        frm_tab[f].fr_status = FRM_MAPPED;
        frm_tab[f].fr_pid = currpid;
        frm_tab[f].fr_type = FR_TBL;
    }
    
    unsigned int q = vp->pt_offset;
    pt_t *pt = (pt_t *)(pd->pd_base * NBPG + q * sizeof(pt_t));
    if(pt->pt_pres == 0) {
        frm_tab[pd->pd_base - FRAME0].fr_refcnt++;
        get_frm(&f);
//        kprintf("Inserting frame %d\n",f);
        scq[scqtail] = f;
        scqtail = (scqtail + 1) % NFRAMES;
        scqcnt++;
        read_bs((char *) ((FRAME0 + f) * NBPG), s, o);
        
        pt->pt_pres = 1;
        pt->pt_write = 1;
        pt->pt_user = 1;
        pt->pt_acc = 1;
        pt->pt_base = f + FRAME0;
        frm_tab[f].fr_status = FRM_MAPPED;
        frm_tab[f].fr_pid = currpid;
        frm_tab[f].fr_vpno = a / NBPG;
        frm_tab[f].fr_type = FR_PAGE;
        frm_tab[f].fr_dirty = 0;
        frm_tab[f].fr_cnt++;
    }
    restore(ps);
    return OK;
}
