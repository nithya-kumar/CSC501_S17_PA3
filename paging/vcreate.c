/* vcreate.c - vcreate */
    
#include <conf.h>
#include <i386.h>
#include <kernel.h>
#include <proc.h>
#include <sem.h>
#include <mem.h>
#include <io.h>
#include <paging.h>

/*
 * static unsigned long esp;
 * */

LOCAL	newpid();
/*------------------------------------------------------------------------
 *  *  create  -  create a process to start running a procedure
 *   *------------------------------------------------------------------------
 *    */
SYSCALL vcreate(procaddr,ssize,hsize,priority,name,nargs,args)
	int	*procaddr;		/* procedure address		*/
	int	ssize;			/* stack size in words		*/
	int	hsize;			/* virtual heap size in pages	*/
	int	priority;		/* process priority > 0		*/
	char	*name;			/* name (for debugging)		*/
	int	nargs;			/* number of args that follow	*/
	long	args;			/* arguments (treated like an	*/
					/* array in the code)		*/
{
	STATWORD ps;
	int process_id;
	int available;
	disable(ps);
	if (hsize > NPAGES) {
		restore(ps);
		return SYSERR;
	}
	if (get_bsm(&available) == SYSERR) {
		restore(ps);
		return SYSERR;
	}
	process_id = create(procaddr, ssize, priority, name, nargs, args);
	if (process_id == SYSERR) {
		restore(ps);
		return SYSERR;
	}
  	
	bsm_map(process_id, VPAGE_NUM, available, hsize);

	proctab[process_id].store = available;
	proctab[process_id].vhpno = 0;
	proctab[process_id].vhpnpages = hsize;
	proctab[process_id].vmemlist->mnext = NBPG * hsize;
	struct mblock *bs = BACKING_STORE_BASE + (available * BACKING_STORE_UNIT_SIZE);
	bs->mlen = NBPG*hsize;
	bs->mnext = NULL;

	restore(ps);
	return process_id;
}

/*------------------------------------------------------------------------
 *  * newpid  --  obtain a new (free) process id
 *   *------------------------------------------------------------------------
 *    */
LOCAL	newpid()
{
	int	pid;			/* process id to return		*/
	int	i;

	for (i=0 ; i<NPROC ; i++) {	/* check all NPROC slots	*/
		if ( (pid=nextproc--) <= 0)
			nextproc = NPROC-1;
		if (proctab[pid].pstate == PRFREE)
			return(pid);
	}
	return(SYSERR);
}
