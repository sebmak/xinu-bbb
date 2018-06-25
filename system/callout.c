/* callout.c - callout */

#include <xinu.h>

/*------------------------------------------------------------------------
 *  chprio  -  Change the scheduling priority of a process
 *------------------------------------------------------------------------
 */

uint32 nextcid = 0;
struct callout *callout_list;
sid32 mut;

void init_callouts () {
	callout_list = NULL;
  mut = semcreate(1);
}

uint32 add_callout(uint32 msdelay, void *funcaddr, void *argp) {
  wait(mut);
  struct callout *curr;
  struct callout *newcallout = (struct callout*)getmem(sizeof(struct callout));

  newcallout->time = ((clktime * 1000) + count1000) + msdelay;
  newcallout->funcaddr = funcaddr;
  newcallout->argp = argp;
  newcallout->cid = ++nextcid;
  newcallout->cnext = NULL;
  newcallout->cprev = NULL;

	curr = callout_list;
  if (curr == NULL) {
    callout_list = newcallout;
  } else {
  	while (curr && curr->cnext != NULL && curr->time <= newcallout->time) {
  		curr = curr->cnext;
  	}

    newcallout->cnext = curr->cnext;
    curr->cnext = newcallout;
    newcallout->cprev = curr;

    if (newcallout->cnext != NULL) {
      newcallout->cnext->cprev = newcallout;
    }
  }

  signal(mut);
  return newcallout->cid;
}

void remove_callout(uint32 cid) {
	kprintf("Removing callouts\n");
  wait(mut);
  struct	callout	*curr;

  curr = callout_list;
  while (curr && curr->cid != cid) {
    curr = curr->cnext;
  }

  if (curr && curr->cid == cid) {
    if (curr->cprev != NULL) {
      curr->cprev->cnext = curr->cnext;
    }
    if (curr->cnext != NULL) {
      curr->cnext->cprev = curr->cprev;
    }

		if (curr == callout_list) {
			callout_list = curr->cnext;
		}
    freemem((char *)curr, sizeof(struct callout));
  }
	kprintf("Done Removing\n");
  signal(mut);
}

void run_callouts() {
  wait(mut);
  struct	callout	*curr;
  kprintf("Running callouts\n");
	intmask	mask;			/* Saved interrupt mask		*/
	mask = disable();
	resched_cntl(DEFER_START);

  curr = callout_list;
  while (curr && curr->time <= (clktime*1000)+count1000) {
    resume(create(curr->funcaddr, 1024, 30, "Running Callout", 1, curr->argp));
		signal(mut);
    remove_callout(curr->cid);
		wait(mut);
  }
	kprintf("Done Running\n");

	restore(mask);
	resched_cntl(DEFER_STOP);
  signal(mut);
}
