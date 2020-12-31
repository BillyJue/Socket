#ifndef __KOSA_MUTEX_H__
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#define __KOSA_MUTEX_H__

#include <kosa.h>
#ifdef _LINUX_
#include <semaphore.h>
#include <pthread.h>
#endif


typedef struct {
#ifdef _LINUX_
    pthread_mutex_t lock;
#endif
#if defined(_MSC_VER) || defined(_MINGW_)
    HANDLE lock;
#endif
}KOSA_MutexHndl;

typedef struct {
#ifdef _LINUX_
	pthread_mutex_t lock;
	pthread_cond_t  cond;
	Uint32 value;
#endif
#if defined(_MSC_VER) || defined(_MINGW_)
	HANDLE sem;
#endif
}KOSA_SemHndl;


KOSA_API Int32 KOSA_mutexCreate(KOSA_MutexHndl* hndl);

KOSA_API Int32 KOSA_mutexDelete(KOSA_MutexHndl* hndl);

KOSA_API Int32 KOSA_mutexLock(KOSA_MutexHndl* hndl);

KOSA_API Int32 KOSA_mutexTrylock(KOSA_MutexHndl* hndl);

KOSA_API Int32 KOSA_mutexUnlock(KOSA_MutexHndl *hndl);


KOSA_API Int32 KOSA_semCreate(KOSA_SemHndl* hndl, Uint32 value);

KOSA_API Int32 KOSA_semDelete(KOSA_SemHndl* hndl);

KOSA_API Int32 KOSA_semPost(KOSA_SemHndl* hndl);

KOSA_API Int32 KOSA_semWait(KOSA_SemHndl* hndl);

KOSA_API Int32 KOSA_semWaitTimeout(KOSA_SemHndl* hndl, Uint32 timeout);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* __OSA_MUTEX_H__ */