/* lock recovery client stub interface : Jiguo 

 When the lock service is faulty, there are two options to wake up the
 blocked threads in scheduler (they have no way to know the lock's
 status) -- Add the current thread onto blocked list 1) Track the
 blocked threads based on per lock over an interface 2) Wake up all
 blked threads on the lock if any blocked thread returns 3) Remove the
 items from list when the lock is released 

 See the reflection on scheduler server interface Question 1: When
 should we wake up all threads that invoke sched_block from lock
 component? When to wake the threads per lock from lock component?
 Question 2: Should we make the reflection from the client or the
 rebooted faulty component?  If do so from client, there is one issue
 :all clients need depend on the bottom component, which seems fine
 since there is no circular dependency created.

When get new lock id (alloc), there are two circumstances 1) on the
normal path (no fault or after the fault), we need get a new unique
client id (server id could be the same as client id, or totally
different) 2) when recovery (in update_rd), we need just a new server
id and unchanged client id

*/

#include <cos_component.h>
#include <cos_debug.h>
#include <print.h>

#include <objtype.h>

#include <sched.h>
#include <lock.h>
#include <cstub.h>

extern void *alloc_page(void);
extern void free_page(void *ptr);

#define CSLAB_ALLOC(sz)   alloc_page()
#define CSLAB_FREE(x, sz) free_page(x)
#include <cslab.h>

#define CVECT_ALLOC() alloc_page()
#define CVECT_FREE(x) free_page(x)
#include <cvect.h>

#define LOCK_PRINT 0

#if LOCK_PRINT == 1
#define print_lk(fmt,...) printc(fmt, ##__VA_ARGS__)
#else
#define print_lk(fmt,...)
#endif

/* global fault counter, only increase, never decrease */
static unsigned long fcounter;

/* recovery data structure lock service */
struct blocked_thd {
	int id;
	struct blocked_thd *next, *prev;
};

struct rec_data_lk {
	spdid_t       spdid;
	int           owner_thd;  // for sanity check
	unsigned long c_lkid;
	unsigned long s_lkid;

	unsigned long fcnt;
	struct blocked_thd blkthd;
};

/**********************************************/
/* slab allocalk and cvect for tracking lock */
/**********************************************/

CVECT_CREATE_STATIC(rec_lk_vect);
CSLAB_CREATE(rdlk, sizeof(struct rec_data_lk));

static struct rec_data_lk *
rdlk_lookup(int id)
{ 
	return cvect_lookup(&rec_lk_vect, id); 
}

static struct rec_data_lk *
rdlk_alloc(int lkid)
{
	struct rec_data_lk *rd;

	rd = cslab_alloc_rdlk();
	assert(rd);
	cvect_add(&rec_lk_vect, rd, lkid);
	return rd;
}

static void
rdlk_dealloc(struct rec_data_lk *rd)
{
	assert(rd);
	cslab_free_rdlk(rd);
}

static void
rdlk_addblk(struct rec_data_lk *rd, struct blocked_thd *ptr_blkthd)
{
	assert(rd && ptr_blkthd);

	ptr_blkthd->id = cos_get_thd_id();
	INIT_LIST(ptr_blkthd, next, prev);
	ADD_LIST(&rd->blkthd, ptr_blkthd, next, prev);
       
	return;
}

static int
get_unique(void)
{
	unsigned int i;
	cvect_t *v;

	v = &rec_lk_vect;
	for(i = 1 ; i <= CVECT_MAX_ID ; i++) {
		if (!cvect_lookup(v, i)) return i;
	}
	/* if (!cvect_lookup(v, CVECT_MAX_ID)) return CVECT_MAX_ID; */

	return -1;
}

static void 
rd_cons(struct rec_data_lk *rd, spdid_t spdid, unsigned long cli_lkid, unsigned long ser_lkid, int owner_thd)
{
	assert(rd);

	rd->spdid	 = spdid;
	rd->s_lkid	 = ser_lkid;
	rd->c_lkid	 = cli_lkid;
	rd->owner_thd    = owner_thd;
	rd->fcnt	 = fcounter;

	return;
}

#ifdef REFLECTION
static int
cap_to_dest(int cap)
{
	int dest = 0;
	assert(cap > MAX_NUM_SPDS);
	if ((dest = cos_cap_cntl(COS_CAP_GET_SER_SPD, 0, 0, cap)) <= 0) assert(0);
	return dest;
}
#endif

extern int sched_reflect(spdid_t spdid, int src_spd, int cnt);
extern int sched_wakeup(spdid_t spdid, unsigned short int thd_id);
extern vaddr_t mman_reflect(spdid_t spd, int src_spd, int cnt);
extern int mman_release_page(spdid_t spd, vaddr_t addr, int flags); 

#ifdef REFLECTION
static void
rd_relfection(int cap)
{
	assert(cap);

	int count_obj = 0; // reflected objects
	int lock_spd = cap_to_dest(cap);
	
	// remove the mapped page for lock spd
	vaddr_t addr;
	count_obj = mman_reflect(cos_spd_id(), lock_spd, 1);
	while (count_obj--) {
		addr = mman_reflect(cos_spd_id(), lock_spd, 0);
		mman_release_page(cos_spd_id(), addr, lock_spd);
	}

	// to reflect all threads blocked from lock component
	int wake_thd;
	count_obj = sched_reflect(cos_spd_id(), lock_spd, 1);
	while (count_obj--) {
		wake_thd = sched_reflect(cos_spd_id(), lock_spd, 0);
		sched_wakeup(cos_spd_id(), wake_thd);
	}

	return;
}
#endif

static struct rec_data_lk *
update_rd(int lkid, int thd, int cap)
{
        struct rec_data_lk *rd = NULL;
	struct blocked_thd *blk_thd;

        rd = rdlk_lookup(lkid);
	if (unlikely(!rd)) goto done;
	if (likely(rd->fcnt == fcounter)) goto done;
	rd->fcnt = fcounter;  // update fcounter first before wake up any other threads

#ifdef REFLECTION
	rd_relfection(cap);
#else
	for (blk_thd = FIRST_LIST(&rd->blkthd, next, prev);
	     blk_thd != &rd->blkthd;
	     blk_thd = FIRST_LIST(blk_thd, next, prev)){
		sched_wakeup(cos_spd_id(), blk_thd->id);
	}
#endif
	// update server side id, but keep client side id unchanged
	rd->s_lkid = __lock_component_alloc(cos_spd_id());
done:
	return rd;
}

CSTUB_FN_ARGS_1(unsigned long, __lock_component_alloc, spdid_t, spdid)

        struct rec_data_lk *rd = NULL;
	unsigned long ser_lkid, cli_lkid;
redo:
CSTUB_ASM_1(__lock_component_alloc, spdid)
       if (unlikely (fault)){
	       if (cos_fault_cntl(COS_CAP_FAULT_UPDATE, cos_spd_id(), uc->cap_no)) {
		       printc("set cap_fault_cnt failed\n");
		       BUG();
	       }

       	       fcounter++;
       	       goto redo;
       }

CSTUB_POST


/************************************/
/******  client stub functions ******/
/************************************/

/* 
   lock_component_alloc will get a server side lock id every time
   client side id should always be uniquex 
*/
CSTUB_FN_ARGS_1(unsigned long, lock_component_alloc, spdid_t, spdid)

        struct rec_data_lk *rd = NULL;
	unsigned long ser_lkid, cli_lkid;
redo:
CSTUB_ASM_1(lock_component_alloc, spdid)
       if (unlikely (fault)){
	       CSTUB_FAULT_UPDATE();
       	       goto redo;
       }

	if ((ser_lkid = ret) > 0) {
		// if does exist, we need an unique client id. Otherwise, create it
		if (unlikely(rdlk_lookup(ser_lkid))) {
			cli_lkid = get_unique();
			assert(cli_lkid > 0 && cli_lkid != ser_lkid);
		} else {
			cli_lkid = ser_lkid;
		}
		// always ensure that cli_lkid is unique
		rd = rdlk_alloc(cli_lkid);
		assert(rd);

		rd_cons(rd, cos_spd_id(), cli_lkid, ser_lkid, 0);
		INIT_LIST(&rd->blkthd, next, prev);
		ret = cli_lkid;
	}

CSTUB_POST


CSTUB_FN_ARGS_3(int, lock_component_pretake, spdid_t, spdid, unsigned long, lock_id, unsigned short int, thd)

        struct rec_data_lk *rd = NULL;
redo:
        rd = update_rd(lock_id, thd, uc->cap_no);
	if (!rd) {
		printc("try to pretake a non-tracking lock\n");
		return -1;
	}

CSTUB_ASM_3(lock_component_pretake, spdid, rd->s_lkid, thd)

       if (unlikely(fault)){
	       CSTUB_FAULT_UPDATE();
       	       goto redo;
       }

CSTUB_POST

// track the block thread on each thread stack
CSTUB_FN_ARGS_3(int, lock_component_take, spdid_t, spdid, unsigned long, lock_id, unsigned short int, thd)

	struct blocked_thd blk_thd;
	struct rec_data_lk *rd = NULL;

        rd = update_rd(lock_id, thd, uc->cap_no);   // call call booter instead
	if (!rd) {
		printc("try to take a non-tracking lock\n");
		return -1;
	}
       rdlk_addblk(rd, &blk_thd);       
CSTUB_ASM_3(lock_component_take, spdid, rd->s_lkid, thd)

       if (unlikely (fault)){
	       CSTUB_FAULT_UPDATE();
	       /*  remove current thd from blocking list since the
		   fault occurs either before the current thd blocked
		   (invoke scheduler) or after woken up (return from
		   scheduler). So it should not staty on the blocking
		   list after fault occurs
	       */
	       REM_LIST(&blk_thd, next, prev);
	       ret = 0; // this will force curr thread to contend the lock again
       }
       REM_LIST(&blk_thd, next, prev);

CSTUB_POST


CSTUB_FN_ARGS_2(int, lock_component_release, spdid_t, spdid, unsigned long, lock_id)

        struct rec_data_lk *rd = NULL;
redo:
        rd = update_rd(lock_id, cos_get_thd_id(), uc->cap_no);
	if (!rd) {
		printc("try to release a non-tracking lock\n");
		return -1;
	}

CSTUB_ASM_2(lock_component_release, spdid, rd->s_lkid)

       if (unlikely (fault)){
	       CSTUB_FAULT_UPDATE();
       	       goto redo;
       }

CSTUB_POST