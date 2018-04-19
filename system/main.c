/*  main.c  - main */

#include <xinu.h>

void alice();
void bob();

pid32 APid, BPid;
sid32 asem, bsem, printmut;
char	buf[SHELL_BUFLEN];

void main(void)
{
	asem = semcreate(0);
	bsem = semcreate(0);
	printmut = semcreate(1);

	APid = create(alice, 1024, 20, "Alice", 0);
	BPid = create(bob,   1024, 40, "Bob",   0);

	resume(APid);
	resume(BPid);

	return OK;
}

void alice() {
	wait(printmut);
	kprintf("My first statement appears before Bob's second statement.\n");
	signal(printmut);

	signal(asem);
	wait(bsem);

	wait(printmut);
	kprintf("This is Alice's second Statement\n");
	signal(printmut);

	kill(APid);
}

void bob() {
	wait(printmut);
	kprintf("My first statement appears before Alice's second statement.\n");
	signal(printmut);

	signal(bsem);
	wait(asem);

	wait(printmut);
	kprintf("This is Bob's second Statement\n");
	signal(printmut);

	kill(BPid);
}
