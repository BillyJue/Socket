#ifndef __KOSA_BUF_H__
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#define __KOSA_BUF_H__

#include <kosa.h>
#include <kosa_que.h>

/* =============================================================================
 * Defines
 * =============================================================================
 */


/* =============================================================================
 * Enums
 * =============================================================================
 */
#define KOSA_BUF_NUM_MAX       (64)

/* =============================================================================
 * Structures
 * =============================================================================
 */
typedef struct {

    KOSA_QueHndl emptyQue;

    KOSA_QueHndl fullQue;

}KOSA_BufHndl;

/* =============================================================================
 * APIs
 * =============================================================================
 */

KOSA_API Int32 KOSA_bufGetEmpty(KOSA_BufHndl *hndl, Void **bufInfo, Int32 timeout);

KOSA_API Int32 KOSA_bufPutFull(KOSA_BufHndl *hndl, Void *bufInfo, Int32 timeout);

KOSA_API Int32 KOSA_bufGetFull(KOSA_BufHndl *hndl, Void **bufInfo, Int32 timeout);

KOSA_API Int32 KOSA_bufPutEmpty(KOSA_BufHndl *hndl, Void *bufInfo, Int32 timeout);

KOSA_API Uint32 KOSA_bufGetEmptyCount(KOSA_BufHndl *hndl);

KOSA_API Uint32 KOSA_bufGetFullCount(KOSA_BufHndl *hndl);

KOSA_API Int32 KOSA_bufCreate(KOSA_BufHndl* hndl, Uint32 maxBufnum);

KOSA_API Int32 KOSA_bufDelete(KOSA_BufHndl *hndl);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* __KOSA_BUF_H__ */
