/* resume.c - resume */

#include <xinu.h>
#include <stdarg.h>

/*------------------------------------------------------------------------
 *  resumen  -  Unsuspend multiple processes, making it ready
 *------------------------------------------------------------------------
 */
syscall	resumen(
		int32 count,
	  ...		/* IDs or processes to unsuspend	*/
	)
{
  va_list args;
	intmask	mask;			/* Saved interrupt mask		*/
	int32 i;
	pid32 currpid;

	mask = disable();
	resched_cntl(DEFER_START);

  va_start(args, count);
  for (i = 0; i < count; ++i) {
		currpid = va_arg(args, pid32);
		if (resume(currpid) == (pri16)SYSERR) {
			return SYSERR;
		};
  }

  va_end(args);

	resched_cntl(DEFER_STOP);
	restore(mask);
	return OK;
}
