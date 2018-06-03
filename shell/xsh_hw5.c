/* xsh_hw5.c - xshhw5 */

#include <xinu.h>

/*------------------------------------------------------------------------
 * xsh_hw5  -  shell command returns the hw5 test code
 *------------------------------------------------------------------------
 */

void send_messages ();
void recv_messages ();

sid32 mut, prod_sem, cons_sem;

shellcmd xsh_hw5(int nargs, char *args[])
{
	intmask	mask;			/* Saved interrupt mask		*/

	mut = semcreate(1);
	prod_sem = semcreate(1);
	cons_sem = semcreate(0);

	mask = disable();
	resched_cntl(DEFER_START);

	pid32 recv_pid;

	recv_pid = create(recv_messages, 1024, 30, "recv_messages", 0);

	resume(create(send_messages, 1024, 30, "send_messages", 1, recv_pid));
	resume(recv_pid);

	resched_cntl(DEFER_STOP);
	restore(mask);
	return OK;
}

void send_messages (pid32 pid) {
	wait(prod_sem);
	//Send 5 messages
	wait(mut);
	kprintf("Sending first messages\n");
	sendn(pid, 5, 'a','b','c','d','e');
	kprintf("Sent: %c\n", 'a');
	kprintf("Sent: %c\n", 'b');
	kprintf("Sent: %c\n", 'c');
	kprintf("Sent: %c\n", 'd');
	kprintf("Sent: %c\n", 'e');
	kprintf("First messages sent\n");
	signal(mut);

	signal(cons_sem);
	wait(prod_sem);

	//Send 5 messages
	wait(mut);

	kprintf("Sending Second messages\n");
	sendn(pid, 7, 'f','g','h','i','j','k','l');
	kprintf("Sent: %c\n", 'f');
	kprintf("Sent: %c\n", 'g');
	kprintf("Sent: %c\n", 'h');
	kprintf("Sent: %c\n", 'i');
	kprintf("Sent: %c\n", 'j');
	kprintf("Sent: %c\n", 'k');
	kprintf("Sent: %c\n", 'l');
	kprintf("Second messages sent\n");
	signal(mut);

	signal(cons_sem);
	wait(prod_sem);
	//Send more than NMSG
	wait(mut);
	kprintf("Sending Too many messages\n");
	int i;
	for (i = 0; i <= NMSG+5; i++) {
		if (send(pid, (umsg32)'n'+i) == SYSERR) {
			kprintf("Tried to send %c but MessageQueue is full for pid %d\n", (umsg32)'n'+i, pid);
		} else {
			kprintf("Sent: %c\n", (umsg32)'n'+i);
		};
	}
	kprintf("Too many messages sent.\n");
	signal(mut);
	signal(cons_sem);
}

void recv_messages () {
	umsg32 msg;
	int32 i;

	//Receive first set
	wait(cons_sem);
	wait(mut);
	kprintf("Receiving First Messages:\n");
	signal(mut);

	for (i = 0; i < 5; i++) {
		msg = receive();
		wait(mut);
		kprintf("Received: %c\n", msg);
		signal(mut);
	}
	signal(prod_sem);


	//Receive second set
	wait(cons_sem);
	wait(mut);
	kprintf("Receiving Second Messages:\n");
	signal(mut);

	for (i = 0; i < 7; i++) {
		msg = receive();
		wait(mut);
		kprintf("Received: %c\n", msg);
		signal(mut);
	}
	signal(prod_sem);

	//Receive last set
	wait(cons_sem);
	wait(mut);
	kprintf("Receiving Last Messages:\n");
	signal(mut);

	for (i = 0; i < NMSG; i++) {
		msg = receive();
		wait(mut);
		kprintf("Received: %c\n", msg);
		signal(mut);
	}
	signal(prod_sem);
}
