
#ifndef _KOSA_DIR_H_
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#define _KOSA_DIR_H_

#include <kosa.h>
#if defined(_MSC_VER) || defined(_MINGW_)
#include <time.h>
#include "direct.h"
#include "io.h"
#include "stdio.h"
#else //_MSC_VER
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/statfs.h>
#include <dirent.h>
#include <errno.h>
#include <string.h>
#endif //_MSC_VER

#ifndef _WIN32
	// Define the pathname separator characters for UNIX.
#  define ACE_DIRECTORY_SEPARATOR_CHAR '/'
#else
	// Define the pathname separator characters for Win32 (ugh).
#  define ACE_DIRECTORY_SEPARATOR_CHAR '\\'
#endif // _WIN32

#if defined(_MSC_VER) || defined(_MINGW_)

#define     R_OK                                4 /* Test for read permission. */
#define     W_OK								2 /* Test for write permission. */
#define		X_OK								1 /* Test for execute permission. */
#define		F_OK								0 /* Test for existence. */

#define     access                              _access
#define     itoa                                _itoa
#define     mkdir								_mkdir
#define		rmdir								_rmdir
#define		unlink								_unlink

#endif

#if defined(_MSC_VER) || defined(_MINGW_)
	typedef struct{
		unsigned short d_ino;
		unsigned short d_off;
		unsigned short d_reclen;
		// This must be a char * and not a one element
		// char array.  It causes problems on wide
		// character builds with Visual C++ 6.0.
		char *d_name;
	}dirent;

#define KOSA_DIRENT dirent

	typedef struct  {
		/// The name of the directory we are looking into
		char *directory_name_;

		/// Remember the handle between calls.
		HANDLE current_handle_;

		/// The struct for the results
		KOSA_DIRENT *dirent_;

		/// The struct for intermediate results.
		WIN32_FIND_DATAA fdata_;

		/// A flag to remember if we started reading already.
		int started_reading_;
	}KOSA_DIR;

	//#define ACE_Export ACE_Proper_Export_Flag
	//#define ACE_Proper_Import_Flag __declspec (dllimport)
	//#define ACE_Proper_Export_Flag __declspec (dllexport)
#define ACE_Export

#else

#define KOSA_DIRENT struct dirent
#define KOSA_DIR DIR
#define ACE_Export

#endif

KOSA_API Void KOSA_closedir(KOSA_DIR *);

KOSA_API KOSA_DIR *KOSA_opendir (const char *filename);

KOSA_API KOSA_DIRENT *KOSA_readdir (KOSA_DIR *);

KOSA_API Int32 KOSA_mkdir(const char *path);
KOSA_API Uint32 KOSA_GetLastError();
KOSA_API Bool KOSA_DirectoryExist(const char* dirName);
KOSA_API long KOSA_DeleteDirectory(const char* dirName);
KOSA_API int KOSA_CreateDirectory(const char* dirName);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* _KOSA_DIR_H_ */



