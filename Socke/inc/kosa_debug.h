#ifndef __KOSA_DEBUG_H__
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#define __KOSA_DEBUG_H__

#include <stdio.h>
#include <kosa.h>


/* =============================================================================
 * Defines
 * =============================================================================
 */
#define MAX_PRINT_MSG_LEN               1024

/*模块ID定义*/
#define PU_ISP_MODULE_ID                (1<<0)
#define PU_VEHICLEalg_MODULE_ID         (1<<1)
#define PU_SERIAL_MODULE_ID             (1<<2)
#define PU_INTERNAL_LOOP_ID             (1<<3)
#define PU_STORE_ID                     (1<<4)
#define PU_EXTDEV_ID                    (1<<5)
#define PU_ALG_ID                       (1<<6)

// 日志等级
#define LOG_LEVEL_ERROR                 (1<<0)
#define LOG_LEVEL_WARN                  (1<<1)
#define LOG_LEVEL_INFO                  (1<<2)
#define LOG_LEVEL_DEBUG                 (1<<3)
/* =============================================================================
 * Enums
 * =============================================================================
 */


/* =============================================================================
 * Structures
 * =============================================================================
 */
KOSA_API Void KOSA_printf(const Char *szFormat, ...);

KOSA_API Void KOSA_log(const Char *szFormat, ...);

KOSA_API Void KOSA_debug(Uint32 moduleId, Uint32 level, const Char *szFormat, ...);

KOSA_API Void KOSA_setDebugLevel(Uint32 level);

KOSA_API Void KOSA_setDebugModule(Uint32 moduleId);

#if defined(_MSC_VER) || defined(_MINGW_)
KOSA_API Void InitConsoleWindow();
#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* __KOSA_DEBUG_H__ */

