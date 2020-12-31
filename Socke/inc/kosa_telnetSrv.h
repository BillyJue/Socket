
#ifndef __KOSA_TELNET_SRV_H__
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#define __KOSA_TELNET_SRV_H__

#include <kosa.h>

KOSA_API Void KOSA_RegCommand(Char* name, Void* func, Char* usage);

KOSA_API Void KOSA_telnetPrint(Char* pszMsg);

KOSA_API Int32 KOSA_telnetSrvInit(Uint16 wPort, Char* username, Char* passwd);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* __KOSA_TELNET_SRV_H__ */

