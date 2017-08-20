#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <paging.h>

int get_bs(bsd_t bs_id, unsigned int npages) {

  /* requests a new mapping of npages with ID map_id */
    STATWORD ps;
	disable(ps);
	if (bs_id < 0 || bs_id > MAXMAP || npages < 0 || npages > NPAGES) {
		restore(ps);
		return SYSERR;
	}
	if (bsm_tab[bs_id].bs_status == BSM_MAPPED) {
		if (bsm_tab[bs_id].bs_priv != 1 && bsm_tab[bs_id].bs_sem != 1) {
			bsm_tab[bs_id].bs_status = BSM_MAPPED;
		  	bsm_tab[bs_id].bs_pid = currpid;
		  	bsm_tab[bs_id].bs_npages = npages;
			bsm_tab[bs_id].bs_priv = 0;
			bsm_tab[bs_id].bs_sem = 0;
			restore(ps);
			npages = bsm_tab[bs_id].bs_npages;
		}
		restore(ps);
		return SYSERR;
	} else {
		bsm_tab[bs_id].bs_status = BSM_MAPPED;
	  	bsm_tab[bs_id].bs_pid = currpid;
	  	bsm_tab[bs_id].bs_npages = npages;
		bsm_tab[bs_id].bs_priv = 0;
		bsm_tab[bs_id].bs_sem = 0;
		restore(ps);
		npages = bsm_tab[bs_id].bs_npages;
	}
	restore(ps);
    return npages;
}
