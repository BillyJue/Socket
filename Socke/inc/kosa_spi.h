
#ifndef _KOSA_SPI_H_
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#define _KOSA_SPI_H_

#include <kosa.h>

typedef struct {

    int fd;

} KOSA_SpiHndl;

typedef struct {
    char *rxdata;                       //接收数据指针
    char *txdata;                       //发送数据指针
    Uint32 len;                           //数据长度
}KOSA_SpiInfo;

KOSA_API Int32 KOSA_spiOpen(KOSA_SpiHndl *hndl, Uint8 busNo, Uint8 cs);
KOSA_API Int32 KOSA_spiSetMode(KOSA_SpiHndl *hndl, Uint8 mode);
KOSA_API Int32 KOSA_spiGetMode(KOSA_SpiHndl *hndl, Uint8 *mode);
KOSA_API Int32 KOSA_spiGetWordLength(KOSA_SpiHndl *hndl, Uint8 *bits);
KOSA_API Int32 KOSA_spiSetWordLength(KOSA_SpiHndl *hndl, Uint8 bits);
KOSA_API Int32 KOSA_spiGetSpeed(KOSA_SpiHndl *hndl, Uint32 *speed);
KOSA_API Int32 KOSA_spiSetSpeed(KOSA_SpiHndl *hndl, Uint32 speed);
KOSA_API Int32 KOSA_SpiRead(KOSA_SpiHndl *hndl, KOSA_SpiInfo *spiInfo);
KOSA_API Int32 KOSA_SpiWrite(KOSA_SpiHndl *hndl, KOSA_SpiInfo *SpiInfo);
KOSA_API Int32 KOSA_SpiReadWrite(KOSA_SpiHndl *hndl, KOSA_SpiInfo *SpiInfo);
KOSA_API Int32 KOSA_spiClose(KOSA_SpiHndl *hndl);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /*_KOSA_I2C_H_*/

