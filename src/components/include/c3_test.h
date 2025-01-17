/* This file define all manually triggered fault and
 * micro-benchmark. The test code is in
 * /implementation/tests/extendedc3  */

#define SWIFI_ON
/* #define SWIFI_WEB   // define this differently, otherwise the performance is horrible. */

// swifi thread and timer thread should not spin (in order to inject fault)
#ifdef SWIFI_ON
#define WAIT_FAULT() do {unsigned int swifi_tt = 0;while(swifi_ready == 1){if (swifi_tt++ < 2) printc("thread %d is spinning...\n", cos_get_thd_id()); if (cos_get_thd_id() == 11 || cos_get_thd_id() == 10) break;} /* printc("thread %d continues\n", cos_get_thd_id()); */} while(0)
#else
#define WAIT_FAULT()
#endif


//#define NO_EXAMINE

#define TEST_RAMFS_C3    // using cbufp version of treadp and twritep

/****************************
 _            _    
| | ___   ___| | __
| |/ _ \ / __| |/ /
| | (_) | (__|   < 
|_|\___/ \___|_|\_\
****************************/
#define EXAMINE_LOCK

#ifdef EXAMINE_LOCK

#ifdef SWIFI_ON
#define SWIFI_SPD 10   // see above info, check this before run
#else
/* #define TEST_LOCK_ALLOC */
/* #define TEST_LOCK_PRETAKE */
#define TEST_LOCK_TAKE_BEFORE
/* #define TEST_LOCK_TAKE_AFTER */
/* #define TEST_LOCK_RELEASE_BEFORE */
/* #define TEST_LOCK_RELEASE_AFTER */

/* #define BENCHMARK_MEAS_INV_OVERHEAD_LOCK */
/* #define MEAS_LOCK_THREAD 14 */

/* #define BENCHMARK_MEAS_TAKE */
/* #define BENCHMARK_MEAS_PRETAKE */
/* #define BENCHMARK_MEAS_ALLOC */
/* #define BENCHMARK_MEAS_CREATION_TIME */
#endif

#endif

/****************************
 _       
| |_ ___ 
| __/ _ \
| ||  __/
 \__\___|
****************************/
/* #define EXAMINE_TE */

#ifdef EXAMINE_TE

#ifdef SWIFI_ON
#define SWIFI_SPD 11
#else
/* #define TEST_PTE_CREATE_BEFORE */
/* #define TEST_PTE_CREATE_AFTER  */
#define TEST_PTE_WAIT_BEFORE
/* #define TEST_PTE_WAIT_AFTER */
/* #define TEST_PTE_TIMER_THD_BEFORE */
/* #define TEST_PTE_TIMER_THD_AFTER */

/* #define BENCHMARK_MEAS_INV_OVERHEAD_PTE */
/* #define MEAS_PTE_THREAD 14 */

/* #define BENCHMARK_MEAS_CREATE */
/* #define BENCHMARK_MEAS_WAIT */
/* #define BENCHMARK_MEAS_TIMER_THD */
#endif
#endif

/****************************
            _   
  _____   _| |_ 
 / _ \ \ / / __|
|  __/\ V /| |_ 
 \___| \_/  \__|
****************************/
/* #define EXAMINE_EVT */

#ifdef EXAMINE_EVT
/* // enable upcall to each client to recover the events */
#define EVT_C3

#ifdef SWIFI_ON
#define SWIFI_SPD 12
#else
/* #define TEST_EVT_SPLIT */
/* #define TEST_EVT_TRIGGER_BEFORE */
/* #define TEST_EVT_TRIGGER_AFTER */
/* #define TEST_EVT_WAIT_BEFORE */
/* #define TEST_EVT_WAIT_AFTER */
/* #define TEST_EVT_FREE_BEFORE */
/* #define TEST_EVT_FREE_AFTER */
/* #define TEST_EVT_CREATE */

/* #define BENCHMARK_MEAS_INV_OVERHEAD_EVT */
/* #define MEAS_EVT_THREAD 14 */

/* #define BENCHMARK_MEAS_SPLIT */
/* #define BENCHMARK_MEAS_CREATE */
/* #define BENCHMARK_MEAS_EVT */
/* #define BENCHMARK_MEAS_WAIT */
/* #define BENCHMARK_MEAS_FREE */
#endif
#endif

/****************************
          _              _ 
 ___  ___| |__   ___  __| |
/ __|/ __| '_ \ / _ \/ _` |
\__ \ (__| | | |  __/ (_| |
|___/\___|_| |_|\___|\__,_|
                           
****************************/
/* #define EXAMINE_SCHED */

#ifdef EXAMINE_SCHED

#ifdef SWIFI_ON
#define SWIFI_SPD 2
#else
/* #define TEST_SCHED_CREATE_THD */
/* #define TEST_SCHED_CREATE_THD_DEFAULT */
/* #define TEST_SCHED_TIMEOUT_THD */

/* #define TEST_SCHED_WAKEUP */
/* #define TEST_SCHED_BLOCK */
/* #define TEST_SCHED_COMPONENT_TAKE */
/* #define TEST_SCHED_COMPONENT_RELEASE */
/* #define TEST_SCHED_TIMEOUT */

/* #define BENCHMARK_MEAS_INV_OVERHEAD_SCHED */
/* #define MEAS_SCHED_THREAD 14 */

//#define MEASU_SCHED_INTERFACE_CREATE
//#define MEASU_SCHED_INTERFACE_DEFAULT
//#define MEASU_SCHED_INTERFACE_WAKEUP
//#define MEASU_SCHED_INTERFACE_BLOCK
//#define MEASU_SCHED_INTERFACE_COM_TAKE
//#define MEASU_SCHED_INTERFACE_COM_RELEASE
#endif
#endif

/****************************
 _ __ ___  _ __ ___  
| '_ ` _ \| '_ ` _ \ 
| | | | | | | | | | |
|_| |_| |_|_| |_| |_|
****************************/
/* #define EXAMINE_MM */

#ifdef EXAMINE_MM

// enable upcall to each client to recover the pages
#define MM_C3
// enable this if needs upcall entry in client

#ifdef SWIFI_ON
#define SWIFI_SPD 3
#else
/* #define RECOVERY_MM_TEST    */

/* #define TEST_MM_GET_PAGE */
/* #define TEST_MM_ALIAS_PAGE */
/* #define TEST_MM_REVOKE_PAGE */

/* #define BENCHMARK_MEAS_INV_OVERHEAD_MM */
/* #define MEAS_MM_THREAD 14 */

#endif
#endif

/****************************
                      __     
 _ __ __ _ _ __ ___  / _|___ 
| '__/ _` | '_ ` _ \| |_/ __|
| | | (_| | | | | | |  _\__ \
|_|  \__,_|_| |_| |_|_| |___/
****************************/
/* #define EXAMINE_RAMFS */

#ifdef EXAMINE_RAMFS

#ifdef SWIFI_ON
#define SWIFI_SPD 15
#else

/* #define TEST_RAMFS_TSPLIT_BEFORE */
/* #define TEST_RAMFS_TSPLIT_AFTER */
/* #define TEST_RAMFS_TREADP */
/* #define TEST_RAMFS_TWRITEP_BEFORE */
/* #define TEST_RAMFS_TWRITEP_AFTER */
/* #define TEST_RAMFS_TRELEASE */
/* #define TEST_RAMFS_TMERGE */

/* #define BENCHMARK_MEAS_INV_OVERHEAD_RAMFS */
/* #define BENCHMARK_MEAS_INV_OVERHEAD_NO_SERVER_TRACK_RAMFS */

#endif
#endif

/****************************
                 _ _ _               
 _ __ ___   __ _(_) | |__   _____  __
| '_ ` _ \ / _` | | | '_ \ / _ \ \/ /
| | | | | | (_| | | | |_) | (_) >  < 
|_| |_| |_|\__,_|_|_|_.__/ \___/_/\_\
****************************/

/* !!!! Do not use this to test. Use another repo. Interface issue!!!
        Will merge later */

/* #define EXAMINE_MBOX */

/* #define TEST_MBOX_TSPLIT_SERVER_BEFORE  // server side create */
/* #define TEST_MBOX_TSPLIT_SERVER_AFTER  // server side create */
/* #define TEST_MBOX_TSPLIT_CLIENT_BEFORE  // client side create */
/* #define TEST_MBOX_TSPLIT_CLIENT_AFTER  // client side create */
/* #define TEST_MBOX_TSPLIT_READY_BEFORE */
/* #define TEST_MBOX_TSPLIT_READY_AFTER */
/* #define TEST_MBOX_TREADP */
/* #define TEST_MBOX_TWRITEP_BEFORE */
/* #define TEST_MBOX_TWRITEP_AFTER */

/* #define TEST_MBOX_TRELEASE_SERVER_BEFORE  // server side trelease */
/* #define TEST_MBOX_TRELEASE_SERVER_AFTER  // server side trelease */
/* #define TEST_MBOX_TRELEASE_CLIENT_BEFORE  // client side trelease */
/* #define TEST_MBOX_TRELEASE_CLIENT_AFTER  // client side trelease */
/* #define TEST_MBOX_TRELEASE_READY_BEFORE */
/* #define TEST_MBOX_TRELEASE_READY_AFTER */

/* #define BENCHMARK_MEAS_TSPLIT */
/* #define BENCHMARK_MEAS_TREADP */
/* #define BENCHMARK_MEAS_TWRITEP */
/* #define BENCHMARK_MEAS_TRELEASE */
