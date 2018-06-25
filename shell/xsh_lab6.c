/* xsh_lab6.c - xsh_lab6 */

#include <xinu.h>

/*------------------------------------------------------------------------
 * xsh_lab6  -  shell command to run lab6
 *------------------------------------------------------------------------
 */

struct printf_args {
 uint32 message;
};

void callout_printf(struct printf_args *argp) {
 kprintf("Message %d Ran\n", argp->message);
}

shellcmd xsh_lab6(int nargs, char *args[])
{
	struct printf_args zero;
	struct printf_args one;
	uint32 id;

	zero.message = 0;
	add_callout(200, callout_printf, &zero);

  one.message = 1;
	id = add_callout(200, callout_printf, &one);
	remove_callout(id);

	return SHELL_EXIT;
}
