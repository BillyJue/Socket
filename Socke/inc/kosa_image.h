#ifndef __KOSA_IMAGE_H__
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#define __KOSA_IMAGE_H__

#include <kosa.h>

/* =============================================================================
 * Defines
 * =============================================================================
 */
/* MD5校验算法需要使用的宏 */
#define F1(x, y, z) (z ^ (x & (y ^ z)))
#define F2(x, y, z) F1(z, x, y)
#define F3(x, y, z) (x ^ y ^ z)
#define F4(x, y, z) (y ^ (x | ~z))

#define FF 255
#define D8 216
#define FE 254
#define D9 217

/* MD5算法核心 */
#define MD5STEP(f, w, x, y, z, data, s) \
    ( w += f(x, y, z) + data,  w = w<<s | w>>(32-s),  w += x )


/* =============================================================================
 * Enums
 * =============================================================================
 */


/* =============================================================================
 * Structures
 * =============================================================================
 */
#if !defined(_MSC_VER) && !defined(_MINGW_)
#pragma pack(push) //保存对齐状态
#pragma pack(2)//设定为4字节对齐
/*BMP文件头结构*/
typedef struct
{
    Uint16        bfType;
    unsigned long bfSize;
    Uint16        bfReserved1;
    Uint16        bfReserved2;
    unsigned long bfOffBits;
} BITMAPFILEHEADER;

/*BMP文件信息头结构*/
typedef struct
{
    unsigned long biSize;
    long          biWidth;
    long          biHeight;
    Uint16        biPlanes;
    Uint16        biBitCount;
    unsigned long biCompression;
    unsigned long biSizeImage;
    long          biXPelsPerMeter;
    long          biYPelsPerMeter;
    unsigned long biClrUsed;
    unsigned long biClrImportant;
} BITMAPINFOHEADER;
#pragma pack(pop)//恢复对齐状态
#endif
/*YUV420P图像缩放算法类型*/
typedef enum
{
    SCALE_TYEP_NEAREST,
    SCALE_TYPE_BILINEAR,
} KOSA_SCALE_TYPE;

/*YUV420P图像缩放内部使用的单元结构*/
typedef struct VSImage
{
    Uint8 *pixels;
    Uint32 width;
    Uint32 height;
    /*步长*/
    Uint32 stride;
} KOSA_VSImage;

/*外部调用YUV420P缩放函数需要的结构*/
typedef struct YUVScaleObj
{
    Uint8 *yuvbuf;
    Uint32 width;
    Uint32 height;
} KOSA_YUVScale;

/*水印算法返回值*/
typedef enum
{
    SUCCEED = 0,
    /*输入码流中找不到标准JPEG码流的起止位“FF D8”，"FF D9"*/
    ERR_INPUT_STREAM,
    /*输入码流中不含校验值*/
    ERR_NOT_FIND_CHECK_FLAG,
    /*接收端计算得出的校验值和发送端的不一致*/
    ERR_CHECK_VALUE_UNEQUAL
}EReturnType;

/*MD5上下文*/
typedef struct
{
    Uint32 State[4];
    Uint32 Count[2];
    Uint8  Buffer[64];
}TMD5Context;


/*定义一个坐标点*/
typedef struct
{
    Uint32 dwX;
    Uint32 dwY;
}KOSA_Point;

/*定义一个多边形*/
typedef struct
{
    Uint32 dwConvexNum;
    KOSA_Point atPoint[16];
}KOSA_Polygon;


/* =============================================================================
 * APIs
 * =============================================================================
 */

/*
 函 数 名: KOSA_GetBmpWidthAndHeight
 功    能: 获取Bmp图像的宽、高
 参    数: pFilePath    输入Bmp文件路径
           pWidth       输出Bmp文件宽指针
           pHeight      输出Bmp文件高指针
 返 回 值: 成功返回0，失败返回-1
 */
Int32 KOSA_GetBmpWidthAndHeight(Char *pFilePath, Uint32 *pWidth, Uint32 *pHeight);

/*
 函 数 名: KOSA_BmpToYUV420SP
 功    能: Bmp格式转YUV420SP(注意:仅获取Y数据)
 参    数: pFilePath    输入Bmp文件路径
           pBufYUV      输出YUV格式中"Y"数据指针
           pWidth       输出Bmp或者YUV格式宽指针
           pHeight      输出Bmp或者YUV格式高指针
 返 回 值: 成功返回0，失败返回-1
 */
Int32 KOSA_BmpToYUV420SP(Char *pFilePath, Uint8 *pBufYUV, Uint32 *pWidth, Uint32 *pHeight);

/*
 函 数 名: KOSA_BmpToYUV420_Color
 功    能: Bmp格式转YUV420SP或者YUV420P(仅支持24位RGB图像)
 参    数: pFilePath    输入Bmp文件路径
           pBufY        输出YUV图像中Y数据指针
           pBufUV       输出YUV图像中UV数据指针
           IS_SP        输入1为YUV420SP,0为YUV420P
 返 回 值: 成功返回0,
 */
Int32 KOSA_BmpToYUV420_Color(Char *pFilePath, Uint8 *pBufY, Uint8 *pBufUV,Int32 IS_SP);

/*
 函 数 名: KOSA_Scale_YUV422_Bilinear
 功    能: YUV422格式图像的缩放
 参    数: pdest         输出目标YUV结构指针(数据，宽，高)
           psrc          输入源YUV结构指针(数据，宽，高)
 返 回 值: 无
 */
Void KOSA_Scale_YUV422_Bilinear(KOSA_YUVScale *pDest, KOSA_YUVScale *pSrc);

/*
 函 数 名: KOSA_Scale_YUV422_Magnify
 功    能: YUV422格式图像的放大
 参    数: pdest         输出目标YUV结构指针(数据，宽，高)
           psrc          输入源YUV结构指针(数据，宽，高)
           mulriple      放大倍数
 返 回 值: 无
 */
Void KOSA_Scale_YUV422_Magnify(KOSA_YUVScale *pDest, KOSA_YUVScale *pSrc, Uint32 mulriple);


/*
 函 数 名: KOSA_Scale_YUV420P
 功    能: YUV420P格式图像的缩放
 参    数: pdest         输出目标YUV结构指针(数据，宽，高)
           psrc          输入源YUV结构指针(数据，宽，高)
           scale_type   输入缩放算法类型:SCALE_TYEP_NEAREST，SCALE_TYPE_BILINEAR
 返 回 值: 无
 */
Void KOSA_Scale_YUV420P(KOSA_YUVScale *pDest, KOSA_YUVScale *pSrc, KOSA_SCALE_TYPE Scale_Type);

/*
函 数 名:  KOSA_JpegCheckFlagCreat
功    能:  计算校验值并将其写入Jpeg码流
算法实现:  无
全局变量:  无
参    数:  pInput       输入码流指针[in]
           InputLen    输入码流的长度[in]
           pOutput      输出Md5校验码指针[out]

返 回 值:  若输入码流符合JPEG格式，返回成功；否则，返回失败
 */
Int32 KOSA_JpegMD5FlagCreat(Uint8 *pInput, Uint32 InputLen, Uint8 *pOutput);

/*
 函 数 名: KOSA_GetBoundRectangle
 功    能: 获取多边形的外接矩形
 参    数: pRectangle    返回的外接矩形
           pPolygon      输入的多边形

 返 回 值: 若输入的多边形边数目大于3，返回成功；否则，返回失败
 */
Int32 KOSA_GetBoundRectangle(KOSA_Polygon *pRectangle, KOSA_Polygon *pPolygon);


#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* __KOSA_IMAGE_H__ */
