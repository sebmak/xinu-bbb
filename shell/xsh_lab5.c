/* xsh_lab5.c - xshlab5 */

#include <xinu.h>

/*------------------------------------------------------------------------
 * xsh_lab5  -  shell command returns the lab5 test code
 *------------------------------------------------------------------------
 */

process proc1(int32);
process proc2(int32);
process proc3(int32);
sid32   printsem;

shellcmd xsh_lab5(int nargs, char *args[])
{
	int32 port;
	intmask	mask;			/* Saved interrupt mask		*/

	printsem = semcreate(1);

	if (ptinit(100) != OK) {
		kprintf("Failed to initialize ports\n");
		return OK;
	};

	if ((port = ptcreate(10)) == SYSERR) {
		kprintf("Failed to create port\n");
		return OK;
	};
	mask = disable();
	resched_cntl(DEFER_START);

	wait(printsem);
	kprintf("Starting Receiver Processes\n");
	signal(printsem);

	resume(create(proc2, 1024, 30, "process 2", 1, port));

	restore(mask);
	resched_cntl(DEFER_STOP);

	kprintf("Port SemCount %d\n", ptcount(port));

	resume(create(proc1, 1024, 30, "process 1", 1, port));

	return OK;
}



/* assume initialization already done */
process proc1(int32 port)
{
	wait(printsem);
	kprintf("Sending Messages\n");
	signal(printsem);

	ptsend(port, (umsg32) "Message 1", (uint16) 'A');
	ptsend(port, (umsg32) "Message 2", (uint16) 'A');
	ptsend(port, (umsg32) "DO NOT READ", (uint16) 'B');
	ptsend(port, (umsg32) "DO NOT READ", (uint16) 'B');
	ptsend(port, (umsg32) "Wildcard", (uint16) 0);
	ptsend(port, (umsg32) "DO NOT READ", (uint16) 'B');
	ptsend(port, (umsg32) "Message 3", (uint16) 'A');
	ptsend(port, (umsg32) "Message 4", (uint16) 'A');

	wait(printsem);
	kprintf("Messages Sent\n");
	signal(printsem);

	return OK;
}

process proc2(int32 port)
{
	wait(printsem);
	kprintf("Receiving A\n");
	signal(printsem);
	char *msg;
	while (TRUE) {
		msg = (char *) ptrecv(port, 'A');
		wait(printsem);
		printf("Received: %s\n", msg);
		signal(printsem);
	}
	return OK;
}
