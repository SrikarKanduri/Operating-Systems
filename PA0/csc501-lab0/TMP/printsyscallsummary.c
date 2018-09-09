/* printsyscallsummary.c - syscallsummary_start, syscallsummary_stop, printsyscallsummary  */

#include <stdio.h>
#include <kernel.h>
#include <proc.h>

char *name[27] = {
"freemem",
"chprio",
"getpid",
"getprio",
"gettime",
"kill",
"receive",
"recvclr",
"recvtim",
"resume",
"scount",
"sdelete",
"send",
"setdev",
"setnok",
"screate",
"signal",
"signaln",
"sleep",
"sleep10",
"sleep100",
"sleep1000",
"sreset",
"stacktrace",
"suspend",
"unsleep",
"wait"
};

extern int currpid;
extern int inblock;
struct pentry *curr;

/*------------------------------------------------------------------------
 *  syscallsummary_start  --  sets flag to start tracking syscalls
 *------------------------------------------------------------------------
 */

void syscallsummary_start() {
	int p, q;
	kprintf("\nvoid printsyscallsummary()\n");
	inblock = 1;

	for(p = 0; p < NPROC; p++) {
		curr = &proctab[p];
		for(q = 0; q< 27; q++) {
			curr->sys_count[q] = 0;
			curr->sys_runtime[q] = 0;
			curr->isstarted = 0;
		}
	}
}

/*------------------------------------------------------------------------
 *  syscallsummary_stop  --  clears flag to stop tracking syscalls
 *------------------------------------------------------------------------
 */

void syscallsummary_stop() {
	inblock = 0;
}

/*------------------------------------------------------------------------
 *  printsyscallsummary  --  summary of syscalls of executed processes
 *------------------------------------------------------------------------
 */

void printsyscallsummary() {
	int p, q;	
	for(p = 0; p < NPROC; p++) {
		curr = &proctab[p];
		if(curr->isstarted) {
			kprintf("Process [pid:%d]\n", p);
			for(q = 0; q < 27; q++) {
				if(curr->sys_count[q])
					kprintf("\tSyscall: sys_%s, count: %d, average execution time: %ld (ms)\n", name[q], curr->sys_count[q], curr->sys_runtime[q]/curr->sys_count[q]);
			}
		}
	}
}
