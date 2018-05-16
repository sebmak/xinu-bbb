/* xsh_lab3.c - xshexit */

#include <xinu.h>

/*------------------------------------------------------------------------
 * xsh_lab3  -  Shell command to test lab 3
 *------------------------------------------------------------------------
 */
local sid32 sem;
local long double sum;

void doSomethingHeavy() {
  int i;
  sum = 1.1;
  for (i = 0; i < 10000000; i++) {
    sum/=2.5;
    sum*=2.5;
  }
}

void long_running () {
	for (;;) {
		doSomethingHeavy();
	}
}

void do_signal () {
  sleepms(3000);
	signal(sem);
}
void long_sleeping () {
	for (;;) {
		sleepms(10000);
	}
}

void long_waiting () {
	for (;;) {
		pid32 pid = create(do_signal, 1024, 40, "Signal", 0);
		set_malleable(pid);
		resume(pid);

		wait(sem);
		doSomethingHeavy();
	}
}

void dumpProctab();
void run_lab3();

shellcmd xsh_lab3(int nargs, char *args[])
{
	resume(create(run_lab3, 1024, 254, "Run Lab3", 0));
}

void run_lab3() {

	sem = semcreate(0);
	pid32 pid = create(long_running, 1024, 40, "Regular", 0);
	set_malleable(pid);
	resume(pid);

	pid = create(long_sleeping, 1024, 40, "Sleep", 0);
	set_malleable(pid);
	resume(pid);

	pid = create(long_waiting, 1024, 40, "Wait", 0);
	set_malleable(pid);
	resume(pid);

	resume(create(long_running, 1024, 50, "Fixed 50", 0));

	while (TRUE) {
		resume(create(dumpProctab, 1024, 255, "dumpproctab", 0));
		sleepms(100);
	}
}


char *prssb[] = {
"FREE ",
"CURR ",
"READY ",
"RECV ",
"SLEEP ",
"SUSP ",
"WAIT ",
"RECTIM",
};
void dumpProctab()
{
	int32 i;
	struct procent *p;
	kprintf("Process table dump:\n\n");
	kprintf("Num\tState\tPrio\tSem\tName\n");
	kprintf("----------------------------------");
	kprintf("----------------------------------\n");
	for (i = 0; i < NPROC; i++) {
		p = &proctab[i];
		if ((p->prstate != PR_FREE)) {
			kprintf("%d\t%6s\t%d\t%d\t%s\n", i, prssb[p->prstate],
			p->prprio, p->prsem, p->prname);
		}
	}
	kprintf("\n\n\n\n\n\n\n\n\n\n\n\n\n\n");
	kprintf("\n\n\n\n\n\n\n\n\n\n\n\n\n\n");
}
