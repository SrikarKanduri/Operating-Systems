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

int prX;
void halt();

/*------------------------------------------------------------------------
 *  *  main  --  user main program
 *   *------------------------------------------------------------------------
 *    */
prch(c)
char c;
{
	int i;
	sleep(1);	
}

int main() {
	kprintf("Task 1 (zfunction)\n");	
	kprintf("%08x\n",zfunction(0xaabbccdd));
	kprintf("Task 2 (printsegaddress)\n");
	printsegaddress();
	kprintf("Task 3 (printtos)\n");
	printtos();
	resume(prX = create(prch,2000,20,"proc X",1,'A'));
	sleep(2);
	kprintf("Task 4 (printprocstks)\n");
	printprocstks(0);
	//kprintf("Task 5 (printsyscallsummary)\n");
	//syscallsummary_start();	
	//sleep(1);
	//resume(prX = create(prch,2000,20,"proc X",1,'A'));
	//sleep(2);
	//getpid();
	//kill(0);
	//syscallsummary_stop();
	//printsyscallsummary();
	return 0;
}
