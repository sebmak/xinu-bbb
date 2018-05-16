/* clkhandler.c - clkhandler */
#include <xinu.h>

#ifndef MAX
#define MAX(X,Y) ((X) > (Y) ? (X) : (Y))
#endif
#ifndef MIN
#define MIN(X,Y) ((X) < (Y) ? (X) : (Y))
#endif

/*-----------------------------------------------------------------------
 * reprioritize - Reprioritized malleable processes
 *-----------------------------------------------------------------------
 */
void reprioritize () {
	int32 i;
	struct	procent	*prptr;		/* Ptr to process table entry	*/

	/* Dequeue everything in the readylist */
	while (dequeue(readylist) != EMPTY);

	/* For all processes */
	for (i = 0; i < NPROC; i++) {
		prptr = &proctab[i];
		/* If the process should be in the readyqueue */
    if (prptr->prstate == PR_READY) {

			/* If it is malleable possibly change it's priority */
  		if (prptr->malleable == TRUE) {
  			switch (prptr->cpuusage) {
  				case CPU_NONE:
  					/* Increment Priority */
  					chprio(i, MIN(255, prptr->prprio + 1));
  					break;
  				case CPU_ALL:
  					/* Cut priority in half */
  					chprio(i, MAX(0, prptr->prprio / 2));
  					break;
  			}
      }

			/* Insert it into the now emptry readylist */
			insert(i, readylist, prptr->prprio);
		}
		prptr->cpuusage = CPU_NONE;
	}
}
