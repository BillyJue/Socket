
#include "sock.h"
#include "kosa_debug.h"
#define TL_VERSION				0
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
	/*消息包的长度(消息头，消息体，CRC检验码)*/
	Uint16 msgLen;
	/*协议版本*/
	Char ver[2];
	/*消息类型*/
	Uint16 msgType;
	/*消息序号*/
	Uint16 msgReq;
	/*设备编号*/
	Char devId[18];
}VehSpeedDetMsgHead;


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
	Char pBody[1024];
	memset(pBody, 0, 1024);
	switch (m_dwMsgType)
	{
	case 0x0001:
	{
		Char *pStr = pBody;
		/*消息头*/
		Uint16 *pMsgLen = (Uint16*)pStr;
		pStr += 2;
		/*协议版本*/
		sprintf(pStr, "%s", "01");
		pStr += 2;
		/*消息类型*/
		Uint16 *pMsgType = (Uint16*)pStr;
		*pMsgType = htons(0x8001);
		pStr += 2;
		/*消息序号*/
		Uint16 *pSeq = (Uint16*)pStr;
		*pSeq = 0;
		pStr += 2;
		/*设备编号*/
		sprintf(pStr, "%s", "123456789098765432");
		pStr += 18;

		Uint8 *pResult = (Uint8*)pStr;
		*pResult = 0;
		pStr += 1;

		sprintf(pStr, "20200904100911");
		pStr += 14;
		//心跳时长
		Uint16 *pTmp = (Uint16*)pStr;
		*pTmp = htons(0x1001);
		pStr += 2;
		/**/
		pTmp = (Uint16*)pStr;
		*pTmp = htons(2);
		pStr += 2;
		/*value*/
		pTmp = (Uint16*)pStr;
		*pTmp = htons(40);
		pStr += 2;
		//心跳超时
		pTmp = (Uint16*)pStr;
		*pTmp = htons(0x1002);
		pStr += 2;
		/**/
		pTmp = (Uint16*)pStr;
		*pTmp = htons(2);
		pStr += 2;
		/*value*/
		pTmp = (Uint16*)pStr;
		*pTmp = htons(80);
		pStr += 2;
		//最大连接次数
		pTmp = (Uint16*)pStr;
		*pTmp = htons(0x1003);
		pStr += 2;
		/**/
		pTmp = (Uint16*)pStr;
		*pTmp = htons(2);
		pStr += 2;
		/*value*/
		pTmp = (Uint16*)pStr;
		*pTmp = htons(50);
		pStr += 2;

		/*MessageHash*/
		pTmp = (Uint16*)pStr;
		*pTmp = htons(0x0002);
		pStr += 2;
		/**/
		pTmp = (Uint16*)pStr;
		*pTmp = htons(2);
		pStr += 2;
		/*value*/
		pTmp = (Uint16*)pStr;
		*pTmp = htons(123);
		pStr += 2;

		int len = pStr - pBody;
		*pMsgLen = htons(len);

		KOSA_socketSend(&gtSockFd, (char*)pBody, len, 0);
	}
		break;
	case 0x8004:
	{
		printf("---------Set ACK----------\n");
		break;
	}

	case 0x0003:
	{
		if (0)
		{
			Char *pStr = pBody;
			/*消息头*/
			Uint16 *pMsgLen = (Uint16*)pStr;
			pStr += 2;
			/*协议版本*/
			sprintf(pStr, "%s", "01");
			pStr += 2;
			/*消息类型*/
			Uint16 *pMsgType = (Uint16*)pStr;
			*pMsgType = htons(0x8003);
			pStr += 2;
			/*消息序号*/
			Uint16 *pSeq = (Uint16*)pStr;
			*pSeq = 0;
			pStr += 2;
			/*设备编号*/
			sprintf(pStr, "%s", "123456789098765432");
			pStr += 18;

			Uint8 *pResultCode = (Uint8*)pStr;
			*pResultCode = 0;
			pStr += 1;

			/*MessageHash*/
			Uint16 *pTmp = (Uint16*)pStr;
			*pTmp = htons(0x0002);
			pStr += 2;
			/**/
			pTmp = (Uint16*)pStr;
			*pTmp = htons(2);
			pStr += 2;
			/*value*/
			pTmp = (Uint16*)pStr;
			*pTmp = htons(123);
			pStr += 2;

			int len = pStr - pBody;
			*pMsgLen = htons(len);

			KOSA_socketSend(&gtSockFd, (char*)pBody, len, 0);

			printf("-------HEART----------\n");
		}
		else
		{
			Char *pStr = pBody;
			/*消息头*/
			Uint16 *pMsgLen = (Uint16*)pStr;
			pStr += 2;
			/*协议版本*/
			sprintf(pStr, "%s", "AB");
			pStr += 2;
			/*消息类型*/
			Uint16 *pMsgType = (Uint16*)pStr;
			*pMsgType = htons(0x0004);
			pStr += 2;
			/*消息序号*/
			Uint16 *pSeq = (Uint16*)pStr;
			*pSeq = htons(11);
			pStr += 2;
			/*设备编号*/
			sprintf(pStr, "%s", "BBBBBBBBBBBBBBBBBS");
			pStr += 18;
			//-------------
			Uint8 *pMode = (Uint8*)pStr;
			*pMode = 1;
			pStr += 1;

			//通用限速值
			Uint16 *pTmp = (Uint16*)pStr;
			*pTmp = htons(0x1004);
			pStr += 2;
			/**/
			pTmp = (Uint16*)pStr;
			*pTmp = htons(2);
			pStr += 2;
			/*value*/
			Uint16 *pValue = (Uint16*)pStr;
			*pValue = htons(50);
			pStr += 2;

			//大车限速值
			pTmp = (Uint16*)pStr;
			*pTmp = htons(0x1005);
			pStr += 2;
			/**/
			pTmp = (Uint16*)pStr;
			*pTmp = htons(2);
			pStr += 2;
			/*value*/
			Uint16 *p2Value = (Uint16*)pStr;
			*p2Value = htons(50);
			pStr += 2;

			//小车限速值
			pTmp = (Uint16*)pStr;
			*pTmp = htons(0x1006);
			pStr += 2;
			/**/
			pTmp = (Uint16*)pStr;
			*pTmp = htons(2);
			pStr += 2;
			/*value*/
			p2Value = (Uint16*)pStr;
			*p2Value = htons(50);
			pStr += 2;

			/*MessageHash*/
			pTmp = (Uint16*)pStr;
			*pTmp = htons(0x0002);
			pStr += 2;
			/**/
			pTmp = (Uint16*)pStr;
			*pTmp = htons(2);
			pStr += 2;
			/*value*/
			pTmp = (Uint16*)pStr;
			*pTmp = htons(12);
			pStr += 2;

			int len = pStr - pBody;
			*pMsgLen = htons(len);

			KOSA_socketSend(&gtSockFd, (char*)pBody, len, 0);

			printf("-------SET----------\n");
		}
		
	}
		break;
	}
}

Int32 SockPlateTest::GetReadLenth()
{
	int len = 0;
	if (m_dwBodyLen == 0)
	{
		len = 26;	
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
	else if (m_dwBodyLen == 26)
	{
		VehSpeedDetMsgHead *ptHeader = (VehSpeedDetMsgHead*)m_pBody;
		m_dwMsgType = htons(ptHeader->msgType);
		len = htons(ptHeader->msgLen) - 26;
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
	///*心跳*/
	//char peer1_0[] = { /* Packet 295 */
	//	0x00, 0x7b, 0x76, 0x30, 0x80, 0x03, 0x00, 0x0d,
	//	0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38,
	//	0x39, 0x38, 0x37, 0x34, 0x35, 0x36, 0x31, 0x32,
	//	0x33, 0x31, 0x00, 0x00, 0x02, 0x00, 0x02, 0x2d,
	//	0x96 };
	///*下发*/
	//char peer1_1[] = { /* Packet 297 */
	//	0x00, 0x0c, 0x76, 0x33, 0x80, 0x04, 0x00, 0x0c,
	//	0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38,
	//	0x39, 0x34, 0x35, 0x36, 0x31, 0x32, 0x33, 0x30,
	//	0x32, 0x31, 0x01, 0x10, 0x04, 0x00, 0x02, 0x10,
	//	0x34, 0x10, 0x05, 0x00, 0x02, 0x10, 0x34, 0x10,
	//	0x06, 0x00, 0x02, 0x10, 0x34, 0x10, 0x07, 0x00,
	//	0x02, 0x10, 0x34, 0x10, 0x08, 0x00, 0x02, 0x10,
	//	0x34, 0x10, 0x09, 0x00, 0x02, 0x10, 0x34, 0x10,
	//	0x0a, 0x00, 0x02, 0x10, 0x34, 0x10, 0x0b, 0x00,
	//	0x02, 0x10, 0x34, 0x10, 0x0c, 0x00, 0x02, 0x10,
	//	0x34, 0x10, 0x0d, 0x00, 0x02, 0x10, 0x34, 0x10,
	//	0x0e, 0x00, 0x02, 0x10, 0x34, 0x00, 0x02, 0x00,
	//	0x02, 0xa5, 0x2d };

	//char peer0_4[] = { /* Packet 66 */
	//	0x00, 0x35, 0x30, 0x31, 0x00, 0x03, 0x00, 0x41,
	//	0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38,
	//	0x39, 0x38, 0x37, 0x36, 0x34, 0x34, 0x33, 0x32,
	//	0x00, 0x9e, 0x00, 0x32, 0x30, 0x32, 0x30, 0x30,
	//	0x39, 0x32, 0x37, 0x30, 0x39, 0x32, 0x36, 0x35,
	//	0x34, 0x10, 0x0e, 0x00, 0x02, 0x00, 0x00, 0x00,
	//	0x02, 0x00, 0x02, 0x51, 0x20 };

	//char peer0_1[] = { /* Packet 1491 */
	//	0x00, 0x35, 0x30, 0x31, 0x00, 0x03, 0x02, 0x44,
	//	0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38,
	//	0x39, 0x38, 0x37, 0x36, 0x34, 0x34, 0x33, 0x32,
	//	0x00, 0x08, 0x00, 0x32, 0x30, 0x32, 0x30, 0x31,
	//	0x30, 0x30, 0x39, 0x30, 0x36, 0x30, 0x30, 0x30,
	//	0x31, 0x10, 0x0e, 0x00, 0x02, 0x00, 0x00, 0x00,
	//	0x02, 0x00, 0x02, 0x0d, 0xd2 };

	//char peer11_11[] = { /* Packet 14 */
	//	0x00, 0xae, 0x30, 0x31, 0x00, 0x01, 0x10, 0x11,
	//	0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38,
	//	0x39, 0x38, 0x37, 0x36, 0x34, 0x34, 0x33, 0x32,
	//	0x00, 0x08, 0x00, 0x63, 0x32, 0x30, 0x32, 0x30,
	//	0x31, 0x30, 0x31, 0x34, 0x30, 0x36, 0x32, 0x33,
	//	0x34, 0x39, 0x20, 0x01, 0x00, 0x32, 0x4e, 0x55,
	//	0x4c, 0x4c, 0x00, 0x63, 0xec, 0x00, 0x00, 0x00,
	//	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	//	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	//	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	//	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	//	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	//	0x20, 0x02, 0x00, 0x20, 0x4e, 0x55, 0x4c, 0x4c,
	//	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	//	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	//	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	//	0x00, 0x00, 0x00, 0x00, 0x20, 0x03, 0x00, 0x20,
	//	0x4e, 0x55, 0x4c, 0x4c, 0x00, 0x00, 0x00, 0x00,
	//	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	//	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	//	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	//	0x00, 0x02, 0x00, 0x02, 0xc5, 0x6b };
	//char peer11_12[] = { /* Packet 87 */
	//	0x00, 0x63, 0x76, 0x32, 0x00, 0x04, 0x11, 0x01,
	//	0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38,
	//	0x39, 0x38, 0x37, 0x34, 0x35, 0x36, 0x31, 0x32,
	//	0x33, 0x32, 0x31, 0x10, 0x04, 0x00, 0x02, 0x00,
	//	0xc6, 0x10, 0x05, 0x00, 0x02, 0x00, 0xc6, 0x10,
	//	0x06, 0x00, 0x02, 0x00, 0xc6, 0x10, 0x07, 0x00,
	//	0x02, 0x00, 0xc6, 0x10, 0x08, 0x00, 0x02, 0x00,
	//	0xc6, 0x10, 0x09, 0x00, 0x02, 0x00, 0xc6, 0x10,
	//	0x0a, 0x00, 0x02, 0x00, 0xc6, 0x10, 0x0b, 0x00,
	//	0x02, 0x00, 0xc6, 0x10, 0x0c, 0x00, 0x02, 0x00,
	//	0xc6, 0x10, 0x0d, 0x00, 0x02, 0x00, 0xc6, 0x10,
	//	0x0e, 0x00, 0x02, 0x41, 0x48, 0x00, 0x02, 0x00,
	//	0x02, 0x8d, 0x12 };


	//double aab = 0.999;
	//int ac = aab * 100;

	//int a = 32772;
	//VehSpeedDetMsgHead *pHead = (VehSpeedDetMsgHead*)peer11_12;
	//int nType = htons(pHead->msgType);
	//int nLen = htons(pHead->msgLen);

	//char *ptr = peer11_12;
	//ptr += sizeof(VehSpeedDetMsgHead);

	//ptr += 1;
	////心跳的T
	//Uint16 nTag = 0;
	//memcpy(&nTag, ptr, 2);
	//nTag = htons(nTag);
	//ptr += 2;
	////心跳的L
	//Uint16 nTmpLen = 0;
	//memcpy(&nTmpLen, ptr, 2);//ptr
	//nTmpLen = htons(nTmpLen);
	//ptr += 2;

	//Uint16 nTmpValue = 0;
	//memcpy(&nTmpValue, ptr, 2);
	//nTmpValue = htons(nTmpValue);
	//ptr += 2;


	//
	// nTag = 0;
	//memcpy(&nTag, ptr, 2);
	//nTag = htons(nTag);
	//ptr += 2;

	// nTmpLen = 0;
	//memcpy(&nTmpLen, ptr, 2);
	//nTmpLen = htons(nTmpLen);
	//ptr += 2;

	// nTmpValue = 0;
	//memcpy(&nTmpValue, ptr, 2);
	//nTmpValue = htons(nTmpValue);
	//ptr += 2;

	//nTag = 0;
	//memcpy(&nTag, ptr, 2);
	//nTag = htons(nTag);
	//ptr += 2;

	//nTmpLen = 0;
	//memcpy(&nTmpLen, ptr, 2);
	//nTmpLen = htons(nTmpLen);
	//ptr += 2;

	//nTmpValue = 0;
	//memcpy(&nTmpValue, ptr, 2);
	//nTmpValue = htons(nTmpValue);
	//ptr += 2;


	//nTag = 0;
	//memcpy(&nTag, ptr, 2);
	//nTag = htons(nTag);
	//ptr += 2;

	//nTmpLen = 0;
	//memcpy(&nTmpLen, ptr, 2);
	//nTmpLen = htons(nTmpLen);
	//ptr += 2;

	//nTmpValue = 0;
	//memcpy(&nTmpValue, ptr, 2);
	//nTmpValue = htons(nTmpValue);
	//ptr += 2;

	////-1007--
	//nTag = 0;
	//memcpy(&nTag, ptr, 2);
	//nTag = htons(nTag);
	//ptr += 2;

	//nTmpLen = 0;
	//memcpy(&nTmpLen, ptr, 2);
	//nTmpLen = htons(nTmpLen);
	//ptr += 2;

	//nTmpValue = 0;
	//memcpy(&nTmpValue, ptr, 2);
	//nTmpValue = htons(nTmpValue);
	//ptr += 2;


	//nTag = 0;
	//memcpy(&nTag, ptr, 2);
	//nTag = htons(nTag);
	//ptr += 2;

	//nTmpLen = 0;
	//memcpy(&nTmpLen, ptr, 2);
	//nTmpLen = htons(nTmpLen);
	//ptr += 2;

	//nTmpValue = 0;
	//memcpy(&nTmpValue, ptr, 2);
	//nTmpValue = htons(nTmpValue);
	//ptr += 2;

	//nTag = 0;
	//memcpy(&nTag, ptr, 2);
	//nTag = htons(nTag);
	//ptr += 2;

	//nTmpLen = 0;
	//memcpy(&nTmpLen, ptr, 2);
	//nTmpLen = htons(nTmpLen);
	//ptr += 2;

	//nTmpValue = 0;
	//memcpy(&nTmpValue, ptr, 2);
	//nTmpValue = htons(nTmpValue);
	//ptr += 2;

	//nTag = 0;
	//memcpy(&nTag, ptr, 2);
	//nTag = htons(nTag);
	//ptr += 2;

	//nTmpLen = 0;
	//memcpy(&nTmpLen, ptr, 2);
	//nTmpLen = htons(nTmpLen);
	//ptr += 2;

	//nTmpValue = 0;
	//memcpy(&nTmpValue, ptr, 2);
	//nTmpValue = htons(nTmpValue);
	//ptr += 2;

	//nTag = 0;
	//memcpy(&nTag, ptr, 2);
	//nTag = htons(nTag);
	//ptr += 2;

	//nTmpLen = 0;
	//memcpy(&nTmpLen, ptr, 2);
	//nTmpLen = htons(nTmpLen);
	//ptr += 2;

	//nTmpValue = 0;
	//memcpy(&nTmpValue, ptr, 2);
	//nTmpValue = htons(nTmpValue);
	//ptr += 2;
	////日衣限速值
	//nTag = 0;
	//memcpy(&nTag, ptr, 2);
	//nTag = htons(nTag);
	//ptr += 2;

	//nTmpLen = 0;
	//memcpy(&nTmpLen, ptr, 2);
	//nTmpLen = htons(nTmpLen);
	//ptr += 2;

	//nTmpValue = 0;
	//memcpy(&nTmpValue, ptr, 2);
	//nTmpValue = htons(nTmpValue);
	//ptr += 2;

	//nTag = 0;
	//memcpy(&nTag, ptr, 2);
	//nTag = htons(nTag);
	//ptr += 2;

	//nTmpLen = 0;
	//memcpy(&nTmpLen, ptr, 2);
	//nTmpLen = htons(nTmpLen);
	//ptr += 2;

	//nTmpValue = 0;
	//memcpy(&nTmpValue, ptr, 2);
	//nTmpValue = htons(nTmpValue);
	//ptr += 2;

	//nTag = 0;
	//memcpy(&nTag, ptr, 2);
	//nTag = htons(nTag);
	//ptr += 2;

	//nTmpLen = 0;
	//memcpy(&nTmpLen, ptr, 2);
	//nTmpLen = htons(nTmpLen);
	//ptr += 2;

	//nTmpValue = 0;
	//memcpy(&nTmpValue, ptr, 2);
	//nTmpValue = htons(nTmpValue);
	//ptr += 2;


	//nTag = 0;
	//memcpy(&nTag, ptr, 2);
	//nTag = htons(nTag);
	//ptr += 2;

	//nTmpLen = 0;
	//memcpy(&nTmpLen, ptr, 2);
	//nTmpLen = htons(nTmpLen);
	//ptr += 2;

	//nTmpValue = 0;
	//memcpy(&nTmpValue, ptr, 2);
	//nTmpValue = htons(nTmpValue);
	//ptr += 2;

	//nTag = 0;
	//memcpy(&nTag, ptr, 2);
	//nTag = htons(nTag);
	//ptr += 2;

	//nTmpLen = 0;
	//memcpy(&nTmpLen, ptr, 2);
	//nTmpLen = htons(nTmpLen);
	//ptr += 2;

	//nTmpValue = 0;
	//memcpy(&nTmpValue, ptr, 2);
	//nTmpValue = htons(nTmpValue);
	//ptr += 2;

    KOSA_printf("%s:%d \n", __FUNCTION__, __LINE__);
	Init();
	return KOSA_SOK;
}

Int32 SockPlateTest::Stop()
{
    KOSA_printf("%s:%d \n", __FUNCTION__, __LINE__);
	return KOSA_SOK;
}


