#ifndef __KOSA_TIMER_H__
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#define __KOSA_TIMER_H__

#include <kosa.h>
#include <kosa_tsk.h>
#include <kosa_mutex.h>
#include <kosa_list.h>

typedef Int32 (*timerCb)(Void* );

typedef struct
{
    Bool bIdle;
    void* cbMethodArgs;
    timerCb cbMethod;
    Int32 startInterval;
    KOSA_SemHndl waitReady;
    void* (*run)(Void* );
    KOSA_TskObj task;
    struct list_head listNode;
}KOSA_timer;

/*method: 定时器执行的方法，method的返回值表示延时多长时间再次执行该方法。返回0，timer结束
  methodArgs: 传给方法的参数
  delayMs: 延时多长时间，执行该方法*/
KOSA_API Int32 KOSA_timerStart(timerCb method, Void* methodArgs, Int32 delayMs);

KOSA_API Void KOSA_timerInit(Int32 maxTimer);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* __KOSA_TIMER_H__ */

