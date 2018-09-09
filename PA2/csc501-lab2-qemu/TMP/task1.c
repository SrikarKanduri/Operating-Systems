#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <lock.h>
#include <stdio.h>
#include <lock.h>

#define DEFAULT_LOCK_PRIO 20

void semproc (char *msg, int sem)
{	
        kprintf ("  %s: to acquire sem\n", msg);
	wait(sem);
	kprintf ("  %s: acquired sem\n", msg);
        sleep (1);
        kprintf ("  %s: to release sem\n", msg);
	signal(sem);
}

void lockproc (char *msg, int lck)
{
        kprintf ("  %s: to acquire lock\n", msg);
        lock (lck, WRITE, DEFAULT_LOCK_PRIO);
        kprintf ("  %s: acquired lock\n", msg);
        sleep (1);
        kprintf ("  %s: to release lock\n", msg);
        releaseall (1, lck);
}

void medproc(char* msg) {
	kprintf("  Process %s running.\n", msg);
	sleep(1);
	kprintf("  Process %s finished running.\n", msg);
}

void createprocs(int mode, int type) {
	int pA, pB, pC;
	if(!type) {
	        pA = create(semproc, 2000, 30, "procA", 2, "procA", mode);
        	pB = create(medproc, 2000, 40, "procB", 1, "procB");
	        pC = create(semproc, 2000, 50, "procC", 2, "procC", mode);
	}
	else {
                pA = create(lockproc, 2000, 30, "procA", 2, "procA", mode);
                pB = create(medproc, 2000, 40, "procB", 1, "procB");
                pC = create(lockproc, 2000, 50, "procC", 2, "procC", mode);
        }
	resume(pA);
	resume(pB);
	resume(pC);
	sleep(7);
}

void task1() {
	int sem, lck;
	kprintf("Processes Execution using semaphores:\n");
	sem  = screate (1);
	createprocs(sem, 0);

	kprintf("\nProcesses Execution using Priority Inheritance:\n");
	lck = lcreate ();
	createprocs(lck, 1);
}
