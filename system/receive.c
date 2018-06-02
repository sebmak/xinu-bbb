/* receive.c - receive */

#include <xinu.h>

/*------------------------------------------------------------------------
 *  receive  -  Wait for a message and return the message to the caller
 *------------------------------------------------------------------------
 */
umsg32	receive(void)
{
	intmask	mask;			/* Saved interrupt mask		*/
	struct	procent *prptr;		/* Ptr to process's table entry	*/
	umsg32	msg;			/* Message to return		*/

	mask = disable();

	while ((msg = get_message(pid)) == NULL) {
		prptr->prstate = PR_RECV;
		resched();		/* Block until message arrives	*/
	}

	restore(mask);
	return msg;
}
