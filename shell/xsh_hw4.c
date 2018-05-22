/* xsh_hw4.c - xshexit */

#include <xinu.h>

/*------------------------------------------------------------------------
 * xsh_hw4  -  shell command returns the exit code causing shell exit
 *------------------------------------------------------------------------
 */

void A(pid32);
void B();
void C();

shellcmd hw4_pt1();
shellcmd hw4_pt2();

shellcmd xsh_hw4(int nargs, char *args[])
{
	if (nargs < 2) {
		printf("You must enter a command: (pt1, pt2)\n");
		return OK;
	}

	if (strncmp(args[1], "pt1", 3 ) == 0) {
		hw4_pt1();
		return OK;
	}
	if (strncmp(args[1], "pt2", 3) == 0) {
		hw4_pt2();
		return OK;
	}

	printf("You must enter a valid command: (pt1, pt2)\n");
	return OK;
}

void pt1_wait();
void pt1_resume(pid32);

pri16 aprio = 25;
pri16 bprio = 26;
int shouldEnd = 0;

shellcmd hw4_pt1() {
	intmask	mask;
	pid32 waitpid;

	mask = disable();
	resched_cntl(DEFER_START);
	waitpid = create(pt1_wait, 1024, 30, "pt1_wait", 0);
	resume(waitpid);
	resume(create(pt1_resume, 1024, aprio, "event a", 1, waitpid));
	resume(create(pt1_resume, 1024, bprio, "event b", 1, waitpid));

	resched_cntl(DEFER_STOP);
	restore(mask);
	return OK;
}

void pt1_wait () {
	pri16 newprio;

	newprio = suspend(getpid());

	if (newprio == aprio) {
		printf("Was resumed by event A\n");
	} else if (newprio == bprio) {
		printf("Was resumed by event B\n");
	}
}

void pt1_resume (pid32 pid) {
	intmask	mask;
	mask = disable();
	resched_cntl(DEFER_START);

	if (proctab[pid].prstate == PR_SUSP) {
		proctab[pid].prprio = proctab[getpid()].prprio;
		resume(pid);
	}

	resched_cntl(DEFER_STOP);
	restore(mask);
}

shellcmd hw4_pt2() {
	printf("PART 2\n");
	pid32 aPid,bPid,cPid;
	shouldEnd = 0;

	bPid = create(B, 1024, 30, "HW4_B", 0);
	cPid = create(C, 1024, 30, "HW4_C", 0);
	aPid = create(A, 1024, 30, "HW4_A", 1, cPid);

	resumen(3, cPid, bPid, aPid);

	return OK;
}

void KillC (pid32 cpid) {
	kill(cpid);
}

void A (pid32 cpid) {
	int i;
	kprintf("A\n");
	suspend(cpid);
	for (i = 0; i < 10; i++) {
		kprintf("A\n");
	}
	shouldEnd = 1;
	resume(create(KillC, 1024, 21, "Kill C", 1, cpid));
	return;
}
void B () {
	for (;;) {
		kprintf("B\n");
		if (shouldEnd) {
			return;
		}
	}
}
void C () {
	for (;;) {
		kprintf("C\n");
	}
}
