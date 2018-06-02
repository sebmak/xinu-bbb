/* send.c - send */

#include <xinu.h>


umsg32 get_message(pid32 pid) {
	intmask	mask;			/* Saved interrupt mask		*/
	struct	procent *prptr;		/* Ptr to process's table entry	*/
  struct Message* m = NULL;
  umsg32 msg = NULL;

	mask = disable();

  if (isbadpid(pid)) {
    restore(mask);
    return SYSERR;
  }

  prptr = &proctab[pid];
  if (prptr->prmsg != NULL) { //If there is a message
    m = prptr->prmsg; //Get first message
    msg = m->message; //Set the return message to the message value of m
    prptr->prmsg = m->next; //Set prmsg to the next message.
    free(m); //Delete the Message m.
  }

  if (prptr->prmsg == NULL) {
    prptr->prhasmsg = TRUE;
  } else {
    prptr->prhasmsg = FALSE;
  }

	restore(mask);		/* Restore interrupts */
	return msg;
}

status add_message(pid32 pid, umsg32 message) {
	intmask	mask;			/* Saved interrupt mask		*/
	struct	procent *prptr;		/* Ptr to process's table entry	*/
  struct Message* new_msg = NULL;
  int i = 0;

	mask = disable();

  if (isbadpid(pid)) {
    restore(mask);
    return SYSERR;
  }

  prptr = &proctab[pid];

  //Create the new message
  new_msg = (struct Message*)malloc(sizeof(struct Message));
  new_msg->message = message;
  new_msg->next = NULL;

  //If there is currently not a message
  if (prptr->prmsg == NULL) {
    prptr->prmsg = new_msg; //Set the first message to the new message
  } else {
    struct Message* cur_msg = prptr->prmsg; //Get the pointer to the first message
    i = 1; //Set the message counter to 1
    while (cur_msg->next != NULL) {
      cur_msg = cur_msg->next; //Get the next message in the list
      if (++i > NMSG) { //If there is currently more messages in the list than NMSG throw error
        free(new_msg); //Free memory for new message
        return SYSERR;
      }
    }
    cur_msg->next = new_msg; //Add the new message to the message list
  }

  if (prptr->prmsg == NULL) {
    prptr->prhasmsg = TRUE;
  } else {
    prptr->prhasmsg = FALSE;
  }

	restore(mask);		/* Restore interrupts */
	return OK;
}
