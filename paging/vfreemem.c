/* vfreemem.c - vfreemem */

#include <conf.h>
#include <kernel.h>
#include <mem.h>
#include <proc.h>
#include <paging.h>

extern struct pentry proctab[];
/*------------------------------------------------------------------------
 *  *  vfreemem  --  free a virtual memory block, returning it to vmemlist
 *   *------------------------------------------------------------------------
 *    */
SYSCALL	vfreemem(block, size)
	struct	mblock	*block;
	unsigned size;
{
	struct	mblock	*memlist;
	struct	mblock *prev;
	struct	mblock *next;
	struct pentry *pptr;
	STATWORD ps;
	if (block < VPAGE_NUM * NBPG || size == 0) 
		return SYSERR;
	disable(ps);
	pptr = &proctab[currpid];
	memlist = &pptr->vmemlist;
	prev = memlist;
	next = prev->mnext;
	while (next != NULL && next < block) {
		prev = next;
		next = next->mnext;
	}
	block->mnext = next;
	block->mlen = size;
	prev->mnext = block;
	restore(ps);
	return(OK);
}

