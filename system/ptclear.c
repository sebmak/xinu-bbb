/* ptclear.c - _ptclear */

#include <xinu.h>

/*------------------------------------------------------------------------
 *  _ptclear  -  Used by ptdelete and ptreset to clear or reset a port
 *		   (internal function assumes interrupts disabled and
 *		   arguments have been checked for validity)
 *------------------------------------------------------------------------
 */
void	_ptclear(
	  struct ptentry *ptptr,	/* Table entry to clear		*/
	  uint16       newstate,	/* New state for port		*/
	  int32        (*dispose)(int32)/* Disposal function to call	*/
	 )
{
	struct	ptnode	*walk;		/* Pointer to walk message list	*/

	/* Place port in limbo state while waiting processes are freed */

	ptptr->ptstate = PT_LIMBO;

	ptptr->ptseq++;			/* Reset accession number	*/
	walk = ptptr->pthead;		/* First item on msg list	*/

	if ( walk != NULL ) {		/* If message list nonempty	*/

		/* Walk message list and dispose of each message */

		for( ; walk!=NULL ; walk=walk->ptnext) {
                        (*dispose)( walk->ptmsg );
		}

		/* Link entire message list into the free list */

                (ptptr->pttail)->ptnext = ptfree;
                ptfree = ptptr->pthead;
        }

	if (newstate == PT_ALLOC) {
		ptptr->pttail = ptptr->pthead = NULL;
	}

	struct ptwaittag *curr;
	for (curr = ptptr->ptwaithead; curr != NULL; curr = curr->next) {
		resume(curr->pid);
	}
	ptptr->ptwaithead = NULL;

	ptptr->ptstate = newstate;
	return;
}
