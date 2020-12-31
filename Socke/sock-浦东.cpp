
#include "sock.h"
#include "kosa_debug.h"
#define TL_VERSION				1
#define JS_START_ATTEST			1
#define JS_STOP_ATTEST			2
#define JS_UPLOAD_VEHDATA		3
#define JS_HEART				4

#define LISTEN_PORT  19001
#pragma pack(push, 1)
typedef char Char;
typedef unsigned char Uint8;

typedef struct
{
	/*8位:aaaabbbb*/
	Char bt[8];
	/*必须填0*/
	Uint16 nRemain;
	/*版本号4位:0002*/
	Char version[4];
	/*协议号 601*/
	Uint16 nMsgType;
	/*认证编号*/
	Uint32 rzbh;
	/*保留字段1*/
	Uint32 nLeft;
	/*数据长度*/
	Uint32 nDataLen;
	/*应答1:允许 0:不允许*/
	Uint8 apply;
	/*IP 15位*/
	Char ip[15];
	/*端口号*/
	Uint16 nPort;
	/*23位:日期yyyy-MM-dd HH:mm:ss.fff*/
	Char tTime[23];
	/*数据校验*/
	Uint32 sjjy;
	/*8位:ccccdddd*/
	Char bw[8];
}TLStartWorkAck;

typedef struct
{
	/*8位:aaaabbbb*/
	Char bt[8];
	/*必须填0*/
	Uint16 nRemain;
	/*版本号4位:0002*/
	Char version[4];
	/*协议号 601*/
	Uint16 nMsgType;
	/*认证编号*/
	Uint32 rzbh;
	/*保留字段1*/
	Uint32 nLeft;
	/*数据长度*/
	Uint32 nDataLen;
	/*应答1:允许 0:不允许*/
	Uint8 apply;
	/*IP 15位*/
	Char ip[15];
	/*端口号*/
	Uint16 nPort;
	/*是否上传抓拍数据*/
	Uint32 dwUploadData;
	/*空拍时间*/
	Char snapTime[5];
	/*23位:日期yyyy-MM-dd HH:mm:ss.fff*/
	Char tTime[23];
	/*数据校验*/
	Uint32 sjjy;
	/*8位:ccccdddd*/
	Char bw[8];
}TLStartWorkAck2;


typedef struct
{
	/*8位:aaaabbbb*/
	Char bt[8];
	/*必须填0*/
	Uint16 nRemain;
	/*协议号 603 615*/
	Uint16 nMsgType;
	/*认证编号*/
	Uint32 rzbh;
	/*保留*/
	Uint32 nLeft;
	/*数据长度*/
	Uint32 nDataLen;
	/*采集点号*/
	Char deviceID[15];
	/*图片ID*/
	Char sjID[34];
	/*数据校验*/
	Uint32 sjjy;
	/*包尾*/
	Char bw[8];

}TLVehDataACK;


typedef struct
{
	/*8位:aaaabbbb*/
	Char bt[8];
	/*必须填0*/
	Uint16 nRemain;
	/*版本号4位:0002*/
	Char version[4];
	/*协议号 603 615*/
	Uint16 nMsgType;
	/*认证编号*/
	Uint32 rzbh;
	/*保留*/
	Uint32 nLeft;
	/*数据长度*/
	Uint32 nDataLen;
	/*采集点号*/
	Char deviceID[15];
	/*图片ID*/
	Char sjID[34];
	/*数据校验*/
	Uint32 sjjy;
	/*包尾*/
	Char bw[8];

}TLVehDataACK2;

typedef struct
{
	/*8位:aaaabbbb*/
	Char bt[8];
	/*必须填0*/
	Uint16 nRemain;
	/*协议号 611*/
	Uint16 nMsgType;
	/*认证编号*/
	Uint32 rzbh;
	/*保留*/
	Uint32 nLeft;
	/*数据长度*/
	Uint32 nDataLen;
	/*数据校验*/
	Uint32 sjjy;
	/*包尾*/
	Char bw[8];
}TLHeartACK;

typedef struct
{
	/*8位:aaaabbbb*/
	Char bt[8];
	/*必须填0*/
	Uint16 nRemain;
	/*版本号4位:0002*/
	Char version[4];
	/*协议号 611*/
	Uint16 nMsgType;
	/*认证编号*/
	Uint32 rzbh;
	/*保留*/
	Uint32 nLeft;
	/*数据长度*/
	Uint32 nDataLen;
	/*数据校验*/
	Uint32 sjjy;
	/*包尾*/
	Char bw[8];
}TLHeartACK2;


#pragma pack(pop)

static Void* AcceptEntryFun(Void* prm)
{
	SockPlateTest *pObj = (SockPlateTest*)prm;
	pObj->AcceptEntryProc();
	return NULL;
}

static Void* RecvFun(Void* prm)
{
	KOSA_printf("%s:%d\n", __FUNCTION__, __LINE__);
	SockPlateTest *pObj = (SockPlateTest*)prm;
	pObj->RecvProc();
	return NULL;
}


SockPlateTest::SockPlateTest()
{	
	gbInit = FALSE;
	gtSockFd.Hndl = INVALID_SOCKET;
	m_semHndl = NULL;
	m_pBody = NULL;
	m_dwMaxLen = 0;
	m_dwBodyLen = 0;
	m_dwMsgType = 0;
}

SockPlateTest::~SockPlateTest()
{
	Release();
}

Int32 SockPlateTest::Init()
{
	if (gbInit)
	{
		return KOSA_SOK;
	}

	Int32 nRet;

	gbInit = TRUE;

	m_pBody = (Char *)malloc(1024*1024*50);
	m_dwMaxLen = 1024 * 1024 * 5;

	gtSockFd.Hndl = INVALID_SOCKET;
	KOSA_socketInit();

	KOSA_TskObj task;
	task.entryName = NULL;
	task.policy = KOSA_TSK_SCHED_POLICY_NOMAL;
	task.pri = KOSA_TSK_NORMAL_PRI_MIN;
	task.stackSize = KOSA_TSK_STACK_SIZE_DEFAULT;
	task.prm = this;
	task.entryFunc = AcceptEntryFun;
	KOSA_tskCreate(&task);

	return 0;
}

Int32 SockPlateTest::Release()
{
	if (gbInit == FALSE)
	{
		return KOSA_SOK;
	}
	gbInit = FALSE;

	if (gtSockFd.Hndl != INVALID_SOCKET)
	{
		KOSA_socketClose(&gtSockFd);
		gtSockFd.Hndl = INVALID_SOCKET;
	}

	KOSA_waitMsecs(1000);
	free(m_pBody);
	m_pBody = NULL;
}








Int32 SockPlateTest::AcceptEntryProc()
{
	Int32 nRet;
	KOSA_Socket m_hSockFd;
	
	nRet = KOSA_socketOpen(&m_hSockFd, AF_INET, SOCK_STREAM, 0);
	if (KOSA_SFAIL == nRet)
	{
		KOSA_printf("%s:%d KOSA_socketOpen fail\n", __FUNCTION__, __LINE__);
		return KOSA_SFAIL;
	}

	struct sockaddr_in tAddr;
	memset(&tAddr, 0, sizeof(struct sockaddr_in));
	tAddr.sin_family = AF_INET;
	tAddr.sin_addr.s_addr = 0;
	tAddr.sin_port = htons(LISTEN_PORT);

	struct linger tLinger;
	tLinger.l_onoff = 1;
	tLinger.l_linger = 0;
	KOSA_socketSetOpt(&m_hSockFd, SOL_SOCKET, SO_LINGER, (void *)&tLinger, sizeof(tLinger));

	nRet = KOSA_socketBind(&m_hSockFd, (SOCKADDR_IN *)&tAddr, sizeof(struct sockaddr));
	if (nRet == KOSA_SFAIL)
	{
		KOSA_printf("%s:%d KOSA_socketBind %d fail\n", __FUNCTION__, __LINE__, LISTEN_PORT);
		KOSA_socketClose(&m_hSockFd);
		m_hSockFd.Hndl = INVALID_SOCKET;
		return KOSA_SFAIL;
	}

	KOSA_socketListen(&m_hSockFd, 4);

	while (gbInit)
	{
		Int32 nRet;
		fd_set fdReadSet;
		struct timeval tm;

		tm.tv_sec = 4;
		tm.tv_usec = 0;
		FD_ZERO(&fdReadSet);
		FD_SET(m_hSockFd.Hndl, &fdReadSet);

		nRet = select(FD_SETSIZE, &fdReadSet, NULL, NULL, &tm);
		if (nRet == 0)
		{
			continue;
		}

		if (nRet == KOSA_SFAIL)
		{
			KOSA_printf("%s:%d select fail\n", __FUNCTION__, __LINE__, LISTEN_PORT);
			KOSA_socketClose(&m_hSockFd);
			m_hSockFd.Hndl = INVALID_SOCKET;
			return KOSA_SFAIL;
		}

		if (m_hSockFd.Hndl == INVALID_SOCKET ||
			!FD_ISSET(m_hSockFd.Hndl, &fdReadSet))
		{
			continue;
		}

		Uint32 dwPeerIp;
		Uint16 wPeerPort;
		KOSA_Socket hsockNewClientFd;

		struct sockaddr_in tAddrClient;
		Int32 iAddrLen = sizeof(struct sockaddr_in);

		KOSA_socketAccept(&m_hSockFd, &hsockNewClientFd, (SOCKADDR_IN *)&tAddrClient, &iAddrLen);
		if (hsockNewClientFd.Hndl == INVALID_SOCKET)
		{
			continue;
		}
		
		dwPeerIp = tAddrClient.sin_addr.s_addr;
		wPeerPort = ntohs(tAddrClient.sin_port);
		Char szIp[32];
		KOSA_ip2string(dwPeerIp, szIp);
		KOSA_printf("accept info: %d:%s:%d\n", hsockNewClientFd.Hndl, szIp, wPeerPort);
		//test---------------
		


        if(gtSockFd.Hndl == INVALID_SOCKET)
        {
		    gtSockFd.Hndl = hsockNewClientFd.Hndl;

			KOSA_TskObj task;
			task.entryName = NULL;
			task.policy = KOSA_TSK_SCHED_POLICY_NOMAL;
			task.pri = KOSA_TSK_NORMAL_PRI_MIN;
			task.stackSize = KOSA_TSK_STACK_SIZE_DEFAULT;
			task.prm = this;
			task.entryFunc = RecvFun;
			KOSA_tskCreate(&task);
        }
        else
        {
            KOSA_socketClose(&hsockNewClientFd);
            KOSA_printf("already have a link\n");
        }
	}

	KOSA_socketClose(&m_hSockFd);
	m_hSockFd.Hndl = INVALID_SOCKET;

	KOSA_printf("%s:%d exit!\n", __FUNCTION__, __LINE__);

	return KOSA_SOK;
}

Char* SockPlateTest::NeedRealloc(Uint32 dwLenth)
{
	void *temp = realloc(m_pBody, dwLenth);
    if(temp == NULL)
    {
        return NULL;
    }

    m_pBody = (Char *)temp;
	
	return &m_pBody[m_dwBodyLen];
}


Bool SockPlateTest::CheckMaxSize(Uint32 dwLenth)
{
	if ((m_dwBodyLen + dwLenth) >= m_dwMaxLen)
	{
		Uint32 maxLen = m_dwBodyLen + dwLenth + 1;

		if (NeedRealloc(maxLen) == NULL)
		{
			return false;
		}
		else
		{
		    m_dwMaxLen = maxLen;
			return true;
		}
	}

	return true;
}

Void SockPlateTest::ClearBuf()
{
	m_dwBodyLen = 0;
	m_dwMsgType = 0;
}


void SockPlateTest::UnpackMsg()
{
	switch (m_dwMsgType)
	{
	case 111:
	{
		if (TL_VERSION)
		{
			TLHeartACK2 ack;
			memset(&ack, 0, sizeof(TLHeartACK2));
			sprintf(ack.bt, "aaaabbbb");
			ack.nMsgType = 611;
			ack.nDataLen = 12;
			KOSA_socketSend(&gtSockFd, (char*)&ack, sizeof(TLHeartACK2), 0);
		}
		else
		{
			TLHeartACK ack;
			memset(&ack, 0, sizeof(TLHeartACK));
			sprintf(ack.bt, "aaaabbbb");
			ack.nMsgType = 611;
			ack.nDataLen = 12;
			KOSA_socketSend(&gtSockFd, (char*)&ack, sizeof(TLHeartACK), 0);
		}
		printf("-------JS_HEART----------\n");
	}
		break;
	case 103:
	case 113:
	{
		if (TL_VERSION)
		{
			TLVehDataACK2  tParam;
			memset(&tParam, 0, sizeof(TLVehDataACK2));
			tParam.nMsgType = 603;
			tParam.nDataLen = 61;
			KOSA_socketSend(&gtSockFd, (char*)&tParam, sizeof(TLVehDataACK2), 0);
		}
		else
		{
			TLVehDataACK  tParam;
			memset(&tParam, 0, sizeof(TLVehDataACK));
			tParam.nMsgType = 603;
			tParam.nDataLen = 61;
			KOSA_socketSend(&gtSockFd, (char*)&tParam, sizeof(TLVehDataACK), 0);
		}
		
		printf("-------JS_UPLOAD_VEHDATA----------\n");
	}
		break;
	}
}

Int32 SockPlateTest::GetReadLenth()
{
	int len = 0;
	if (m_dwBodyLen == 0)
	{
		if (TL_VERSION)
		{
			len = 75;
		}
		else
		{
			len = 71;
		}
		
	}
	else if (71 == m_dwBodyLen)
	{
		Uint16 *msgType = (Uint16*)(m_pBody + 10);
		m_dwMsgType = *msgType;
		Uint32 *pLen = (Uint32*)(m_pBody + 67);
		len = *pLen;
	}
	else if (m_dwBodyLen == 75)
	{
		Uint16 *msgType = (Uint16*)(m_pBody + 14);
		m_dwMsgType = *msgType;
		Uint32 *pLen = (Uint32*)(m_pBody + 71);
		len = *pLen;
	}
	return len;
}

Int32 SockPlateTest::RecvProc()
{
	while (1)
	{
		Int32 nRet;
		fd_set fdReadSet;
		struct timeval tm;
		tm.tv_sec = 1;
		tm.tv_usec = 0;
		FD_ZERO(&fdReadSet);
		FD_SET(gtSockFd.Hndl, &fdReadSet);
		nRet = select(FD_SETSIZE, &fdReadSet, NULL, NULL, &tm);
		if (nRet == 0)
		{
			KOSA_printf("%s:%d nRet = 0\n", __FUNCTION__, __LINE__);
			continue;
		}
		else if (nRet == -1)
		{
			KOSA_socketClose(&gtSockFd);
			gtSockFd.Hndl = INVALID_SOCKET;
			KOSA_printf("%s:%d nRet = -1\n", __FUNCTION__, __LINE__);
			return KOSA_SFAIL;
		}

		ClearBuf();
		Char *szReadBuf = m_pBody;
		Uint32 dwRemainLen = 0;
		dwRemainLen = GetReadLenth();
		while (dwRemainLen != 0)
		{
		    //KOSA_printf("%s:%d dwRemainLen = %d\n", __FUNCTION__, __LINE__, dwRemainLen);
			nRet = KOSA_socketRecv(&gtSockFd, (Char *)szReadBuf, dwRemainLen, 0);
			//KOSA_printf("%s:%d nRet = %d\n", __FUNCTION__, __LINE__, nRet);
			if (nRet == -1 || nRet == 0)
			{
				KOSA_socketClose(&gtSockFd);
				gtSockFd.Hndl = INVALID_SOCKET;
				KOSA_printf("%s:%d nRet = %d\n", __FUNCTION__, __LINE__, nRet);
				return KOSA_SFAIL;
			}
			
			dwRemainLen -= nRet;
			szReadBuf += nRet;
			m_dwBodyLen += nRet;

			if (dwRemainLen == 0)
			{
				dwRemainLen = GetReadLenth();
				if(dwRemainLen == -1)
				{
				    KOSA_socketClose(&gtSockFd);
    				gtSockFd.Hndl = INVALID_SOCKET;
    				return KOSA_SFAIL;
				}

				if (CheckMaxSize(dwRemainLen))
				{
					szReadBuf = &m_pBody[m_dwBodyLen];
				}
			}

			if (dwRemainLen == 0)
			{
				//static int nCount = 0;
				//nCount++;
				//KOSA_printf("recv num ----------:%d\n", nCount);
				UnpackMsg();
			}
		}
	}

	KOSA_printf("%s:%d exit!\n", __FUNCTION__, __LINE__);

	return KOSA_SOK;
}

Int32 SockPlateTest::Start()
{
    KOSA_printf("%s:%d \n", __FUNCTION__, __LINE__);
	Init();
	return KOSA_SOK;
}

Int32 SockPlateTest::Stop()
{
    KOSA_printf("%s:%d \n", __FUNCTION__, __LINE__);
	return KOSA_SOK;
}


