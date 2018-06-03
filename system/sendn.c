/* sendn.c - sendn */

#include <xinu.h>
#include <stdarg.h>

/*------------------------------------------------------------------------
 *  sendn  -  Pass multiple messages to a process and start recipient if waiting
 *------------------------------------------------------------------------
 */
syscall	sendn(
	  pid32		pid,		/* ID of recipient process	*/
	  int32	count,		/* number of messages */
		...             /* messages */
	)
{
	intmask	mask;			/* Saved interrupt mask		*/
	struct	procent *prptr;		/* Ptr to process's table entry	*/
	umsg32 currmsg;
	int i;

	va_list args;

	mask = disable();
	va_start(args, count);
	for (i = 0; i < count; ++i) {
		currmsg = va_arg(args, umsg32);
		if (add_message(pid, currmsg) == SYSERR) {
			restore(mask);
			return SYSERR;
		}
	}
	va_end(args);

	prptr = &proctab[pid];

	/* If recipient waiting or in timed-wait make it ready */
	if (prptr->prstate == PR_RECV) {
		ready(pid);
	} else if (prptr->prstate == PR_RECTIM) {
		unsleep(pid);
		ready(pid);
	}

	restore(mask);		/* Restore interrupts */
	return OK;
}
