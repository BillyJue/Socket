#ifndef _KOSA_CCONV_H_
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#define _KOSA_CCONV_H_

#ifdef _MSC_VER
#include <time.h>
#include "direct.h"
#include "io.h"
#endif //_MSC_VER

#include <kosa.h>


//
KOSA_API void KOSA_GB2312ToUTF8(Char *szOut,Char *szInput, Uint32 dwLen);
//
KOSA_API void KOSA_UTF8ToGB2312(Char *szOut, Char *szInput, Uint32 dwLen);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif //_KOSA_CCONV_H_

