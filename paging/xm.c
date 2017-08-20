/* xm.c = xmmap xmunmap */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <paging.h>


/*-------------------------------------------------------------------------
 *  * xmmap - xmmap
 *   *-------------------------------------------------------------------------
 *    */
SYSCALL xmmap(int virtpage, bsd_t source, int npages)
{
  STATWORD ps;
  if ((virtpage < NBPG) || (npages < 1) || (npages > NPAGES) || (source < 1) || (source >= MAXMAP))
  	return SYSERR;
  disable(ps);
  bsm_map(currpid, virtpage, source, npages);
  restore(ps);
  return (OK);
}



/*-------------------------------------------------------------------------
 *  * xmunmap - xmunmap
 *   *-------------------------------------------------------------------------
 *    */
SYSCALL xmunmap(int virtpage)
{
  STATWORD ps;
  if ((virtpage < NBPG))
  	return SYSERR;
  disable(ps);
  bsm_unmap(currpid, virtpage, 1);
  write_cr3(proctab[currpid].pdbr * NBPG);
  restore(ps);
  return (OK);
}
