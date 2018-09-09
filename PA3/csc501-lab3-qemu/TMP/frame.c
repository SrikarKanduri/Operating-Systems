/* frame.c - manage physical frames */
#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <paging.h>

extern int page_replace_policy;
extern int debug;
extern int scqhead, scqtail, scqcnt;
/*-------------------------------------------------------------------------
 * init_frm - initialize frm_tab
 *-------------------------------------------------------------------------
 */
SYSCALL init_frm()
{
    STATWORD ps;
    disable(ps);
    
    int i;
    for(i = 0; i < NFRAMES; i++) {
        frm_tab[i].fr_status = FRM_UNMAPPED;
        frm_tab[i].fr_pid = -1;
        frm_tab[i].fr_vpno = VIRTUAL_BASE;
        frm_tab[i].fr_refcnt = 0;
        frm_tab[i].fr_type = FR_PAGE;
        frm_tab[i].fr_dirty = 0;
        frm_tab[i].fr_cnt = 0;
    }
    
    restore(ps);
    return OK;
}

/*-------------------------------------------------------------------------
 * get_frm - get a free frame according page replacement policy
 *-------------------------------------------------------------------------
 */
SYSCALL get_frm(int* avail)
{
    STATWORD ps;
    disable(ps);
    
    int i;
    for(i = 0; i < NFRAMES; i++) {
        if( FRM_UNMAPPED == frm_tab[i].fr_status ) {
            *avail = i;
//            kprintf("Replacing the frame: %d\n", *avail);
            restore(ps);
            return OK;
        }
    }
    
    int frm = (page_replace_policy == SC) ? get_frm_sc() : get_frm_lfu();

    if(frm == -1) {
        restore(ps);
        return SYSERR;
    }
    
    if(debug) kprintf("Replacing the frame: %d\n", FRAME0 + frm);
    free_frm(frm);
    *avail = frm;

    restore(ps);
    return OK;
}

/*-------------------------------------------------------------------------
 * free_frm - free a frame 
 *-------------------------------------------------------------------------
 */
SYSCALL free_frm(int i)
{
    STATWORD ps;
    disable(ps);
    
    if(invalid_frm(i) || frm_tab[i].fr_type != FR_PAGE) {
        restore(ps);
        return SYSERR;
    }
    int pid = frm_tab[i].fr_pid;
    int page = frm_tab[i].fr_vpno - proctab[pid].vhpno;
    unsigned int virt_addr = frm_tab[i].fr_vpno;
    unsigned int pd_off = virt_addr / 1024;
    unsigned int pt_off = virt_addr & 1023;
    //unsigned int pd_off = ((virt_addr >> 10) & (0x000003ff));
    //unsigned int pt_off = virt_addr;
    pd_t *pd = proctab[pid].pdbr + (pd_off * sizeof(pd_t));
    pt_t *pt = (pd->pd_base * NBPG) + (pt_off * sizeof(pt_t));
    
    write_bs((FRAME0 + i) * NBPG, proctab[pid].store, page);
    pt->pt_pres = 0;
    
    int frm = pd->pd_base - FRAME0;
    if(--frm_tab[frm].fr_refcnt == 0) {
        frm_tab[i].fr_status = FRM_UNMAPPED;
        frm_tab[i].fr_pid = -1;
        frm_tab[i].fr_vpno = VIRTUAL_BASE;
        frm_tab[i].fr_type = FR_PAGE;
        pd->pd_pres = 0;
    }
    restore(ps);
    return OK;
}

int get_frm_sc()
{
    int frm, i;
    for(frm = scqhead; frm <= scqtail; frm = (frm+1)%NFRAMES) {
        if(frm == scqtail) frm = scqtail - scqcnt;
        int j = scq[frm];
        int pid = frm_tab[j].fr_pid;
        unsigned int virt_addr = frm_tab[j].fr_vpno;
        unsigned int pd_off = virt_addr / 1024;
        unsigned int pt_off = virt_addr & 1023;
        pd_t *pd = proctab[pid].pdbr + (pd_off * sizeof(pd_t));
        pt_t *pt = (pd->pd_base * NBPG) + (pt_off * sizeof(pt_t));
        if(pt->pt_acc == 0) {
            scqhead = frm;
            for(i = frm; i < scqtail-1; i = (i+1)%NFRAMES)
                scq[i] = scq[(i+1)%NFRAMES];
            scqtail = (scqtail-1)%NFRAMES;
            scqcnt--;
            return j;
        } else pt->pt_acc = 0;
    }
}

int get_frm_lfu()
{
    int i, frm, min_cnt = -1;
    for(i = 0; i < NFRAMES; i++)
        if( FR_PAGE == frm_tab[i].fr_type) {
            if(frm_tab[i].fr_cnt < min_cnt || min_cnt == -1) {
                min_cnt = frm_tab[i].fr_cnt;
                frm = i;
            } else if(frm_tab[i].fr_cnt == min_cnt && frm_tab[i].fr_vpno > frm_tab[frm].fr_vpno)
                    frm = i;
        }
    return frm;
}

void release_frm(int pid)
{
    int i;
    for(i = 0; i < NFRAMES; i++) {
        
        if(pid == frm_tab[i].fr_pid) {
            frm_tab[i].fr_status = FRM_UNMAPPED;
            frm_tab[i].fr_pid = -1;
            frm_tab[i].fr_vpno = VIRTUAL_BASE;
            frm_tab[i].fr_refcnt = 0;
            frm_tab[i].fr_type = FR_PAGE;
            frm_tab[i].fr_dirty = 0;
        }
    }
}

int set_pd(int pid)
{
    int i, frm;
    if(get_frm(&frm) == SYSERR)
        return SYSERR;

    frm_tab[frm].fr_status = FRM_MAPPED;
    frm_tab[frm].fr_pid = pid;
    frm_tab[frm].fr_type = FR_DIR;
    proctab[pid].pdbr = (FRAME0 + frm) * NBPG;
    pd_t *pd = (pd_t *) proctab[pid].pdbr;
    
    for(i = 0; i < 4; i++) {
        pd[i].pd_pres = 1;
        pd[i].pd_write = 1;
        pd[i].pd_base = FRAME0 + i;
    }
    
    for(i = 4; i < NFRAMES; i++)
        pd[i].pd_pres = 0;
    
    return OK;
}

int set_pt()
{
    int i, frm;
    if(get_frm(&frm) == SYSERR)
        return SYSERR;
    
    pt_t *pt = (pt_t *) (NBPG * (FRAME0 + frm));
    for(i = 0; i < NFRAMES; i++) {
        pt[i].pt_write = 1;
        pt[i].pt_pres = 0;
        pt[i].pt_user = 0;
        pt[i].pt_pwt = 0;
        pt[i].pt_pcd = 0;
        pt[i].pt_acc = 0;
        pt[i].pt_dirty = 0;
        pt[i].pt_mbz = 0;
        pt[i].pt_global = 0;
        pt[i].pt_avail = 0;
        pt[i].pt_base = 0;
    }
    return frm;
}
