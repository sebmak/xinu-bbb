/* malleable.c - set_malleable, clear_malleable, toggle_malleable */

#include <xinu.h>

int32 set_malleable (pid32 pid) {
  struct	procent	*prptr;		/* Ptr to process table entry	*/
  prptr = &proctab[pid];
  prptr->malleable = TRUE;
  return OK;
}

int32 clear_malleable(pid32 pid) {
  struct	procent	*prptr;		/* Ptr to process table entry	*/
  prptr = &proctab[pid];
  prptr->malleable = FALSE;
  return OK;
}
