/* frame.c - manage physical frames */
#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <paging.h>

/*-------------------------------------------------------------------------
 *  * init_frm - initialize frm_tab
 *   *-------------------------------------------------------------------------
 *    */
 bs_map_t bsm_tab[NUMBS];
 fr_map_t frm_tab[NUMBS];
 struct fifo_queue {
  int fr_no;
  struct fifo_queue* fr_next;
  struct fifo_queue* fr_prev;
};
struct fifo_queue* fq_ptr;
SYSCALL init_frm()
{
  STATWORD ps;
  disable (ps);
  int i = 0;
  fq_ptr = NULL;
  for (i = 0; i < NFRAMES; i ++) {
    frm_tab[i].fr_status = FRM_UNMAPPED;
  	frm_tab[i].fr_pid = -1;
  	frm_tab[i].fr_vpno = VPAGE_NUM;
    frm_tab[i].fr_refcnt = 0;
    frm_tab[i].fr_type = -1;
    frm_tab[i].fr_dirty = 0;
  }
  restore(ps);
  return OK;
}

/*-------------------------------------------------------------------------
 *  * get_frm - get a free frame according page replacement policy
 *   *-------------------------------------------------------------------------
 *    */
SYSCALL get_frm(int* avail)
{
  STATWORD ps;
  disable (ps);
  int i;
  int flag = 0;
  struct fifo_queue* frm_avail;
  for (i = 0; i < NFRAMES; i++) {
    if (frm_tab[i].fr_status == FRM_UNMAPPED) {
      *avail = i;
      return (OK);
    }
  }

  for(i = 0; i< NFRAMES; i++)
  {
    if(frm_tab[i].fr_status == FRM_UNMAPPED)
    {
      *avail = i;
      return OK;
    }
  }
  i = free_frame(*avail, &flag);
  if (i == SYSERR) {
    restore(ps);
    return SYSERR;
  }
  *avail = i;
  if (*avail == FR_PAGE && flag == 0) {
    frm_avail = (struct fifo_queue*)getmem(sizeof(struct fifo_queue));
    if(frm_avail == NULL) {
      restore(ps);
      return(SYSERR);
    }
    frm_avail->fr_prev = NULL;
    frm_avail->fr_next = NULL;
    frm_avail->fr_no = i;
    if (fq_ptr == NULL) {
      fq_ptr = frm_avail;
      fq_ptr->fr_prev = fq_ptr;
      fq_ptr->fr_next = fq_ptr;
    } else {
      frm_avail->fr_prev = fq_ptr->fr_prev;
      frm_avail->fr_next = fq_ptr;  
      fq_ptr->fr_prev->fr_next = frm_avail;
      fq_ptr->fr_prev = frm_avail;
    }
  }
  restore (ps);
  return (OK);
}

/*-------------------------------------------------------------------------
 *  * free_frm - free a frame 
 *   *-------------------------------------------------------------------------
 *    */
SYSCALL free_frm(int i)
{
  return OK;
}




