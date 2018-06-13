/* ptsend.c - ptsend */

#include <xinu.h>

/*------------------------------------------------------------------------
 *  ptsend  -  Send a message to a port by adding it to the queue
 *------------------------------------------------------------------------
 */
syscall	ptsend(
	  int32		portid,		/* ID of port to use		*/
	  umsg32	msg,		/* Message to send		*/
		uint16 tag /* tag for message */
	)
{
	intmask	mask;			/* Saved interrupt mask		*/
	struct	ptentry	*ptptr;		/* Pointer to table entry	*/
	int32	seq;			/* Local copy of sequence num.	*/
	struct	ptnode	*msgnode;	/* Allocated message node 	*/
	struct	ptnode	*tailnode;	/* Last node in port or NULL	*/

	mask = disable();
	if ( isbadport(portid) ||
	     (ptptr= &porttab[portid])->ptstate != PT_ALLOC ) {
		restore(mask);
		return SYSERR;
	}

	/* Wait for space and verify port has not been reset */

	seq = ptptr->ptseq;		/* Record original sequence	*/
	if (wait(ptptr->ptmsem) == SYSERR
	    || ptptr->ptstate != PT_ALLOC
	    || ptptr->ptseq != seq) {
		restore(mask);
		return SYSERR;
	}
	if (ptfree == NULL) {
		panic("Port system ran out of message nodes");
	}

	/* Obtain node from free list by unlinking */

	msgnode = ptfree;		/* Point to first free node	*/
	ptfree  = msgnode->ptnext;	/* Unlink from the free list	*/
	msgnode->ptnext = NULL;		/* Set fields in the node	*/
	msgnode->ptmsg  = msg;
	msgnode->tag    = tag;

	/* Link into queue for the specified port */
	struct ptwaittag *curr = NULL, *prev = NULL;
	//Check if a waiting process matches the tag
	curr = ptptr->ptwaithead;

	//There is atleast 1 waiting process
	if (curr != NULL) {
		wait (ptptr->ptwaitmut);
		//There is only 1 waiting process
		if (curr->next == NULL) {
			if (curr->tag == tag || curr->tag == 0 || tag == 0) {
				curr->ptmsg = msg;
				ptptr->ptwaithead = NULL;
				signal(ptptr->ptwaitmut);
				resume(curr->pid);
				restore(mask);
				return OK;
			}
		}
		while (curr->next != NULL) {
			if (curr->tag == tag || curr->tag == 0 || tag == 0) {
				curr->ptmsg = msg;
				//Is first element in list
				if (prev == NULL) {
					ptptr->ptwaithead = curr->next;
				} else {
					prev->next = curr->next;
				}
				signal(ptptr->ptwaitmut);
				resume(curr->pid);
				restore(mask);
				return OK;
			}
			prev = curr;
			curr = curr->next;
		}
		signal(ptptr->ptwaitmut);
	}

	//Else queue the message
	tailnode = ptptr->pttail;
	if (tailnode == NULL) {		/* Queue for port was empty	*/
		ptptr->pttail = ptptr->pthead = msgnode;
	} else {			/* Insert new node at tail	*/
		tailnode->ptnext = msgnode;
		ptptr->pttail = msgnode;
	}

	signal(ptptr->ptmsem);
	restore(mask);
	return OK;
}
