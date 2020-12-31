#ifndef __KOSA_H__
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#define __KOSA_H__

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#if defined(_MSC_VER) || defined(_MINGW_)
#include <winsock2.h>
#include <windows.h>
#pragma comment(lib, "ws2_32.lib")
#endif

/* =============================================================================
 * Defines
 * =============================================================================
 */
#define KOSA_SOK                            0
#define KOSA_SFAIL                          -1
#define MAX_STRING_LENGTH_128               128
#define MAX_STRING_LENGTH_64                64
#define MAX_STRING_LENGTH_32                32
#define MAX_STRING_LENGTH_16                16
#define MAX_STRING_LENGTH_8                 8

/** \brief Floor a integer value. */
#define KOSA_floor(val, align)  (((val) / (align)) * (align))

/** \brief Align a integer value. */
#define KOSA_align(val, align)  KOSA_floor(((val) + (align)-1), (align))

#define uABS(a, b)   (a>b ? (a-b) : (b-a))

#ifndef TRUE
#define TRUE                    1
#endif

#ifndef FALSE
#define FALSE                   0
#endif

#if defined(WIN32)
    #define KOSA_API __declspec(dllexport)
#else
    #define KOSA_API
#endif

#define KOSA_TIMEOUT_NONE        ((Uint32) 0)
#define KOSA_TIMEOUT_FOREVER     ((Uint32)-1)
/* =============================================================================
 * Enums
 * =============================================================================
 */


/* =============================================================================
 * Structures
 * =============================================================================
 */
#ifndef Uint64
#ifdef _MSC_VER
typedef unsigned __int64        Uint64;
#else
typedef unsigned long long      Uint64;
#endif
#endif
#ifndef Int64
#ifdef _MSC_VER
typedef __int64                 Int64;
#else
typedef long long               Int64;
#endif
#endif
#ifndef Uint32
typedef unsigned int            Uint32;
#endif
#ifndef Int32
typedef int                     Int32;
#endif
#ifndef Uint16
typedef unsigned short          Uint16;
#endif
#ifndef Int16
typedef short                   Int16;
#endif
#ifndef Uint8
typedef unsigned char           Uint8;
#endif
#ifndef Int8
typedef signed char             Int8;
#endif
#ifndef Void
typedef void                    Void;
#endif
#ifndef Char
typedef char                    Char;
#endif
#ifndef Bool
typedef unsigned char           Bool;
#endif
#ifndef Float
typedef float                   Float;
#endif
#ifndef KOSA_HANDLE
typedef void *                  KOSA_HANDLE;
#endif

/*点*/
typedef struct{
    Int32 X;
    Int32 Y;
}POS_S;

/*线*/
typedef struct{
    Int32 X1;
    Int32 Y1;
    Int32 X2;
    Int32 Y2;
}LINE_S;

/*矩形*/
typedef struct{
    Int32 X;
    Int32 Y;
    Int32 W;
    Int32 H;
}RECTANGLE_S;
typedef struct
{
	Int32 up;
	Int32 down;
	Int32 left;
	Int32 right;
}ZRECT_S;

/*梯形*/
typedef struct{
    Int32 X1;
    Int32 Y1;
    Uint32 W1;
    Int32 X2;
    Int32 Y2;
    Uint32 W2;
}ECHELON_S;

/*规则线*/
typedef struct {
    /*使能*/
    Bool enable;
    /*规则线编号*/
    Uint32 LineLabel;
    /*直线*/
    LINE_S tLine;
}TRuleLine;

/*规则区域*/
typedef struct {
    /*使能*/
    Bool enable;
    /*编号*/
    Uint32 Label;
    /*区域*/
    ECHELON_S echelonArea;
}TRuleRegion;

/*多边形*/
typedef struct
{
    Int32 pointNum;
    POS_S point[8];
}TPolygon_S;

/*多边形规则区域*/
typedef struct {
    /*使能*/
    Bool enable;
    /*编号*/
    Uint32 Label;
    /*区域*/
    TPolygon_S area;
}TPolygonRegion;

/*车道线*/
typedef struct
{
    /*车道左边线*/
    TRuleLine tRdLineL;
    /*车道右边线*/
    TRuleLine tRdLineR;
}TRdLine_S;

/*定时抓拍*/
typedef struct
{
    /*时间段个数: 最多4个时间段*/
    Uint32 num;
    /*开始时间: 精确到分钟*/
    Uint32 startTimeHour[4];
    Uint32 startTimeMin[4];
    /*结束时间: 精确到分钟*/
    Uint32 stopTimeHour[4];
    Uint32 stopTimeMin[4];
    /*轮询时间*/
    Uint32 time_Retention[4];
}TSnapTiming;

/* =============================================================================
 * APIs
 * =============================================================================
 */
/**/
KOSA_API Bool bSnapTimeIn(const TSnapTiming* , Uint32* );

KOSA_API Int32 KOSA_LocalTime(Int64 second, struct tm *pTime);

KOSA_API Char* KOSA_asctime(Int64 timeStamp);

/*不受系统时间的修改影响*/
KOSA_API Uint32 KOSA_getCurTimeInMsec();

/*系统时间*/
KOSA_API Int64 KOSA_getTimeStampInMsec();

KOSA_API Void KOSA_waitMsecs(Uint32 msecs);

KOSA_API Void KOSA_sysReboot();

KOSA_API Uint32 KOSA_ClkRateGet();

#define KOSA_assert(x) \
        { \
            if((x) == 0) \
            { \
                do \
                { \
                    KOSA_log("ASSERT (%s|%s|%d)\r\n", __FILE__, __FUNCTION__, __LINE__); \
                    KOSA_waitMsecs(60*1000); \
                    KOSA_sysReboot(); \
                }while (1); \
            } \
        } \

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* __KOSA_H__ */

