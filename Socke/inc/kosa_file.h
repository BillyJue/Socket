
#ifndef _KOSA_FILE_H_
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#define _KOSA_FILE_H_

#include <kosa.h>

KOSA_API Int32 KOSA_fileReadFile(const Char *fileName, Uint8 *addr, Uint32 readSize, Uint32 *actualReadSize);

KOSA_API Int32 KOSA_fileWriteFile(const Char *fileName, Uint8 *addr, Uint32 size, Bool isAdd);

KOSA_API Int32 KOSA_fileGetLength(const Char *fileName, Uint32 *length);

KOSA_API Int32 KOSA_fileCopy(const Char *srcFile, const Char *dstFile);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* _KOSA_FILE_H_ */



