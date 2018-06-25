/* callout.h */

struct callout {
  uint32 time;
  void *funcaddr;
  void *argp;
  uint32 cid;

	struct	callout	*cnext;	/* Pointer to next callout on list	*/
	struct	callout	*cprev;	/* Pointer to prev callout on list	*/
};

extern struct callout *callout_list;	/* List of pending callouts		*/
