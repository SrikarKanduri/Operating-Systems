/* user.c - main */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <lab0.h>
#include <stdio.h>

/*------------------------------------------------------------------------
 *  main  --  user main program
 *------------------------------------------------------------------------
 */

int main() {
	int c;
	kprintf("Task 1 (zfunction)\n");	
	kprintf("%08x\n",zfunction(0xaabbccdd));
	kprintf("Task 2 (printsegaddress)\n");
	printsegaddress();
	kprintf("Task 3 (printtos)\n");
	printtos();
	kprintf("Task 4 (printprocstks)\n");
	printprocstks(0);
	sleep(1);
	kprintf("Task 5 (printsyscallsummary)\n");
	syscallsummary_start();	
	sleep(1);
	sleep(1);
	getpid();
	syscallsummary_stop();
	printsyscallsummary();
	return 0;
}
