#include "stdheader.h"
#include "SocketBase.h"
#include "SocketWrapper.h"
#include "Debug.h"
using namespace debug;
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CSocketBase::CSocketBase()
{

}

CSocketBase::~CSocketBase()
{

}
int CSocketBase::_Recv( SOCKET s,  char *buffer, int length ,int timeout )
{
	return Recv( s, buffer, length, timeout );
}
int CSocketBase::_RecvFrom( SOCKET s,  char *buffer, int length  , struct sockaddr* AddrFrom, int *AddrLen,int timeout)
{
	return RecvFrom( s, buffer, length,AddrFrom, AddrLen, timeout );
}

int CSocketBase::_Send( SOCKET s, char *buffer, int length  ,int timeout)
{
	return Send( s, buffer, length, timeout );
}
int CSocketBase::_SendTo( SOCKET s, char *buffer, int length  ,const struct sockaddr *To, int ToLen,int timeout)
{
	return SendTo( s, buffer, length,To,ToLen, timeout );
}
/** @brief ��socket�н���һ���ֽ�
*/
BOOL CSocketBase::_GetOneByteFromSocket( SOCKET s, int &n  ,int timeout )
{
	int ret = 0;
	char buffer[10] = {0};

	ret = _Recv( s, buffer, 1 ,timeout );
	if( ret != 1 )
	{
		PrintfW( LEVEL_ERROR,_T("CSocketBase::_GetOneByteFromSocket Failed.") );	
		return FALSE;
	}

	n = buffer[0];

	return TRUE;
}

void CSocketBase::_SetSocketTimeout( SOCKET s, int nTimeout  /** Ĭ��5��*/,int t /** 0: �����պͷ���ʱ, 1: ֻ������, 2: ֻ���÷�*/ )
{
	int len = sizeof(int);
	int timeout = nTimeout * 1000 ; // 10 seconds.

	if( t == 0 )
	{
		// ���ý��ճ�ʱ
		setsockopt( s,SOL_SOCKET,SO_RCVTIMEO,(char*)&timeout,len);
		// ���÷��ͳ�ʱ
		setsockopt( s,SOL_SOCKET,SO_SNDTIMEO,(char*)&timeout,len);
	} 
	else if( t == 1 )
	{
		// ���ý��ճ�ʱ
		setsockopt( s,SOL_SOCKET,SO_RCVTIMEO,(char*)&timeout,len);
	}
	else if( t == 2 )
	{
		// ���÷��ͳ�ʱ
		setsockopt( s,SOL_SOCKET,SO_SNDTIMEO,(char*)&timeout,len);
	}
}

/** @brief ����������׽���
*/
SOCKET CSocketBase::CreateSocket(BOOL bIsUdp, BOOL bNonBlocking )
{
	SOCKET s = socket( AF_INET, bIsUdp? SOCK_DGRAM: SOCK_STREAM, bIsUdp? IPPROTO_UDP : IPPROTO_TCP );
	if( s == INVALID_SOCKET )
	{
		return INVALID_SOCKET;
	}

	//set the socket in non-blocking  
	if( bNonBlocking ) 
	{
		SetNoBlocking( s , TRUE );
	}

	PrintfW( LEVEL_INFO,_T("CSocketBase::CreateSocket Create socket: %d."), s );	

	return s;
}

void CSocketBase::SetNoBlocking( SOCKET s,BOOL bIsNonBlocking  /** TRUE: NON- BLOCKING, FALSE: BLOCKING */ )
{
	unsigned long iMode = 1;  

	if( bIsNonBlocking )
	{
		iMode = 1;
	}
	else 
	{
		iMode = 0;
	}

	ioctlsocket( s , FIONBIO,  &iMode);  

	return;
}

/** @brief ���׽���
*/
BOOL CSocketBase::BindSocket( SOCKET s, u_long addr, u_short port )
{
	struct sockaddr_in bind_addr;
	bind_addr.sin_family=AF_INET;
	bind_addr.sin_addr.s_addr = addr; // �������ַ
	bind_addr.sin_port=htons( port ); // ������PORT

	if(bind( s ,(struct sockaddr *)&bind_addr,sizeof(struct sockaddr_in)) == SOCKET_ERROR )
	{
		return FALSE;
	}

	return TRUE;
}


/*  @brief Waits until socket states completion of operation
* @param[in]		hSock			Socket for calling "select"
* @param[in] nTimeout timeout in secods.
* @return							0 on success, SOCKET_ERROR on error (use WSAGetLastError to get error info)
*/
int CSocketBase::WaitCompletion(SOCKET hSock , int nTimeout /* seconds */)
{
	fd_set sRead;
	fd_set sWrite;
	fd_set sError;
	struct timeval tval;

	sError.fd_count=sWrite.fd_count=sRead.fd_count=1;
	*sError.fd_array=*sWrite.fd_array=*sRead.fd_array=hSock;

	tval.tv_sec = 0;
	tval.tv_usec = nTimeout * 1000 * 1000 ; 

	//call original select function
	if( select(0, &sRead, &sWrite, &sError, &tval ) <= 0 )
		return SOCKET_ERROR;

	if(FD_ISSET( hSock, &sError))
	{
		//we need an error code for the caller
		WSASetLastError(WSAEHOSTDOWN);
		return SOCKET_ERROR;
	}

	return 0;
}
/** @brief ����TCP NAGLE �㷨
*  ������ͨ�� TCP socket �ֶ�η��ͽ��ٵ�����ʱ������С�� 1460 ���� 100 ���ڣ��Զ˿��ܻ�ܳ�ʱ���ղ������ݣ����±���Ӧ�ó�����Ϊ��ʱ������ʱ�������ܵ��� TCP NAGLE �㷨��Ӱ�졣
*/
void CSocketBase::SetNodelay(SOCKET s)
{
	int on = 1;  

	setsockopt(s, IPPROTO_TCP, TCP_NODELAY, (const char *)&on, sizeof(on));  

	return;
}