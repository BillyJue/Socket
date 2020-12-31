
#ifndef __KOSA_SOCKET_H__
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#define __KOSA_SOCKET_H__

#include <kosa.h>
#ifdef _LINUX_
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#endif
#include <errno.h>

#ifdef _LINUX_
    #define ERRNO_GET                    errno
#else
    #define ERRNO_GET                    WSAGetLastError()
#endif

#ifdef _LINUX_
    #define INVALID_SOCKET               (-1)
    #define SOCKADDR_IN                  struct sockaddr_in
#endif

#ifdef _MSC_VER
#ifndef EINPROGRESS
#define EINPROGRESS                      WSAEINPROGRESS
#endif
#ifndef EWOULDBLOCK
#define EWOULDBLOCK                      WSAEWOULDBLOCK
#endif
#endif

typedef struct
{
#ifdef _LINUX_
    Int32 Hndl;
#endif
#if defined(_MSC_VER) || defined(_MINGW_)
    SOCKET Hndl;
#endif
}KOSA_Socket;

KOSA_API Int32 KOSA_socketRecvfrom(KOSA_Socket* SocketHndl, Char* buf, Int32 len, Int32 flags,
                        SOCKADDR_IN* dst_addr, Int32* addrlen);

KOSA_API Int32 KOSA_socketRecv(KOSA_Socket* SocketHndl, Char* buf, Int32 len, Int32 flags);

KOSA_API Int32 KOSA_socketSendto(KOSA_Socket* SocketHndl, const Char* buf, Int32 len, Int32 flags,
                    const SOCKADDR_IN* dst_addr, Int32 addrlen);

KOSA_API Int32 KOSA_socketSend(KOSA_Socket* SocketHndl, const Char* buf, Int32 len, Int32 flags);

KOSA_API Int32 KOSA_socketIOctl(KOSA_Socket* SocketHndl, Int32 command, void* argstruct);

KOSA_API Int32 KOSA_socketSetOpt(KOSA_Socket* SocketHndl, Int32 level, Int32 optname, Void* optval,Int32 optlen);

KOSA_API Int32 KOSA_socketConnect(KOSA_Socket* SocketHndl, SOCKADDR_IN* serv_addr, Int32 addrlen);

KOSA_API Int32 KOSA_socketAccept(KOSA_Socket* SocketHndl, KOSA_Socket* RetSocketHndl, SOCKADDR_IN* addr, Int32* addrlen);

KOSA_API Int32 KOSA_socketListen(KOSA_Socket* SocketHndl, Int32 backlog);

KOSA_API Int32 KOSA_socketBind(KOSA_Socket* SocketHndl, SOCKADDR_IN* addr, Int32 addrlen);

KOSA_API Int32 KOSA_socketClose(KOSA_Socket* SocketHndl);

KOSA_API Int32 KOSA_socketOpen (KOSA_Socket* SocketHndl, Int32 domain, Int32 type, Int32 proto);

KOSA_API Int32 KOSA_SockCleanup( );

KOSA_API Int32 KOSA_socketInit( );

/*将IP地址(本地字节序)转换成点分十进制的字符串输出*/
KOSA_API Void KOSA_ip2string(Uint32 src, Char* dst);

/*将IP地址(网络字节序)转换成点分十进制的字符串输出*/
KOSA_API Char* KOSA_ip2string_s(Uint32 *pdwIp, Char* IPstr);

/*点分十进制转换成IP，本地字节序*/
KOSA_API Uint32 KOSA_inet_addr(const Char* string);

/*新增接口，IPCV7使用KOSA_AddrListGet无法正常获取*/
KOSA_API Int32 KOSA_AddrGetbyEth0(Uint32 *ipaddr);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* __KOSA_SOCKET_H__ */


