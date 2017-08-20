/* bsm.c - manage the backing store mapping*/

#include <conf.h>
#include <kernel.h>
#include <paging.h>
#include <proc.h>

/*-------------------------------------------------------------------------
 *  * init_bsm- initialize bsm_tab
 *   *-------------------------------------------------------------------------
 *    */
SYSCALL init_bsm()
{
	STATWORD ps;
	disable(ps);
	int i = 0;
	for (i = 0; i < NUMBS; i ++) {
		bsm_tab[i].bs_status = BSM_UNMAPPED;
	  	bsm_tab[i].bs_pid = -1;
	  	bsm_tab[i].bs_vpno = VPAGE_NUM;
	  	bsm_tab[i].bs_npages = 0;
		bsm_tab[i].bs_priv = 0;
		bsm_tab[i].bs_sem = 0;
		bsm_tab[i].bs_id = i;
	}
	restore(ps);
	return (OK);
}

/*-------------------------------------------------------------------------
 *  * get_bsm - get a free entry from bsm_tab 
 *   *-------------------------------------------------------------------------
 *    */
SYSCALL get_bsm(int* avail)
{
	STATWORD ps;
	disable(ps);
	int i = 0;
	for (i = 0; i < NUMBS; i ++) {
		if (bsm_tab[i].bs_status == BSM_UNMAPPED) {
			*avail = i;
			restore(ps);
			return (OK);
		}  	
	}
	restore(ps);
	return (OK);
}


/*-------------------------------------------------------------------------
 *  * free_bsm - free an entry from bsm_tab 
 *   *-------------------------------------------------------------------------
 *    */
SYSCALL free_bsm(int i)
{
	STATWORD ps;
	disable(ps);
	bsm_tab[i].bs_status = BSM_UNMAPPED;
  	bsm_tab[i].bs_pid = -1;
  	bsm_tab[i].bs_npages = 0;
	bsm_tab[i].bs_priv = 0;
	restore(ps);
	return (OK);
}

/*-------------------------------------------------------------------------
 *  * bsm_lookup - lookup bsm_tab and find the corresponding entry
 *   *-------------------------------------------------------------------------
 *    */
SYSCALL bsm_lookup(int pid, long vaddr, int* store, int* pageth)
{
	STATWORD ps;
	disable(ps);
	unsigned int pg_offset = vaddr/NBPG;
	int i;
	for (i = 0; i < NUMBS; i ++) {
		if (pid == bsm_tab[i].bs_pid) {
			if ((pg_offset < bsm_tab[i].bs_vpno) || (pg_offset > bsm_tab[i].bs_vpno + bsm_tab[i].bs_npages)) {
				return SYSERR;
			}
			*pageth = pg_offset - bsm_tab[i].bs_vpno;
			*store = i;
		}
	}
	restore(ps);
	return (OK);
}


/*-------------------------------------------------------------------------
 *  * bsm_map - add an mapping into bsm_tab 
 *   *-------------------------------------------------------------------------
 *    */
SYSCALL bsm_map(int pid, int vpno, int source, int npages)
{
	STATWORD ps;
	disable(ps);
	bsm_tab[source].bs_status = BSM_MAPPED;
  	bsm_tab[source].bs_pid = pid;
  	bsm_tab[source].bs_vpno = vpno;
  	bsm_tab[source].bs_npages = npages;
	bsm_tab[source].bs_priv = 1;
	restore(ps);
	return (OK);
}



/*-------------------------------------------------------------------------
 *  * bsm_unmap - delete an mapping from bsm_tab
 *   *-------------------------------------------------------------------------
 *    */
SYSCALL bsm_unmap(int pid, int vpno, int flag)
{
	STATWORD ps;
	disable(ps);
	struct pentry *pptr = &proctab[pid];
	int source = pptr->store;
	bsm_tab[source].bs_status = BSM_UNMAPPED;
  	bsm_tab[source].bs_pid = -1;
  	bsm_tab[source].bs_vpno = vpno;
  	bsm_tab[source].bs_npages = 0;
	bsm_tab[source].bs_priv = 0;
	bsm_tab[source].bs_sem = 0;
	restore(ps);
	return (OK);
}
