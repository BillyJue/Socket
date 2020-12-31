#include <iostream>
using namespace std;
#include "sock.h"
int main()
{
	char pBody[1024] = { 0 };
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
	
	SockPlateTest sock;
	sock.Start();
	system("pause");
	return 0;
}