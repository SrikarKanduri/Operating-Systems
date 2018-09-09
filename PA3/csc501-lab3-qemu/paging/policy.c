/* policy.c = srpolicy*/

#include <conf.h>
#include <kernel.h>
#include <paging.h>


extern int page_replace_policy;
extern int debug;
/*-------------------------------------------------------------------------
 * srpolicy - set page replace policy 
 *-------------------------------------------------------------------------
 */
SYSCALL srpolicy(int policy)
{
    STATWORD ps;
    disable(ps);
    
    /* sanity check ! */
    if(policy != SC && policy != LFU) {
        restore(ps);
        return SYSERR;
    }
    
    page_replace_policy = (policy == LFU) ? LFU : SC;
    debug = 1;
    restore(ps);
    return OK;
}

/*-------------------------------------------------------------------------
 * grpolicy - get page replace policy 
 *-------------------------------------------------------------------------
 */
SYSCALL grpolicy()
{
  return page_replace_policy;
}
