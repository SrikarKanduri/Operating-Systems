/* lock.h - isbadlock */

#ifndef _LOCK_H_
#define _LOCK_H_

#ifndef NLOCKS
#define	NLOCKS		50	/* number of locks, if not defined	*/
#endif

#define LFREE	'\01'		/* this lock is free			*/
#define LUSED	'\02'		/* this lock is used			*/
#define	READ	'\03'		/* this lock is used for reading	*/
#define WRITE	'\04'		/* this lock is used for writing	*/

struct	lentry	{		/* locks table entry			*/
	char	lstate;		/* the state LFREE/LUSED/READ/WRITE	*/
	int	nr;		/* no of readers holding the lock	*/
	int	lqhead;		/* q index of head of list		*/
	int	lqtail;		/* q index of tail of list		*/
	int	lprio;		/* max priority of waiting processes	*/
	int	lholders[NPROC];/* processes holding for the lock	*/
};
extern	struct	lentry	locks[];
extern	int	nextlock;

#define	isbadlock(l)	(l<0 || l>=NLOCKS)
#endif

void linit();
int lcreate(void);
int lock(int, int, int);
int ldelete(int);
