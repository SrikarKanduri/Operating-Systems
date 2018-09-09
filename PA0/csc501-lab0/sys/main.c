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
	int c;
	kprintf("%08x\n",zfunction(0xabcdabcd));
	printsegaddress();
	printtos();
	printprocstks(0);
	syscallsummary_start();
	resume(prX = create(prch,2000,20,"proc X",1,'A'));
	sleep(2);
	syscallsummary_stop();
	printsyscallsummary();
	return 0;
}
