/* printsegaddress.c - printsegaddress */

#include <stdio.h>
extern long etext, edata, end;

/*------------------------------------------------------------------------
 *  printsegaddress  --  prints end addresses of the text, data, and BSS
 *------------------------------------------------------------------------
 */

void printsegaddress() {
        kprintf("\nvoid printsegaddress()\n");
	kprintf("Current: etext[0x%08x]=0x%08x, edata[0x%08x]=0x%08x, ebss[0x%08x]=0x%08x\n", &etext, *&etext, &edata, *&edata, &end, *&end);
        kprintf("Preceding: etext[0x%08x]=0x%08x, edata[0x%08x]=0x%08x, ebss[0x%08x]=0x%08x\n", &etext-1, *(&etext-1), &edata-1, *(&edata-1), &end-1, *(&end-1));
        kprintf("After: etext[1x%08x]=0x%08x, edata[0x%08x]=0x%08x, ebss[0x%08x]=0x%08x\n", &etext+1, *(&etext+1), &edata+1, *(&edata+1), &end+1, *(&end+1));
}
