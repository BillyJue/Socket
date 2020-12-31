
#include "sock.h"
#include "kosa_debug.h"
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
	/*���ݰ�ͷ*/
	Char a[2];
	/*Э���*/
	Char ctype;
}JSMsgHead;


typedef struct
{
	/*���ݰ�ͷ*/
	Uint8 bt;
	/*Э��� 601*/
	Char protocol[3];
	/*��֤���*/
	Char rzbh[15];
	/*Ӧ�� 1:���� 0:������*/
	Char apply;
	/*IP*/
	Char ip[15];
	/*�˿�*/
	Char port[5];
	/*���� yyyy-MM-dd*/
	char cDate[10];
	/*ʱ�� hh24:mi:ss.zzz*/
	char cTime[12];
	/*����У��*/
	char sjjy[8];
	/*���ݰ�β 0x3*/
	Uint8 bw;
}JsStartAttestAck;

typedef struct
{
	/*���ݰ�ͷ 0x2*/
	Uint8 bt;
	/*Э��� 102*/
	Char protocol[3];
	/*��֤���*/
	Char rzbh[15];
	/*����У�� ��BT��SJJY֮ǰ*/
	Char sjjy[8];
	/*���ݰ�β 0x3*/
	Uint8 bw;
}JsStopWorkReq;

typedef struct
{
	/*���ݰ�ͷ 0x2*/
	Uint8 bt;
	/*Э��� 103*/
	Char protocol[3];
	/*��֤���*/
	Char rzbh[15];
	/*���ݰ����ȣ�BH��BW֮ǰ�����ֽڵĳ���)*/
	Char dataLen[7];
	//Uint32 dataLen;
	/*�ɼ���� �豸�ı�� */
	Char deviceId[15];
	/*ͼƬID BH(15λ) + ������(2λ)+����ʱ��(17λ) yyyyMMddhhmisszzz*/
	Char tPicId[34];
	/*���ƺ��� */
	Char plateNum[15];
	/*������ɫ*/
	Char plateClr[2];
	/*����ʱ��*/
	Char passTime[19];
	/*����*/
	Char nSpeed[3];
	//Uint32 nSpeed;
	/*��������*/
	Char vehTerritory[3];
	/*������ɫ*/
	Char vehColor;
	/*������*/
	Char errCode[2];
	//Uint32 errCode;
	/*����ԭ��*/
	Char errReason[2];
	//Uint32 errReason;
	/*ʶ��ʱ��*/
	Char recogTime[10];
	//Uint32 recogTime;
	/*���ƽṹ*/
	Char plateStruct;
	//Uint32 plateStruct;
	/*��������*/
	Char nNum;
	/*Υ������*/
	Char illegalType[5];
	/*��������X*/
	Char plateX[5];
	/*��������Y*/
	Char plateY[5];
	/*��������*/
	Char vehLogType[2];
	/*����*/
	Char vehType;
	/*����*/
	Char limitSpeed[3];
	/*���ʱ��*/
	Char redTime[6];
	/*ͼƬ����*/
	Char picNum;
}JsVehInfoReq;

typedef struct
{
	/*1:������д 2:�ĺ�һ*/
	Char dwPicType[2];
	/*ͼƬ��С*/
	Char dwPicLen[7];
	/*ͼƬ���ʵ�ַ*/
	Char addr[128];
}JsPicInfo;

typedef struct
{
	/*¼������*/
	Char videoType;
	/*¼����ʵ�ַ*/
	Char videoAddr[256];
	/*¼�����ص�ַ*/
	Char videoDownAddr[128];

}JsVideoInfo;


typedef struct
{
	/*���ݰ�ͷ 0x2*/
	Uint8 bt;
	/*Э��� 603*/
	Char protocol[3];
	/*��֤���*/
	Char rzbh[15];
	/*�ɼ���� �豸�ı�� */
	Char bh[15];
	/*ͼƬID BH(15λ) + ������(2λ)+����ʱ��(17λ) yyyyMMddhhmisszzz*/
	Char tPicId[34];
	/*����У��*/
	Char sjjy[8];
	/*���ݰ�β 0x3*/
	Uint8 bw;

}JsVehInfoAck;

typedef struct
{
	/*���ݰ�ͷ 0x2*/
	Uint8 bt;
	/*Э��� 111*/
	Char protocol[3];
	/*��֤���*/
	Char rzbh[15];
	/*����У��*/
	Char sjjy[8];
	/*���ݰ�β 0x3*/
	Uint8 bw;
}JsHeart;
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
	case JS_START_ATTEST:
	{
		char s[100] = { "S 3 201601231624356 #" };
		KOSA_socketSend(&gtSockFd, s, strlen(s), 0);
		printf("-------JS_START_ATTEST----------\n");
	}
		break;
	case JS_HEART:
	{
		char s[100] = { "S 1 SYN #" };
		KOSA_socketSend(&gtSockFd, s, strlen(s), 0);
		printf("-------JS_HEART----------\n");
	}
		break;
	case JS_UPLOAD_VEHDATA:
	{
		JsVehInfoAck  tParam;
		memset(&tParam, 0, sizeof(JsVehInfoAck));
		tParam.bt = 0x2;
		tParam.protocol[0] = '6';
		tParam.protocol[1] = '0';
		tParam.protocol[2] = '3';
		tParam.bw = 0x3;
		KOSA_socketSend(&gtSockFd, (char*)&tParam, sizeof(JsVehInfoAck), 0);
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
		return sizeof(JSMsgHead);
	}
	else if (sizeof(JSMsgHead) == m_dwBodyLen)
	{

		JSMsgHead *ptHeader = (JSMsgHead*)m_pBody;
		if (ptHeader->ctype == '3')
		{
			m_dwMsgType = JS_START_ATTEST;
		}
		if (ptHeader->ctype == '1')
		{
			m_dwMsgType = JS_HEART;
		}
		
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


