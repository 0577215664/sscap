#include "stdheader.h"
#include "Debug.h"
#include "SocketWrapper.h"
#include "SocketBase.h"
#include "SocksClient.h"
#include "BaseDef.h"
#include "SSClient.h"
#include "SSNodeSelector.h"
#include "EncyptionMgr.h"
#include <assert.h>
#include "Utils.h"
#include "privoxy.h"
#include "pac.h"
#include "proxynscache.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace debug;
CSSCLient::CSSCLient( SOCKET s ,const char *username /* proxy user */, const char *password /* proxy pass */) 
	: CSocksClient( s , username, password )
{
	pNodeInfo = NULL;
	pCryptor = NULL;
	bIsCipher = TRUE;
	bUserBreaking = FALSE;
}

CSSCLient::~CSSCLient()
{
	if( pCryptor )
	{
		delete pCryptor;
		pCryptor = NULL;
	}
}

/** @brief ����socks 5����
*/
BOOL CSSCLient::Start()
{
	// ѡȡһ��SS�ڵ�
	CSSNodeInfo *pServerNode = CSSNodeSelector::SelectNode();
	if( !pServerNode )
	{
		PrintfW( LEVEL_ERROR, _T("CSSCLient::Start: Can not get current Shadowsocks server node.") );
		return FALSE;
	}
	
	// ����shadowsocks ��������Ϣ
	//ssNodeInfo = *pServerNode;
	pNodeInfo = pServerNode;

	pCryptor = CEncryptionMgr::Create( pNodeInfo->method,pNodeInfo->password );

	if( !pCryptor )
	{
		PrintfW( LEVEL_ERROR, _T("CSSCLient::Start: Create Encryption error.") );
		return FALSE;
	}

	pNodeInfo->UpdateConnections( 1 );
	BOOL bRet = __super::Start();

	pNodeInfo->UpdateConnections( 2 );

	return bRet;
}

/** @brief ��������ĵ�ַ. �п�����һ������, �������IP
* ��������ɹ�, ����Ĵ�ַ��Ϣͨ��sockAddr����
*/
BOOL CSSCLient::TranslateSSServerAddress( SOCKADDR_IN &sockAddr , char *szAddress, u_short port )
{
	memset( &sockAddr,0,sizeof(sockAddr) );

	sockAddr.sin_family = AF_INET;
	sockAddr.sin_port = htons((u_short)port);
	sockAddr.sin_addr.s_addr = inet_addr(szAddress);

	if (sockAddr.sin_addr.s_addr == INADDR_NONE)
	{
		// ��Cache�в���
		string dest_ip;
		if( GetProxyNSIPForDomain( szAddress ,dest_ip ) )
		{
			sockAddr.sin_addr.s_addr = inet_addr( dest_ip.c_str() );
		}
		else 
		{
			LPHOSTENT lphost;
			lphost = gethostbyname(szAddress);
			if (lphost != NULL)
			{
				sockAddr.sin_addr.s_addr = ((LPIN_ADDR)lphost->h_addr)->s_addr;

				if( !IsProxyNSRecordExist( szAddress ) )
				{
					string ip = string( inet_ntoa( sockAddr.sin_addr ));
					PushProxyNSCache( szAddress, ip );
				}
			}
			else
			{
				return FALSE;
			}
		}
	}

	return TRUE;
}
/** @brief ���ӵ�ss ������
*/
BOOL CSSCLient::ConnectToSSServer( BOOL bUdp,BOOL bNonBlocking,SOCKADDR_IN *pSockAddrIn )
{
	PrintfW( LEVEL_INFO, _T("CSSCLient::ConnectToSSServer...") );

	CSSConfigInfo *pCfg = GetConfigInfo();

	int times = 1;
	SOCKET s = INVALID_SOCKET;

	do{
		PrintfA( LEVEL_INFO, ("CSSCLient::ConnectToSSServer: the %d time connect to SS server: %s:%d.") ,times ,(char *)pNodeInfo->server.c_str(), pNodeInfo->server_port);

		// create tcp no-blocking socket.
		s = CreateSocket( bUdp, bNonBlocking );

		if( s == INVALID_SOCKET )
		{
			_MakeSocksRspV5( SOCKS_ERR_GENERAL, 0, 0 );

			PrintfW( LEVEL_ERROR, _T("CSSCLient::ConnectToSSServer: Create remote socket error.") );
			return FALSE;
		}
		PrintfW( LEVEL_INFO, _T("CSSCLient::ConnectToSSServer: Remote socket created. Socket(%d)..."), s );

		if( !BindSocket( s ))
		{
			_MakeSocksRspV5( SOCKS_ERR_GENERAL, 0, 0 );

			CLOSESOCKET( s );

			return FALSE;
		}
		SOCKADDR_IN ServerAddr;
		pNodeInfo->UpdateConnectedTimes( 1 );

		if( !TranslateSSServerAddress( ServerAddr, (char *)pNodeInfo->server.c_str(), pNodeInfo->server_port ) )
		{
			_MakeSocksRspV5( SOCKS_ERR_HOST_UNREACHABLE, 0, 0 );

			PrintfW( LEVEL_ERROR, _T("CSSCLient::ConnectToSSServer: TranslateSSServerAddress error.") );

			CLOSESOCKET( s );

			if( times > pCfg->reconnectTimes )
			{
				pNodeInfo->UpdateFailureTimes( 1 );
				return FALSE;
			}
			else
			{
				pNodeInfo->UpdateFailureTimes( 1 );
				times ++;
				continue;
			}
		}

		if( pSockAddrIn )
		{
			memcpy( pSockAddrIn, &ServerAddr, sizeof( ServerAddr ) );
		}

		//connect SOCKS server
		int iRet =  0;
		DWORD start = GetTickCount();
		DWORD end = 0;
		DWORD latency = 0;
		iRet = connect( s, (const struct sockaddr*)&ServerAddr, sizeof(ServerAddr) );
		
		if (iRet == SOCKET_ERROR) 
		{
			// �첽SOCKET,�ȴ�10���������.
			if( WSAGetLastError() == WSAEWOULDBLOCK )
			{
				if( WaitCompletion( s, 10 ) == SOCKET_ERROR )
				{
					_MakeSocksRspV5( SOCKS_ERR_CONNECTION_REFUSED, 0, 0 );

					PrintfW( LEVEL_ERROR, _T("CSSCLient::ConnectToSSServer: Connect to Shadowsocks server error.") );

					CLOSESOCKET( s );

					if( times > pCfg->reconnectTimes )
					{
						pNodeInfo->UpdateFailureTimes( 1 );
						pNodeInfo->SetLastStatus( 0 );
						return FALSE;
					}
					else
					{
						pNodeInfo->UpdateFailureTimes( 1 );
						times ++;
						continue;
					}
				}
				else 
				{
					end = GetTickCount();
					latency = end - start;

					pNodeInfo->UpdateLatency( latency );
					pNodeInfo->SetLastStatus( 1 );

					break;
				}
			}
			else
			{
				_MakeSocksRspV5( SOCKS_ERR_CONNECTION_REFUSED, 0, 0 );

				PrintfW( LEVEL_ERROR, _T("CSSCLient::ConnectToSSServer: Connect to Shadowsocks server error code: %d."),WSAGetLastError() );

				CLOSESOCKET( s );

				if( times > pCfg->reconnectTimes )
				{
					pNodeInfo->UpdateFailureTimes( 1 );
					pNodeInfo->SetLastStatus( 0 );
					return FALSE;
				}
				else{
					pNodeInfo->UpdateFailureTimes( 1 );
					times ++;
					continue;
				}
			}
		}
		else
		{
			end = GetTickCount();
			latency = end - start;
			pNodeInfo->UpdateLatency( latency );
			pNodeInfo->SetLastStatus( 1 );

			break;
		}

		times ++;

		if( times > pCfg->reconnectTimes )
		{
			pNodeInfo->UpdateFailureTimes( 1 );
			pNodeInfo->SetLastStatus( 0 );
			return FALSE;
		}
	} while( 1 );

	if( bNonBlocking )
	{
		// ������ɺ�, ��SOCKET����Ϊ������. ����֮��Ľ�������,����Ϊ���������޷��������.
		SetNoBlocking( s, FALSE );
	}

	// set no delay
	SetNodelay( s );

	hRemoteSocket = s;

	PrintfW( LEVEL_INFO, _T("CSSCLient::ConnectToSSServer: Socket: %d has connected to ss server"),s );

	return TRUE;
}
/** @brief ִ��shadowsocks�ķ��Ͳ���
* ��Ϊ����Զ��SS�������ͷ���SOCKS CLIENT
* SS CLIENT -> SS SERVER :����Ҫִ�м��ܹ���
* SS SERVER -> SS CLIENT :  ��Ҫִ�н��ܹ���
*/
int CSSCLient::SSSend( SOCKET s, char *buffer, int length  ,int timeout )
{
	assert( pCryptor != NULL );

	if( buffer == NULL || length <= 0 )
		return 0;

	// ��һ�η���ʱ���Ƨһ��IV, ��������Ԥ����һ��buffer
	char *pOutBuf = new char[ length + 100 ];
	if( pOutBuf == NULL )
		return 0;

	int OutLen = 0;
	memset( pOutBuf, 0 , length + 100 );

	// �����Ҫ����Զ��SS������, �����
	if( bIsCipher && IsRemoteSocket( s ) )
	{
		pCryptor->Encrypt( buffer, length, pOutBuf, OutLen );
	}
	// ���͸�SOCKS CLIENT, �������,��ΪRecvʱ�Ѿ�������
	else 
	{
		memcpy( pOutBuf, buffer, length );
		OutLen = length;
	}

	//DWORD start = GetTickCount();
	int ret = _Send( s, pOutBuf, OutLen, timeout );

	// �ɹ�, ��ͳ������
	if( ret > 0 && IsRemoteSocket( s ))
	{
		pNodeInfo->PushData( ret );
		pNodeInfo->UpdateUpload_traffic( ret );
	}

	//nUpdateTimes ++;
	delete []pOutBuf;

	return ret;
}
int CSSCLient::SSSendTo( SOCKET s, char *buffer, int length  ,int timeout )
{
	assert( pCryptor != NULL );

	if( buffer == NULL || length <= 0 )
		return 0;

	char *pOutBuf = new char[ length + 100 ];
	if( pOutBuf == NULL )
		return 0;

	int OutLen = 0;
	memset( pOutBuf, 0 , length + 100 );

	// �����Ҫ����Զ��SS������, �����
	if( IsRemoteSocket( s ) )
	{
		// ����ǰ��Ҫ��UDPͷ�е�ǰ�����ֽ�ȥ��
		if( !Remove3BytesForUdpHeader(buffer, length ) )
		{
			delete []pOutBuf;

			return SOCKET_ERROR;
		}

		length -= 3;

		// UDPÿ����ͷ���������ܵ�IV��Ϣ,����ҪRESET��.
		pCryptor->ResetCryption();
		pCryptor->Encrypt( buffer, length, pOutBuf, OutLen );
	}
	// ���͸�SOCKS CLIENT, �������,��ΪRecvʱ�Ѿ�������
	else 
	{
		memcpy( pOutBuf, buffer, length );
		OutLen = length;
	}
	int ret = SOCKET_ERROR;

	DWORD start = GetTickCount();

	// ����udp client
	if( IsUdpClientSocket( s ) )
	{
		ret = sendto( s, pOutBuf, OutLen,0, (const sockaddr *)&udpSrcAddr, sizeof( udpSrcAddr ) );
	}
	// ����remote ss server
	else if( IsRemoteSocket( s ) )
	{
		ret = sendto( s, pOutBuf, OutLen,0, (const sockaddr *)&udpDstAddr, sizeof( udpDstAddr ) );
	}

	// ͳ������
	if( ret > 0 && IsRemoteSocket( s ))
	{
		pNodeInfo->PushData( ret );
		pNodeInfo->UpdateUpload_traffic( ret );
	}

	delete []pOutBuf;

	return ret;
}

/** @brief ִ��shadowsocks�Ľ��ղ���
* ��Ϊ����Զ��SS�������ͽ���SOCKS CLIENT
* SS CLIENT -> SS SERVER :����Ҫִ�м��ܹ���
* SS SERVER -> SS CLIENT :  ��Ҫִ�н��ܹ���
*/
int CSSCLient::SSRecv( SOCKET s,  char *buffer, int length  ,int timeout )
{
	assert( pCryptor != NULL );

	if( buffer == NULL || length <= 0 )
	{
		PrintfW( LEVEL_ERROR, _T("CSSCLient::SSRecv buffer == NULL || length <= 0") );
		return 0;
	}

	char *pOutBuf = new char[ length  ];
	if( pOutBuf == NULL )
	{
		PrintfW( LEVEL_ERROR, _T("CSSCLient::SSRecv pOutBuf == NULL ") );
		return 0;
	}

	int OutLen = 0;
	memset( pOutBuf, 0 , length  );

	DWORD start = GetTickCount();
	DWORD dwLastError = 0;

	int ret = _Recv( s, pOutBuf, length, timeout );
	dwLastError = WSAGetLastError();

	if( ret == 0 )
	{
		PrintfW( LEVEL_ERROR, _T("CSSCLient::SSRecv Socket: %d _Recv error connection closed by peer.") ,s );

		delete []pOutBuf;
		return SOCKET_ERROR;
	}

	if( ret < 0 )
	{
		delete []pOutBuf;

		PrintfW( LEVEL_ERROR, _T("CSSCLient::SSRecv Socket: %d _Recv error, lastcode: %d"),s,dwLastError );

		WSASetLastError( dwLastError );

		return SOCKET_ERROR;
	}

	// ͳ������
	if( ret > 0 && IsRemoteSocket( s ) )
	{
		pNodeInfo->PushData( ret );
		pNodeInfo->UpdateDownload_traffic( ret );
	}

	// �����Զ��SS��������������, �����
	if( bIsCipher && IsRemoteSocket( s ) )
	{
		pCryptor->Decrypt( pOutBuf, ret, buffer, OutLen );
	}
	// �����SS CLIENT����, �������. ֮����ʱ����ܵ�
	else
	{
		memcpy( buffer, pOutBuf, ret );
		OutLen = ret;
	}
	delete [] pOutBuf;

	WSASetLastError( dwLastError );

	return OutLen;
}

int CSSCLient::SSRecvFrom( SOCKET s,  char *buffer, int length  ,int timeout )
{
	assert( pCryptor != NULL );

	if( buffer == NULL || length <= 0 )
		return 0;

	// ��֪��UDP���յ��İ��Ƕ��, ���Ը�����
	char *pOutBuf = new char[ BUF_SIZE ];
	if( pOutBuf == NULL )
		return 0;

	int OutLen = 0;
	memset( pOutBuf, 0 , BUF_SIZE  );

	struct sockaddr addrfrom;
	int addrlen = sizeof( addrfrom );
	memset( &addrfrom,0, sizeof( addrfrom ) );

	DWORD start = GetTickCount();
	DWORD dwLastError = 0;

	int ret = RecvFrom( s, pOutBuf, BUF_SIZE,(sockaddr *)&addrfrom,&addrlen, timeout );
	dwLastError = WSAGetLastError();

	if( ret == 0 )
	{
		delete []pOutBuf;

		PrintfW( LEVEL_ERROR, _T("CSSCLient::SSRecvFrom RecvFrom error, connection closed by peer.") );

		return SOCKET_ERROR;
	}

	if( ret < 0 )
	{
		delete []pOutBuf;

		PrintfW( LEVEL_ERROR, _T("CSSCLient::SSRecvFrom RecvFrom error, lastcode: %d") ,dwLastError );

		WSASetLastError( dwLastError );

		return SOCKET_ERROR;
	}


	// ͳ������
	if( ret > 0 && IsRemoteSocket( s ))
	{
		pNodeInfo->PushData( ret );
		pNodeInfo->UpdateDownload_traffic( ret );
	}

	// ����CLIENT�İ�, ��ַ���û��ͨ��
	if( IsUdpClientSocket( s ) )
	{
		if( !_CheckUdpSrcBindAddr( (sockaddr *)&addrfrom ) )
		{
			delete []pOutBuf;

			return SOCKET_ERROR;
		}

		memcpy( &udpSrcAddr, &addrfrom, sizeof( addrfrom ) );
	}

	// ���Ļ�����̫С��
	if( ret > length )
	{
		delete []pOutBuf;

		return SOCKET_ERROR;
	}

	// �����Զ��SS��������������, �����
	if( IsRemoteSocket( s ) )
	{
		// UDPÿ����ͷ���������ܵ�IV��Ϣ,����ҪRESET��.
		pCryptor->ResetCryption();
		pCryptor->Decrypt( pOutBuf, ret, buffer, OutLen );

		// ��ss�������յ���UDPͷ
		/*
		* +------+----------+----------+----------+
		* | ATYP | DST.ADDR | DST.PORT |   DATA   |
		* +------+----------+----------+----------+
		* |  1   | Variable |    2     | Variable |
		* +------+----------+----------+----------+
		*/

		// ���ܺ���Ҫ��UDPͷǰ����3���ֽ��ٷ���SOCKS CLIENT
		if( !Add3BytesForUdpHeader( buffer,OutLen, length) )
		{
			delete []pOutBuf;

			return SOCKET_ERROR;
		}

		OutLen += 3;
	}
	// �����SS CLIENT����, �������. ֮����ʱ����ܵ�
	else
	{
		memcpy( buffer, pOutBuf, ret );
		OutLen = ret;
	}

	delete [] pOutBuf;

	return OutLen;
}
/** @brief  ����SSЭ��
* https://shadowsocks.org/en/spec/protocol.html
* +--------------+---------------------+------------------+----------+
* | Address Type | Destination Address | Destination Port |   Data   |
* +--------------+---------------------+------------------+----------+
* |      1       |       Variable      |         2        | Variable |
* +--------------+---------------------+------------------+----------+
*/
BOOL CSSCLient::SendSSProtocol()
{
	PrintfW( LEVEL_INFO, _T("CSSCLient::SendSSProtocol with socket: %d"),hRemoteSocket );

	if( hRemoteSocket == INVALID_SOCKET )
	{
		PrintfW( LEVEL_ERROR, _T("CSSCLient::SendSSProtocol: Remote shadowsocks server is not connected.") );
		return FALSE;
	}

	UINT SndLen = 0;
	char buf[600] = {0};
	buf[0] = ATyp;
	u_short _DstPort = htons( uDestinationPort );

	PrintfA( LEVEL_INFO, ("CSSCLient::SendSSProtocol Socket: %d, destination addr: %d,%s:%d"),hRemoteSocket ,ATyp,uDestinationAddr,uDestinationPort );

	if( ATyp == SOCKS_ATYPE_IPV4 )
	{
		SndLen = 7;
		u_long addr = inet_addr( uDestinationAddr );
		memcpy( buf + 1 , &addr, 4 );
		memcpy( buf + 5, &_DstPort, 2 );
	}
	else if( ATyp == SOCKS_ATYPE_DOMAIN )
	{
		int len = strlen ( uDestinationAddr );
		buf[1] = len;
		memcpy( buf + 2 , uDestinationAddr, len );
		memcpy( buf + 2 + len , &_DstPort, 2 );

		SndLen = 4 + len;
	}
	else{
		PrintfW( LEVEL_ERROR, _T("CSSCLient::SendSSProtocol: Address type error.") );
		return FALSE;
	}

	int ret = SSSend( hRemoteSocket, buf, SndLen,15 );
	if( ret <= 0 )
	{
		PrintfW( LEVEL_ERROR, _T("CSSCLient::SendSSProtocol: Send shadowsocks protocol error.") );
		return FALSE;
	}

	return TRUE;
}
/** @brief ִ����ProcessSocksTcpConnection֮��Ĳ���
*/
BOOL CSSCLient::AfterProcessSocksTcpConnection()
{
	PrintfW( LEVEL_INFO, _T("CSSCLient::AfterProcessSocksTcpConnection...") );

	if( !ConnectToSSServer( FALSE, TRUE ,NULL ))
	{
		PrintfW( LEVEL_INFO, _T("CSSCLient::AfterProcessSocksTcpConnection: connect to ss server error.") );
		return FALSE;
	}

	// send secceeded msg to client.
	int len;
	struct sockaddr_in clientBindSsin;
	len=sizeof(struct sockaddr);
	memset((char *)&clientBindSsin, 0, sizeof(struct sockaddr_in));
	getsockname( hRemoteSocket,(struct sockaddr *)&clientBindSsin,&len);

	if( Ver == SOCKS_V5 )
		_MakeSocksRspV5( SOCKS_ERR_SECCEEDED, clientBindSsin.sin_addr.s_addr, clientBindSsin.sin_port );
	else 
		_MakeSocksRspV4( SOCKS4_ERR_GRANTED, clientBindSsin.sin_addr.s_addr, clientBindSsin.sin_port );

	if( !SendSSProtocol() )
		return FALSE;

	return TRUE;
}
/** @brief ����socks udp ��������
*/
BOOL CSSCLient::ProcessSocksUdpConnection()
{
	PrintfW( LEVEL_INFO, _T("CSSCLient::ProcessSocksUdpConnection...") );

	SOCKET s = INVALID_SOCKET;
	s = CreateSocket( TRUE, FALSE );
	if( s == INVALID_SOCKET )
	{
		_MakeSocksRspV5( SOCKS_ERR_GENERAL, 0 , 0 );

		PrintfW( LEVEL_ERROR, _T("CSSCLient::ProcessSocksUdpConnection: Create UDP ASSOCIATE socket error.") );

		return FALSE;
	}
	PrintfW( LEVEL_INFO, _T("CSSCLient::ProcessSocksUdpConnection: UDP ASSOCIATE socket created. Socket(%d)."), s );

	struct sockaddr_in serverbind_ssin,clientBindSsin;
	memset((char *)&serverbind_ssin, 0, sizeof(struct sockaddr_in));
	memset((char *)&clientBindSsin, 0, sizeof(struct sockaddr_in));

	if( !BindSocket( s ))
	{
		_MakeSocksRspV5( SOCKS_ERR_GENERAL, 0 , 0 );

		CLOSESOCKET( s );

		return FALSE;
	}

	SOCKADDR_IN ServerAddr;
	if( !ConnectToSSServer( TRUE, FALSE ,&ServerAddr ) )
	{
		PrintfW( LEVEL_INFO, _T("CSSCLient::AfterProcessSocksTcpConnection: connect to ss server error.") );

		CLOSESOCKET( s );

		return FALSE;
	}

	// ��ʼ��UDPĿ���ַ, SSЭ����Ŀ���ַ����SS��������ַ
	memcpy(&udpDstAddr,&ServerAddr,sizeof( ServerAddr ));

	/*
    * Get information about ip and port after bind operation
    * to send to client
    */
    int len = sizeof (struct sockaddr_in);
    getsockname( s,(struct sockaddr *)&serverbind_ssin,&len);

	udpSocket = s;

	/*
     * SS5: create response to send to client
     */
	_MakeSocksRspV5( SOCKS_ERR_SECCEEDED, serverbind_ssin.sin_addr.s_addr , serverbind_ssin.sin_port );

	return AfterProcessSocksUdpConnection();
}

/** @brief ִ����ProcessSocksUdpConnection֮��Ĳ���
*/
BOOL CSSCLient::AfterProcessSocksUdpConnection()
{

	return TRUE;
}

BOOL CSSCLient::ConnectToPrivoxy()
{
	PrintfW( LEVEL_INFO, _T("CSSCLient::ConnectToPrivoxy...") );

	u_short privoxy_port = GetPrivoxyListenPort();

	if( !IsPrivoxyStarted( ) || privoxy_port == 0 )
	{
		return FALSE;
	}

	SOCKET s = INVALID_SOCKET;
	// create tcp no-blocking socket.
	s = CreateSocket( FALSE, FALSE );
	if( s == INVALID_SOCKET )
	{
		PrintfW( LEVEL_ERROR, _T("CSSCLient::ConnectToPrivoxy: Create remote socket error.") );
		return FALSE;
	}

	PrintfW( LEVEL_INFO, _T("CSSCLient::ConnectToPrivoxy: Remote socket created. Socket(%d)..."), s );
	
	if( !BindSocket( s ))
	{
		CLOSESOCKET( s );
		return FALSE;
	}

	SOCKADDR_IN ServerAddr;
	if( !TranslateSSServerAddress( ServerAddr, SSCAP_PRIVOXY_LOCAL_IP, privoxy_port ) )
	{
		PrintfW( LEVEL_ERROR, _T("CSSCLient::ConnectToPrivoxy: TranslateSSServerAddress error.") );
		CLOSESOCKET( s );
		return FALSE;
	}
	//connect SOCKS server
	int iRet =  0;
	iRet = connect( s, (const struct sockaddr*)&ServerAddr, sizeof(ServerAddr) );

	if (iRet == SOCKET_ERROR) 
	{
		PrintfW( LEVEL_ERROR, _T("CSSCLient::ConnectToPrivoxy: Connect to privoxy error.") );
		CLOSESOCKET( s );
		return FALSE;
	}

	// set no delay
	SetNodelay( s );

	hRemoteSocket = s;

	return TRUE;
}

/** @brief ������������
	* 
	* @returns
	*	0: ʧ��, �����߿���ֱ�ӷ�����.
	*	1: �ɹ�, ���ǵ����߿���ֱ�ӷ��ز���Ҫ���Ŵ���MainLoop��
	*	2: �ɹ�, ��������Ҫ����ȥ�����±ߵ�MainLoop
	*/
int CSSCLient::ProcessOtherRequest(int nUnrecognizedByte)
{
#ifdef USE_LIBPRIVOXY
	return 0;
#else 
	// TODO: PAC�ļ�����
	char szBuf[1024] = {0};
	szBuf[0] = nUnrecognizedByte;

	// ���Ƿ�SOCKS�����ݶ���Ϊ������, ���ټӼӽ��ܴ���

	bIsCipher = FALSE;

	int ret = _LocalRecv( szBuf + 1 , 1023 );
	if( ret > 0 )
	{
		char szCmd[100] = {0};
		sprintf_s(szCmd, 100 , "GET %s", DEFAULT_LOCAL_PAC_URL );

		// ����PAC�ļ�
		if( _strnicmp( szCmd, szBuf, strlen(szCmd)) == 0 )
		{
			if( RespPacFileBody() )
				return 1;
			else 
				return 0;
		}
		else
		{
			// ��������ȫ������Port Forward����
			if( ConnectToPrivoxy() )
			{
				// �Ѹղ��յ��İ��ͳ�ȥ.
				_RemoteSend( szBuf, ret + 1 );

				return 2;
			}
			else 
				return 0;
		}
	}


	return 0;
#endif
}
BOOL CSSCLient::UDPMainLoop()
{
	PrintfW( LEVEL_INFO, _T("CSSCLient::UDPMainLoop...") );
	fd_set fdRead;
	fd_set fdErro;
	int ret = 0;
	struct timeval tv;

	char *pRecvBuf = new char[BUF_SIZE];
	if( !pRecvBuf )
		return FALSE;

	do{
		//FD_ZERO(&fdWrite);
		FD_ZERO(&fdRead);
		FD_ZERO(&fdErro);

		//FD_SET( hSocksSocket, fdWrite );
		//FD_SET( hRemoteSocket, fdWrite );

		FD_SET( hSocksSocket, &fdRead ); // tcp socket
		FD_SET( hRemoteSocket, &fdRead ); // udp socket
		FD_SET( udpSocket, &fdRead ); // udp socket

		FD_SET( hSocksSocket, &fdErro );	// tcp
		FD_SET( hRemoteSocket, &fdErro );	// udp
		FD_SET( udpSocket, &fdErro );	//udp

		/*
		* Wait for receiving socks V5 request from client
		* until UDP_TIMEOUT value
		*/
		tv.tv_sec=UDP_TIMEOUT;
		tv.tv_usec=0;

		memset( pRecvBuf, 0 , BUF_SIZE );

		if( bUserBreaking )
			break;

		ret = select( 0 , &fdRead,NULL, &fdErro, &tv );
		if( ret <= 0 )
		{
			PrintfW( LEVEL_ERROR, _T("CSSCLient::UDPMainLoop: select error or timeout.") );
			break;
		}

		// ��socket���ִ���
		if( fdErro.fd_count > 0 )
		{
			PrintfW( LEVEL_ERROR, _T("CSSCLient::UDPMainLoop: (%d) sockets met error."),fdErro.fd_count );
			break;
		}

		if( fdRead.fd_count > 0 )
		{
			for( int i = 0 ; i < (int) fdRead.fd_count; i ++ )
			{
				SOCKET s = fdRead.fd_array[i];

				// �ȴ�SOCKS CLIENT�������ݵ�UDP SOCKET, ����.
				// ����һ��TCP��������,Э����ɺ���������һֱ����,���ǲ����շ�����.
				if( IsLocalSocket(  s ) )
				{
					char temp[100] = {0};

					ret = _Recv( s, temp, 100, 1 );
					if( ret <= 0 )
					{
						PrintfW( LEVEL_ERROR, _T("CSSCLient::UDPMainLoop: TCP Controller socket error.") );
						goto CLEAN;
					}
					continue;
				}

				BOOL bIsRemote = IsRemoteSocket( s );

				PrintfW( LEVEL_INFO, _T("CSSCLient::UDPMainLoop: socket(%d)[%s] ready to read."),s, bIsRemote ? _T("REMOTE"): _T("CLIENT") );

				// ����
				ret = SSRecvFrom( s,pRecvBuf, BUF_SIZE, 10 );
				if( ret > 0 )
				{
					SOCKET PeerSocket;

					// ����յ�����Զ��SS����������������. ��ô��ת��SOCKS CLIENT
					if( bIsRemote )
						PeerSocket = udpSocket;
					// ����ͽ�SOCKET CLIENT������ת��REMOTE SS
					else 
						PeerSocket  = hRemoteSocket;

					ret = SSSendTo( PeerSocket, pRecvBuf, ret, 15 );

					if( ret <= 0 )
					{
						PrintfW( LEVEL_WARNING, _T("CSSCLient::UDPMainLoop: socket(%d)[%s] Send data to peer error."),s, bIsRemote ? _T("REMOTE"): _T("CLIENT") );
						goto CLEAN;
					}
				}
				else
				{
					PrintfW( LEVEL_WARNING, _T("CSSCLient::UDPMainLoop: socket(%d)[%s] Recv error."),s, bIsRemote ? _T("REMOTE"): _T("CLIENT") );
					goto CLEAN;
				}
			}
		}
	}while( 1 );

CLEAN:
	delete []pRecvBuf;
	return TRUE;
}
BOOL CSSCLient::TCPMainLoop()
{
	PrintfW( LEVEL_INFO, _T("CSSCLient::TCPMainLoop...") );

	//fd_set fdWrite;
	fd_set fdRead;
	fd_set fdErro;
	//struct timeval tv;
	int ret = 0;
	char *pRecvBuf = new char[BUF_SIZE];
	if( !pRecvBuf )
		return FALSE;

	do{
		//FD_ZERO(&fdWrite);
		FD_ZERO(&fdRead);
		FD_ZERO(&fdErro);

		//FD_SET( hSocksSocket, fdWrite );
		//FD_SET( hRemoteSocket, fdWrite );

		FD_SET( hSocksSocket, &fdRead );
		FD_SET( hRemoteSocket, &fdRead );

		FD_SET( hSocksSocket, &fdErro );
		FD_SET( hRemoteSocket, &fdErro );

		memset( pRecvBuf, 0 , BUF_SIZE );

		if( bUserBreaking )
			break;

		ret = select( 0 , &fdRead,NULL, &fdErro, NULL );
		if( ret <= 0 )
		{
			PrintfW( LEVEL_ERROR, _T("CSSCLient::TCPMainLoop: select error(%d)"),WSAGetLastError() );
			break;
		}

		// ��socket���ִ���
		if( fdErro.fd_count > 0 )
		{
			PrintfW( LEVEL_ERROR, _T("CSSCLient::TCPMainLoop: (%d) sockets met error."),fdErro.fd_count );
			break;
		}

		if( fdRead.fd_count > 0 )
		{
			for( int i = 0 ; i < (int) fdRead.fd_count; i ++ )
			{
				SOCKET s = fdRead.fd_array[i];
				BOOL bIsRemote = IsRemoteSocket( s );

				PrintfW( LEVEL_INFO, _T("CSSCLient::TCPMainLoop: Socket(%d)[%s] is waiting data."),s, bIsRemote ? _T("REMOTE"): _T("CLIENT") );

				// ����
				ret = SSRecv( s,pRecvBuf, BUF_SIZE, 10 );
				//ret = SSRecv( s,pRecvBuf, 100, 10 );
				if( ret > 0 )
				{
					SOCKET PeerSocket;

					// ����յ�����Զ��SS����������������. ��ô��ת��SOCKS CLIENT
					if( bIsRemote )
						PeerSocket = hSocksSocket;
					// ����ͽ�SOCKET CLIENT������ת��REMOTE SS
					else 
						PeerSocket  = hRemoteSocket;

					PrintfW( LEVEL_INFO, _T("CSSCLient::TCPMainLoop: socket(%d)[%s] Sending data to Peer(%d) [%s]."),s, bIsRemote ? _T("REMOTE"): _T("CLIENT") ,
						PeerSocket,bIsRemote ? _T("CLIENT"): _T("REMOTE") );

					ret = SSSend( PeerSocket, pRecvBuf, ret, 15 );

					if( ret <= 0 )
						PrintfW( LEVEL_WARNING, _T("CSSCLient::TCPMainLoop: socket(%d)[%s] Send data to peer error."),s, bIsRemote ? _T("REMOTE"): _T("CLIENT") );
				}
				else
				{
					PrintfW( LEVEL_WARNING, _T("CSSCLient::TCPMainLoop: socket(%d)[%s] Recv error."),s, bIsRemote ? _T("REMOTE"): _T("CLIENT") );
					goto CLEAN;
				}
			}
		}
	}while( 1 );

CLEAN:
	delete []pRecvBuf;
	return TRUE;
}

/** @brief ִ�����ݴ��ݵ�loop
*/
BOOL CSSCLient::MainLoop()
{
	if( Cmd == SOCKS_CMD_UDP_ASSOCIATE )
	{
		return UDPMainLoop();
	}

	return TCPMainLoop();
}

/** @brief ��SS��������������UDP��ǰ����3���ֽ�
* �յ�����:
* +------+----------+----------+----------+
* | ATYP | DST.ADDR | DST.PORT |   DATA   |
* +------+----------+----------+----------+
* |  1   | Variable |    2     | Variable |
* +------+----------+----------+----------+
* ����Ϊ:
* +----+------+------+----------+----------+----------+
* |RSV | FRAG | ATYP | DST.ADDR | DST.PORT |   DATA   |
* +----+------+------+----------+----------+----------+
* | 2  |  1   |  1   | Variable |    2     | Variable |
* +----+------+------+----------+----------+----------+
*
* @param buffer: ��Ҫ�޸ĵ�BUFFER
* @param DataLen: buffer�е����ݳ���
* @param BufLen: Buffer�ܳ���
*/
BOOL CSSCLient::Add3BytesForUdpHeader( char *buffer, int DataLen, int BufLen )
{
	if( ( DataLen + 3 ) > BufLen 
		|| buffer == NULL
		)
		return FALSE; // buffer���Ȳ���

	char *temp = new char[ DataLen + 3 ];
	if( !temp ) return FALSE;

	temp[0] = 0 ;
	temp[1] = 0 ;
	temp[2] = 0 ;

	memcpy( temp + 3, buffer, DataLen );
	memcpy( buffer, temp, DataLen + 3 );

	delete [] temp;

	return TRUE;
}

/** @brief ��Socks Client��������UDP��ǰ����ͷ3���ֽ�
* �յ�����:
* +----+------+------+----------+----------+----------+
* |RSV | FRAG | ATYP | DST.ADDR | DST.PORT |   DATA   |
* +----+------+------+----------+----------+----------+
* | 2  |  1   |  1   | Variable |    2     | Variable |
* +----+------+------+----------+----------+----------+
* ����Ϊ:
* +------+----------+----------+----------+
* | ATYP | DST.ADDR | DST.PORT |   DATA   |
* +------+----------+----------+----------+
* |  1   | Variable |    2     | Variable |
* +------+----------+----------+----------+
*
* @param buffer: ��Ҫ�޸ĵ�BUFFER
* @param DataLen: buffer�е����ݳ���
* @param BufLen: Buffer�ܳ���
*/
BOOL CSSCLient::Remove3BytesForUdpHeader( char *bufer, int DataLen )
{
	if( bufer == NULL ||
		DataLen == 0 
		)
		return FALSE;

	memcpy( bufer, bufer + 3 , DataLen - 3 );

	return TRUE;
}
/** @brief ����PAC�ļ�����
*/
BOOL CSSCLient::RespPacFileBody()
{
	/*
	CSSConfigInfo *pCfg = GetConfigInfo();

	int length = 0;
	char *filebody = NULL;
	
	if( strPacBodyCache.empty() )
	{
		filebody = readallfile( (TCHAR *)pCfg->localPacFileFullName.c_str(),_T("rb") , &length );
		if( !filebody ) return FALSE;	

		strPacBodyCache = string( filebody );

		char szLocalProxy[100] = {0};
		sprintf_s( szLocalProxy, 50, "PROXY 127.0.0.1:%d;DIRECT;",nProxyListenPort );

		string proxy = string( szLocalProxy );
		Replace( strPacBodyCache,string("__PROXY__"),proxy  );
		free( filebody );
	}
	*/
#define SSCAP_PAC_HEADER "HTTP/1.1 200 OK\r\n"\
"Server: SSCap\r\n"\
"Content-Type: application/x-ns-proxy-autoconfig\r\n"\
"Content-Length: %d\r\n"\
"Connection: Close\r\n\r\n"

	char szHeader[2000] = {0};
	string body = GetPacFileContent();
	if( body.empty() )
		return FALSE;

	sprintf_s( szHeader, 2000, SSCAP_PAC_HEADER, body.length() );

	int ret = _LocalSend( (char *)szHeader, strlen( szHeader ) );
	if( ret <= 0 )
		return FALSE;

	ret = _LocalSend( (char *)body.c_str(), body.length() );
	if( ret <= 0 )
		return FALSE;

	return TRUE;
}
/** @brief �û�ϣ��������ֹ��ǰ����ת������
*/
void CSSCLient::SetUserBreaking()
{
	bUserBreaking = TRUE;

	CloseAllSocket();
}