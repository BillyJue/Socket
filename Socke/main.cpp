#include <iostream>
using namespace std;
#include "sock.h"
int main()
{
	char pBody[1024] = { 0 };
	Char *pStr = pBody;
	/*��Ϣͷ*/
	Uint16 *pMsgLen = (Uint16*)pStr;
	pStr += 2;
	/*Э��汾*/
	sprintf(pStr, "%s", "01");
	pStr += 2;
	/*��Ϣ����*/
	Uint16 *pMsgType = (Uint16*)pStr;
	*pMsgType = htons(0x8001);
	pStr += 2;
	/*��Ϣ���*/
	Uint16 *pSeq = (Uint16*)pStr;
	*pSeq = 0;
	pStr += 2;
	/*�豸���*/
	sprintf(pStr, "%s", "123456789098765432");
	pStr += 18;

	Uint8 *pResult = (Uint8*)pStr;
	*pResult = 0;
	pStr += 1;

	sprintf(pStr, "20200904100911");
	pStr += 14;
	//����ʱ��
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
	//������ʱ
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
	//������Ӵ���
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