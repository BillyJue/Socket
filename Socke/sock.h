
#ifndef __RECV_YUV_H__
#define __RECV_YUV_H__

#include <kosa.h>
#include <kosa_buf.h>
#include <kosa_tsk.h>
#include <kosa_socket.h>


class SockPlateTest
{
public:
	SockPlateTest();
	
	~SockPlateTest();


	Int32 Start();

	Int32 Stop();

	Int32 AcceptEntryProc();

	Int32 RecvProc();

private:
	Int32 Init();

	Int32 Release();


	Char* NeedRealloc(Uint32 dwLenth);

	Bool CheckMaxSize(Uint32 dwLenth);

	Void ClearBuf();

	void UnpackMsg();

	Int32 GetReadLenth();

private:
	KOSA_BufHndl *gptBufHndl;
	Bool gbInit;
	KOSA_Socket gtSockFd;
	KOSA_SemHndl *m_semHndl;
	Char *m_pBody;
	Uint32 m_dwMaxLen;
	Uint32 m_dwBodyLen;
	Uint32 m_dwMsgType;
};


#endif /*__RECV_YUV_H__*/

