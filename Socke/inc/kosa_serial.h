#ifndef __KOSA_SERIAL_H__
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#define __KOSA_SERIAL_H__

#include "kosa.h"



#define INVALID_SERIAL_HANDLE   0

/* 设置串口的波特率 */
#define SERIAL_SET_BAUDRATE                   0
/* 查询串口的波特率 */
#define SERIAL_GET_BAUDRATE                   1
/* 设置串口的停止位 */
#define SERIAL_SET_STOPBIT                    2
/* 查询串口的停止位 */
#define SERIAL_GET_STOPBIT                    3
/* 设置串口的数据位 */
#define SERIAL_SET_DATABIT                    4
/* 查询串口的数据位 */
#define SERIAL_GET_DATABIT                    5
/* 设置串口的奇偶校验位 */
#define SERIAL_SET_PARITY                     6
/* 查询串口的奇偶校验位 */
#define SERIAL_GET_PARITY                     7
/* 设置串口的起始位 */
#define SERIAL_SET_STARTBIT                   8
/* 查询串口的起始位 */
#define SERIAL_GET_STARTBIT                   9
/* 设置流控制 */
#define SERIAL_SET_FLOW_CONTROL               10
/* 设置流控制 */
#define SERIAL_GET_FLOW_CONTROL               11

/* =============================================================================
 * Enums
 * =============================================================================
 */
/*校验类型*/
typedef enum
{
    NO_PARITY,                    //无校验
    EVEN_PARITY,                  //偶校验
    ODD_PARITY,                   //奇校验
    SPACE_PARITY,                 //空格校验
    MARK_PARITY,                  //标记校验
}EParityMode;

/*串口设备句柄*/
typedef struct {
#ifdef _LINUX_
    Int32 fd;
#endif
#if defined(_MSC_VER) || defined(_MINGW_)
    HANDLE fd;
#endif
} KOSA_SerialHndl;

/*串口参数*/
typedef struct {
    Uint32 nBaudrate;           //波特率
    Uint32 nStopBits;           //停止位
    Uint32 nDataBits;           //数据位
    Uint32 nParity;             //奇偶校验位
}KOSA_SerialParams;

/* =============================================================================
 * APIs
 * =============================================================================
 */
KOSA_API Int32 KOSA_SerialOpen(KOSA_SerialHndl *hndl, Char *device);

KOSA_API Int32 KOSA_SerialClose(KOSA_SerialHndl *hndl);

KOSA_API Int32 KOSA_SerialSetParams(KOSA_SerialHndl *hndl, KOSA_SerialParams *params);

KOSA_API Int32 KOSA_SerialReadData(KOSA_SerialHndl *hndl, Uint8 *buf, Uint32 len);

KOSA_API Int32 KOSA_SerialWriteData(KOSA_SerialHndl *hndl, Uint8 *buf, Uint32 len);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* __KOSA_SERIAL_H__ */

