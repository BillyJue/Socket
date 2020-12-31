
#ifndef _KOSA_I2C_H_
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#define _KOSA_I2C_H_

#include <kosa.h>

typedef struct {

    int fd;

} KOSA_I2cHndl;

typedef struct {
    Uint16 devAddr;                         /*i2c device address*/
    Uint8 *reg;                             /*register address  */
    Uint8 *value;                           /*i2c transfer value*/
    Uint32 count;                           /*i2c transfer count*/
}KOSA_I2cInfo;

KOSA_API Int32 KOSA_i2cOpen(KOSA_I2cHndl *hndl, Uint8 instId);
KOSA_API Int32 KOSA_i2cRead8(KOSA_I2cHndl *hndl, KOSA_I2cInfo *i2cInfo);
KOSA_API Int32 KOSA_i2cWrite8(KOSA_I2cHndl *hndl, KOSA_I2cInfo *i2cInfo);
KOSA_API Int32 KOSA_i2cRawWrite8(KOSA_I2cHndl *hndl, KOSA_I2cInfo *i2cInfo);
KOSA_API Int32 KOSA_i2cRawRead8(KOSA_I2cHndl *hndl, KOSA_I2cInfo *i2cInfo);
KOSA_API Int32 KOSA_i2cClose(KOSA_I2cHndl *hndl);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /*_KOSA_I2C_H_*/

