/* ptrecv.c - ptrecv */

#include <xinu.h>

/*------------------------------------------------------------------------
 *  ptrecv  -  Receive a message from a port, blocking if port empty
 *------------------------------------------------------------------------
 */
umsg32 get_tagged_message (struct ptentry *ptptr, uint16 tag) {
	int32	seq;			/* Local copy of sequence num.	*/
	umsg32	msg = NULL;			/* Message to return		*/

	seq = ptptr->ptseq;		/* Record orignal sequence	*/

  wait(ptptr->ptmsem);
	//If there is atleast 1 element
	if (ptptr->pthead != NULL) {
		//If there is only 1 element
		if (ptptr->pthead == ptptr->pttail) {
			if (ptptr->pthead->tag == 0 || ptptr->pthead->tag == tag || tag == 0) {
				msg = ptptr->pthead->ptmsg;
				ptptr->pthead->ptnext = ptfree;
				ptfree = ptptr->pthead;
				ptptr->pthead = ptptr->pttail = NULL;
			}
		} else {
			struct	ptnode	*msgnode, *lastnode;	/* First node on message list	*/

			lastnode = ptptr->pthead;
			msgnode = ptptr->pthead->ptnext;

			if (lastnode->tag == 0 || lastnode->tag == tag || tag == 0) {
				//Set message
				msg = lastnode->ptmsg;
				//Remove the first message
				ptptr->pthead = msgnode;
				//Set the last node next to free node
				lastnode->ptnext = ptfree;
				//Set freenode list to point to last node
				ptfree = lastnode;
			} else {
				while (msgnode != NULL) {
					if (msgnode->tag == 0 || msgnode->tag == tag || tag == 0) {
						//Set message
						msg = msgnode->ptmsg;

						if (msgnode == ptptr->pttail) {
							ptptr->pttail = lastnode;
							lastnode->ptnext = NULL;
						} else {
							lastnode->ptnext = msgnode->ptnext;
						}
						//Free the used message
						msgnode->ptnext = ptfree;
						ptfree = msgnode;
						break;
					}

					lastnode = msgnode;
					msgnode = msgnode->ptnext;
				};
			}
		}
	}
	//If there is not a message
	if (msg == NULL) {
    wait(ptptr->ptwaitmut);
		struct ptwaittag *waittag = (struct ptwaittag*)getmem(sizeof(struct ptwaittag));
		waittag->pid = currpid;
		waittag->tag = tag;
		waittag->ptmsg = NULL;
		waittag->next = NULL;

		if (ptptr->ptwaithead == NULL) {
			ptptr->ptwaithead = waittag;
		} else {
			struct ptwaittag * curr;
			curr = ptptr->ptwaithead;
			while (curr->next != NULL) {
				curr = curr->next;
			}
			curr->next = waittag;
		}
    signal(ptptr->ptwaitmut);

    if (waittag->ptmsg == NULL) {
  		//Suspend the process and wait for a message
  		suspend(currpid);
    }
		msg = waittag->ptmsg;
		freemem((char *)waittag, sizeof(struct ptwaittag));

		if (
			msg == NULL
			|| ptptr->ptstate != PT_ALLOC
			|| ptptr->ptseq != seq
		) {
			return (umsg32)SYSERR;
		}
	}
	return msg;
}


uint32	ptrecv(
	  int32		portid,		/* ID of port to use		*/
		uint16 tag        /* tag for message */
	)
{
	intmask	mask;			/* Saved interrupt mask		*/
	struct	ptentry	*ptptr;		/* Pointer to table entry	*/
	umsg32	msg;			/* Message to return		*/

	mask = disable();
	if ( isbadport(portid) ||
	     (ptptr= &porttab[portid])->ptstate != PT_ALLOC ) {
		restore(mask);
		return (uint32)SYSERR;
	}

	/* Wait for message and verify that the port is still allocated */

	if (
		(msg = get_tagged_message(ptptr, tag)) == (umsg32)SYSERR
	) {
		restore(mask);
		return (uint32)SYSERR;
	}

	restore(mask);
	return msg;
}
