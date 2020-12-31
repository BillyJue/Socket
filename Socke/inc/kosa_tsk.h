#ifndef __KOSA_TSK_H__
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#define __KOSA_TSK_H__

#include <kosa.h>
#ifdef _LINUX_
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include <sched.h>
#include <pthread.h>
#include <unistd.h>
#endif

/* =============================================================================
 * Defines
 * =============================================================================
 */
#ifdef _LINUX_
#define KOSA_TSK_FIFO_PRI_MAX                 sched_get_priority_max(SCHED_FIFO)
#define KOSA_TSK_FIFO_PRI_MIN                 sched_get_priority_min(SCHED_FIFO)
#define KOSA_TSK_RR_PRI_MAX                   sched_get_priority_max(SCHED_RR)
#define KOSA_TSK_RR_PRI_MIN                   sched_get_priority_min(SCHED_RR)
#define KOSA_TSK_NORMAL_PRI_MAX               sched_get_priority_max(SCHED_OTHER)
#define KOSA_TSK_NORMAL_PRI_MIN               sched_get_priority_min(SCHED_OTHER)

#endif
#if defined(_MSC_VER) || defined(_MINGW_)
#define KOSA_TSK_FIFO_PRI_MAX                 255
#define KOSA_TSK_FIFO_PRI_MIN                 128
#define KOSA_TSK_RR_PRI_MAX                   255
#define KOSA_TSK_RR_PRI_MIN                   128
#define KOSA_TSK_NORMAL_PRI_MAX               127
#define KOSA_TSK_NORMAL_PRI_MIN               0
#endif
#define KOSA_TSK_PRI_DEFAULT             0

#define KOSA_TSK_STACK_SIZE_DEFAULT      0

#define KOSA_TSK_SCHED_POLICY_FIFO       1
#define KOSA_TSK_SCHED_POLICY_NOMAL      2
#define KOSA_TSK_SCHED_POLICY_RR         3

#if defined(_APQ8056_) || defined(_KYLIN3_)
#define KOSA_SET_AFFINITY(CPUCORE)\
    cpu_set_t mask;\
if(CPUCORE >= sysconf(_SC_NPROCESSORS_CONF))\
{\
    CPUCORE = sysconf(_SC_NPROCESSORS_CONF) - 1;\
}\
{\
    CPU_ZERO(&mask);\
    CPU_SET(CPUCORE, &mask);\
    sched_setaffinity(0, sizeof(cpu_set_t), &mask);\
}
#else
#define KOSA_SET_AFFINITY(CPUCORE)
#endif

#if defined(_APQ8056_) || defined(_KYLIN3_)
#define KOSA_SET_AFFINITY_2(CPUCORE, CPUCORE_2)\
    cpu_set_t mask;\
if(CPUCORE >= sysconf(_SC_NPROCESSORS_CONF))\
{\
    CPUCORE = sysconf(_SC_NPROCESSORS_CONF) - 1;\
}\
if(CPUCORE_2 >= sysconf(_SC_NPROCESSORS_CONF))\
{\
    CPUCORE_2 = sysconf(_SC_NPROCESSORS_CONF) - 1;\
}\
{\
    CPU_ZERO(&mask);\
    CPU_SET(CPUCORE, &mask);\
    CPU_SET(CPUCORE_2, &mask);\
    sched_setaffinity(0, sizeof(cpu_set_t), &mask);\
}
#else
#define KOSA_SET_AFFINITY(CPUCORE)
#endif

/* =============================================================================
 * Enums
 * =============================================================================
 */


/* =============================================================================
 * Structures
 * =============================================================================
 */
typedef void * (*KOSA_TskEntryFunc)(void *);

typedef struct {
#ifdef _LINUX_
  pthread_t      hndl;
#endif
#if defined(_MSC_VER) || defined(_MINGW_)
  HANDLE    hndl;
#endif
}KOSA_TskHndl;

typedef struct {
    KOSA_TskHndl hndl;
    Uint32 cpuCore;
    Uint32 cpuCore_2;
    Uint32 policy;
    Uint32 pri;
    Uint32 stackSize;
    KOSA_TskEntryFunc entryFunc;
    Char* entryName;
    Void* prm;
}KOSA_TskObj;

/* =============================================================================
 * APIs
 * =============================================================================
 */
KOSA_API Int32 KOSA_tskCreate(KOSA_TskObj *tskObj);

KOSA_API Uint32 KOSA_tskSelfID(  );

KOSA_API Int32 KOSA_tskTerminate(KOSA_TskHndl hTask);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* __KOSA_TSK_H__ */

