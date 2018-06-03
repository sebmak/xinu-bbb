/* recvclr.c - recvclr */

#include <xinu.h>

/*------------------------------------------------------------------------
 *  recvclr  -  Clear an incoming message, and return message if one waiting
 *------------------------------------------------------------------------
 */
umsg32	recvclr(void)
{
	intmask	mask;			/* Saved interrupt mask		*/
	umsg32	msg;			/* Message to return		*/

	mask = disable();

	msg = get_message(currpid);
	if (msg == NULL) {
		msg = OK;
	}

	restore(mask);
	return msg;
}
