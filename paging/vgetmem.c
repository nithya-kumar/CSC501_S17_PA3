/* vgetmem.c - vgetmem */

#include <conf.h>
#include <kernel.h>
#include <mem.h>
#include <proc.h>
#include <paging.h>

extern struct pentry proctab[];
/*------------------------------------------------------------------------
 *  * vgetmem  --  allocate virtual heap storage, returning lowest WORD address
 *   *------------------------------------------------------------------------
 *    */
WORD	*vgetmem(nbytes)
	unsigned nbytes;
{

	STATWORD ps;
	
	struct pentry *pptr = &proctab[currpid];
	disable(ps);
	if (nbytes == 0) {
		restore(ps);
		return ((WORD*) SYSERR);
	}
	struct mblock *memlist, *prev, *next, *hp;
	memlist = &(pptr->vmemlist);
	if (memlist->mnext == NULL) {
		restore(ps);
		return ((WORD*) SYSERR);
	}
	prev = memlist;
	next = memlist->mnext;

	while (next != NULL) {
		if (next->mlen == nbytes) {
			prev->mnext = next->mnext;
			restore(ps);
			return next;
		}
		else if (next->mlen > nbytes) {
			hp = next + nbytes;
			hp->mlen = next->mlen - nbytes;
			hp->mnext = next->mnext;
			prev->mnext = hp;
			restore(ps);
			return next;
		}
		prev= next;
		next = next->mnext;
	}
	restore(ps);
	return ((WORD*) SYSERR);
}
