/* pfint.c - pfint */

#include <conf.h>
#include <kernel.h>
#include <paging.h>
#include <proc.h>

extern int page_replace_policy;
/*-------------------------------------------------------------------------
 *  * pfint - paging fault ISR
 *   *-------------------------------------------------------------------------
 *    */
SYSCALL pfint()
{
  STATWORD ps;
  disable (ps);
  unsigned long v_addr;
  unsigned int pg_offset;
  unsigned int pt_offset;
  unsigned int pd_offset;
  unsigned long pdbr;
  virt_addr_t *virt_addr;
  int new_frm;
  pt_t *pt_tab;
  pd_t *pd_tab;
  int pt_new;
  int bs;
  int bs_pg_offset;

  v_addr = read_cr2();
  virt_addr = (virt_addr_t *)&v_addr;
  pg_offset = virt_addr->pg_offset;
  pt_offset = virt_addr->pt_offset;
  pd_offset = virt_addr->pd_offset;
  pdbr = proctab[currpid].pdbr;
  pd_tab = pdbr + pd_offset*sizeof(pd_t);

  /* If the page fault occurs due to the non-presence of page table,
 * 	a new page table is created and the directory tab is updated */
  if(pd_tab->pd_pres == 0) {
	  	pt_new = create_page_table();
	  	if (pt_new == -1) {
				kill(currpid);
				restore(ps);
				return SYSERR;
		}
		pd_tab->pd_pres = 1;
		pd_tab->pd_write = 1;
		pd_tab->pd_user = 0;
		pd_tab->pd_pwt = 0;
		pd_tab->pd_pcd = 0;
		pd_tab->pd_acc = 0;
		pd_tab->pd_mbz = 0;
		pd_tab->pd_fmb = 0;
		pd_tab->pd_global = 0;
		pd_tab->pd_avail = 0;
		pd_tab->pd_base = pt_new + FRAME0;
		frm_tab[pt_new].fr_type = FR_TBL;
		frm_tab[pt_new].fr_status = FRM_MAPPED;
		frm_tab[pt_new].fr_pid = currpid;
  }
  pt_tab = (pt_t*)(pd_tab->pd_base * NBPG + pt_offset*sizeof(pt_t));
  if(pt_tab->pt_pres != 1) {
  	if(bsm_lookup(currpid,v_addr,&bs,&bs_pg_offset) == SYSERR) {
		kill(currpid);
		restore(ps);
		return SYSERR;
	}

	if(get_frm(&new_frm)==SYSERR)
	{
		kill(currpid);
		restore(ps);
		return SYSERR;
	}
	pt_tab->pt_pres = 1;
	pt_tab->pt_write = 1;
	pt_tab->pt_base = (FRAME0 + new_frm);
	frm_tab[pd_tab->pd_base - FRAME0].fr_refcnt++;
	frm_tab[new_frm].fr_vpno = v_addr/NBPG;	
	frm_tab[new_frm].fr_pid = currpid;
	frm_tab[new_frm].fr_type = FR_PAGE;
	frm_tab[new_frm].fr_status = FRM_MAPPED;

	read_bs((char*)((FRAME0+new_frm)*NBPG),bs,bs_pg_offset);

	/*if(page_replace_policy == AGING)
 * 		fifo_insert_frm(new_frm);
 * 			else if(page_replace_policy == SC)
 * 					circ_insert_frm(new_frm);*/
  }
  write_cr3(pdbr);
  restore(ps);
  return OK;
}

int create_page_table() {
	int i;
	unsigned int fr_addr;
	int fr_no;
	pt_t *pt_tab;

	if(get_frm(&fr_no) == SYSERR) {
		return -1;
	}

	fr_addr = (FRAME0 + fr_no)*NBPG;

	pt_tab = (pt_t*)fr_addr;

	for(i = 0;i < NFRAMES; i++)
	{
		pt_tab[i].pt_pres = 0;
		pt_tab[i].pt_write = 0;
		pt_tab[i].pt_user = 0;
		pt_tab[i].pt_pwt = 0;
		pt_tab[i].pt_pcd = 0;
		pt_tab[i].pt_acc = 0;
		pt_tab[i].pt_dirty = 0;
		pt_tab[i].pt_mbz = 0;
		pt_tab[i].pt_global = 0;
		pt_tab[i].pt_avail = 0;
		pt_tab[i].pt_base = 0;
	}
	return fr_no;
}


