#include <cos_component.h>
#include <print.h>
#include <sched.h>
#include <evt.h>
#include <periodic_wake.h>
#include <timed_blk.h>
#include <evt.h>

#include <mem_mgr_large.h>
#include <valloc.h>

#include <c3_test.h>

int ec3_ser3_pass(long id)
{
	/* printc("\n*** trigger *****\n"); */
	/* printc("\n(ser 3) thread %d is triggering event %ld\n", cos_get_thd_id(), id); */
	evt_trigger(cos_spd_id(), id);

	return 0;
}

vaddr_t ec3_ser3_test(void)
{
	/* do not return valloc address */
	vaddr_t ret = (vaddr_t)valloc_alloc(cos_spd_id(), cos_spd_id(), 1);
	return ret;
}


void cos_upcall_fn(upcall_type_t t, void *arg1, void *arg2, void *arg3)
{
	switch (t) {
	case COS_UPCALL_REBOOT:
	{
		printc("thread %d passing arg1 %p here (type %d spd %ld)\n", 
		       cos_get_thd_id(), arg1, t, cos_spd_id());
		break;
	}
/****************************
 _ __ ___  _ __ ___  
| '_ ` _ \| '_ ` _ \ 
| | | | | | | | | | |
|_| |_| |_|_| |_| |_|
****************************/
#ifdef MM_C3
	case COS_UPCALL_RECOVERY:
	{
		/* printc("thread %d passing arg1 %p here (type %d spd %ld) to recover parent\n",  */
		/*        cos_get_thd_id(), arg1, t, cos_spd_id()); */
		mem_mgr_cli_if_recover_upcall_entry((vaddr_t)arg1);
		break;
	}
	case COS_UPCALL_RECOVERY_SUBTREE:
	{
		/* printc("thread %d passing arg1 %p here (type %d spd %ld) to recover subtree\n",  */
		/*        cos_get_thd_id(), arg1, t, cos_spd_id()); */
		mem_mgr_cli_if_recover_upcall_subtree_entry((vaddr_t)arg1);
		break;
	}
	case COS_UPCALL_REMOVE_SUBTREE:
	{
		/* printc("thread %d passing arg1 %p here (type %d spd %ld) to remove subtree\n",  */
		/*        cos_get_thd_id(), arg1, t, cos_spd_id()); */
		mem_mgr_cli_if_remove_upcall_subtree_entry((vaddr_t)arg1);
		break;
	}
#endif
	default:
		return;
	}
	return;
}
