#include "stdheader.h"
#include "Debug.h"
#include "mymutex.h"
#include "SocketWrapper.h"
#include "SocketBase.h"
#include "SocksClient.h"
#include <assert.h>
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace debug;

CSocksClient::CSocksClient( SOCKET s ,const char *username /* proxy user */, const char *password /* proxy pass */ )
{
	hSocksSocket = s;
	hRemoteSocket = INVALID_SOCKET;
	//bNeedAuthByUserPass = FALSE;

	memset( uDestinationAddr , 0 , SOCKS_MAX_DOMAIN_LEN + 1 );
	uDestinationPort = 0;

	//memset( szDestinationDomainName,0, SOCKS_MAX_DOMAIN_LEN + 1 );

	//memset(SrcAddr, 0 , 16 );
	//SrcPort = 0;
	memset( &SocksSrcAddr, 0 , sizeof( SocksSrcAddr ) );
	
	udpSocket = INVALID_SOCKET;
	//memset( udpSrcAddr , 0 , 16 );
	//udpSrcPort = 0;

	Ver = SOCKS_V5;
	Cmd = 0;
	Rsv = 0;
	ATyp = 0;

	memset( &udpSrcBindAddr, 0 , sizeof( udpSrcBindAddr ) );

	memset( &udpSrcAddr, 0 , sizeof( udpSrcAddr ) );

	memset( szSocksV4UserId , 0 , 256 );
	//memset( udpSrcBindAddr, 0 , 16 );
	//udpSrcBindPort = 0;
	nUnrecognizedByte = 0;
	bFoundUnrecognizedByte = FALSE;
	bCouldProcessOtherRequest = TRUE;
	nProxyListenPort = 0;

	memset( szProxyUser , 0 , 256 );
	memset( szProxyPass , 0 , 256 );

	if( username != NULL )
	{
		strncpy_s(szProxyUser,256,username, 255 );
	}

	if( password != NULL )
	{
		strncpy_s(szProxyPass,256,password, 255 );
	}
}

CSocksClient::~CSocksClient()
{
	CloseAllSocket();
}

void CSocksClient::CloseAllSocket()
{
	PrintfW( LEVEL_INFO, _T("CSocksClient::CloseAllSocket, local Socket: %d..."), hSocksSocket );
	CLOSESOCKET( hSocksSocket );
	PrintfW( LEVEL_INFO, _T("CSocksClient::CloseAllSocket, remote Socket: %d..."), hRemoteSocket );
	CLOSESOCKET( hRemoteSocket );
	PrintfW( LEVEL_INFO, _T("CSocksClient::CloseAllSocket, Udp Socket: %d..."), udpSocket );
	CLOSESOCKET( udpSocket );
}

/** @brief ����socks 5����
*/
BOOL CSocksClient::Start()
{
	PrintfW( LEVEL_INFO, _T("CSocksClient::Started with Socket(%d)..."), hSocksSocket );

	_GetClientInfo();

	SetNodelay( hSocksSocket );

	UINT version = 0;
	BOOL bValidVer = TRUE;
	bValidVer = _IsReceivedValidSocksVersion( version );

	if( !bValidVer )
	{
		bFoundUnrecognizedByte = TRUE;
		nUnrecognizedByte = version;

		// ������������, ����: PAC�ļ�������
		if( bCouldProcessOtherRequest )
		{
			int process_ret = 0;
			process_ret = ProcessOtherRequest( nUnrecognizedByte ) ;
			// 0: ʧ��
			if( process_ret == 0 )
				return FALSE;
			// 1: �ɹ�, ������ֱ�ӷ���
			else if( process_ret == 1 )
				return TRUE;
			// 2: �ɹ�, ���Ŵ���
		}
		else 
		{
			PrintfW( LEVEL_ERROR, _T("CSocksClient::Start: Received Invalid Socks Version.") );
			return FALSE;
		}
	}

	if( bValidVer && Ver == SOCKS_V5 )
	{
		if( !ProcessSocksAuthMethod() )
		{
			PrintfW( LEVEL_ERROR, _T("CSocksClient::Start: ProcessSocksAuthMethod failed.") );
			return FALSE;
		}

		if( !ProcessSocksAuth() )
		{
			PrintfW( LEVEL_ERROR, _T("CSocksClient::Start: ProcessSocksAuth failed.") );
			return FALSE;
		}

		if( !ProcessSocksConnection() )
		{
			PrintfW( LEVEL_ERROR, _T("CSocksClient::Start: ProcessSocksConnection failed.") );

			return FALSE;
		}
	}
	else if( bValidVer && Ver == SOCKS_V4 )
	{
		if( !ProcessSocksV4() )
			return FALSE;
	}

	return MainLoop();
}

int CSocksClient::_LocalRecv( char *buffer, int length )
{
	return __super::_Recv( hSocksSocket, buffer, length, DEFAULT_TIMEOUT );
}
int CSocksClient::_RemoteRecv( char *buffer, int length )
{
	return __super::_Recv( hRemoteSocket, buffer, length, DEFAULT_TIMEOUT );
}

int CSocksClient::_LocalSend( char *buffer, int length )
{
	return __super::_Send( hSocksSocket, buffer, length, DEFAULT_TIMEOUT );
}
int CSocksClient::_RemoteSend( char *buffer, int length )
{
	return __super::_Send( hRemoteSocket, buffer, length, DEFAULT_TIMEOUT );
}
/** @brief ��socket�н���һ���ֽ�
*/
BOOL CSocksClient::_GetOneByteFromLocalSocket( int &n )
{
	return __super::_GetOneByteFromSocket( hSocksSocket , n , DEFAULT_TIMEOUT );
}
/** @brief ��local socket( ���ӵ�Ŀ�ĵص�socket ) �н���һ���ֽ�
*/
BOOL CSocksClient::_GetOneByteFromRemoteSocket( int &n )
{
	return __super::_GetOneByteFromSocket( hRemoteSocket , n ,DEFAULT_TIMEOUT * 2 );
}
/** �Ƿ��յ�һ����Ч��SOCKS�汾��
*/
BOOL CSocksClient::_IsReceivedValidSocksVersion( UINT &r_version  )
{
	int version = 0;
	PrintfW( LEVEL_INFO, _T("CSocksClient::_IsReceivedValidSocksVersion...") );

	if( !_GetOneByteFromLocalSocket( version ) )
	{
		return FALSE;
	}

	r_version = version;

	if( version != SOCKS_V5 
		&& version != SOCKS_V4
		)
	{
		PrintfW( LEVEL_WARNING, _T("CSocksClient::_IsReceivedValidSocksVersion: Get Invalid Socks version(%d).") , version );
		return FALSE;
	}

	Ver = version;

	return TRUE;
}
/** @brief ����Socks v4����
+----+----+----+----+----+----+----+----+----+----+....+----+
| VN | CD | DSTPORT |      DSTIP        | USERID       |NULL|
+----+----+----+----+----+----+----+----+----+----+....+----+
#  1    1      2              4           variable       1
*/
BOOL CSocksClient::ProcessSocksV4()
{
	PrintfW( LEVEL_INFO, _T("CSocksClient::ProcessSocksV4..." ) );

	int command = 0;

	// command
	if( !_GetOneByteFromLocalSocket( command ) )
	{
		_MakeSocksRspV4( SOCKS4_ERR_REJECTED, 0, 0 );
		return FALSE;
	}
	if( command != SOCKS4_CMD_CONNECT
		&& command != SOCKS4_CMD_BIND
		)
	{
		PrintfW( LEVEL_ERROR, _T("CSocksClient::ProcessSocksV4: command error.") );
		return FALSE;
	}

	Cmd = command;
	ATyp = SOCKS_ATYPE_IPV4;

	char addr_port[50] = {0};
	int ret = _LocalRecv( addr_port, 6 );
	if( ret != 6 )
	{
		PrintfW( LEVEL_ERROR, _T("CSocksClient::ProcessSocksV4: Get IPv4 addr and port error.") );
		return FALSE;
	}

	u_long addr  = 0 ;
	u_short port = 0;
	char domain[256] = {0};
	char userid[256] = {0};
	/*
	+----+----+----+----+----+----+----+----+----+----+....+----+----+----+....+----+
	| VN | CD | DSTPORT |      DSTIP        | USERID       |NULL| HOST       |NULL|
	+----+----+----+----+----+----+----+----+----+----+....+----+----+----+....+----+
	# 1    1      2              4           variable       1      variable       1
	A server using protocol 4A must check the DSTIP in the request packet.
	If it represent address 0.0.0.x with nonzero x, the server must read
	in the domain name that the client sends in the packet. The server
	should resolve the domain name and make connection to the destination
	host if it can. 
	*/
	BOOL bNeedResolveDomain = FALSE;

	memcpy( &port, addr_port, 2 );

	if( addr_port[4] == 0 
		&& addr_port[5] == 0 
		&& addr_port[6] == 0 
		&& addr_port[7] != 0 )
	{
		// �ͻ��� ʹ����SOCKS 4AЭ��, ��Ҫ��������
		bNeedResolveDomain = TRUE;
		ATyp = SOCKS_ATYPE_DOMAIN;
	}

	memcpy( &addr, addr_port + 2 , 4 );

	if( ( addr == 0 || port == 0 ) )
	{
		PrintfW( LEVEL_ERROR, _T("CSocksClient::ProcessSocksV4: Get IPv4 addr =0 or port = 0.") );
		return FALSE;
	}

	// ��ȡuserid
	BOOL bGotEndFlag = FALSE;
	for( int i = 0 ; i < 255 ; i ++ )
	{
		int userid_flag = 0;
		if( !_GetOneByteFromLocalSocket( userid_flag ) )
		{
			_MakeSocksRspV4( SOCKS4_ERR_REJECTED, 0, 0 );
			return FALSE;
		}

		userid[ i ] = userid_flag;

		if( userid_flag == 0 )
		{
			bGotEndFlag = TRUE;
			break;
		}
	}

	// û�ж���userid�����ַ�
	if( !bGotEndFlag )
	{
		_MakeSocksRspV4( SOCKS4_ERR_REJECTED, 0, 0 );
		return FALSE;
	}

	// ��Ҫ��ȡ����
	if( bNeedResolveDomain )
	{
		bGotEndFlag = FALSE;
		for( int i = 0 ; i < 255 ; i ++ )
		{
			int domain_flag = 0;
			if( !_GetOneByteFromLocalSocket( domain_flag ) )
			{
				_MakeSocksRspV4( SOCKS4_ERR_REJECTED, 0, 0 );
				return FALSE;
			}

			domain[ i ] = domain_flag;

			if( domain_flag == 0 )
			{
				bGotEndFlag = TRUE;
				break;
			}
		}

		// û�ж���domain�����ַ�
		if( !bGotEndFlag )
		{
			_MakeSocksRspV4( SOCKS4_ERR_REJECTED, 0, 0 );
			return FALSE;
		}
	}

#ifdef _DEBUG
	in_addr inaddr;
	inaddr.s_addr = addr ;

	PrintfA( LEVEL_INFO, ("CSocksClient::ProcessSocksV4: Destination IPv4 addr: %s@%s:%d"),userid,bNeedResolveDomain? domain : inet_ntoa( inaddr), ntohs( port ) );
#endif

	// CONNECT , BIND
	// Ŀ���ַ,�˿�
	if( command == SOCKS4_CMD_CONNECT || command == SOCKS4_CMD_BIND )
	{
		uDestinationPort = ntohs( port );

		if( ATyp == SOCKS_ATYPE_IPV4 )
		{
			in_addr inaddr;
			inaddr.s_addr = addr ;

			strncpy_s( uDestinationAddr, SOCKS_MAX_DOMAIN_LEN + 1, inet_ntoa( inaddr ),  SOCKS_MAX_DOMAIN_LEN + 1  );
		}
		else if( ATyp == SOCKS_ATYPE_DOMAIN )
		{
			strncpy_s( uDestinationAddr, SOCKS_MAX_DOMAIN_LEN + 1, domain,  SOCKS_MAX_DOMAIN_LEN + 1  );
		}
	}

	if( command == SOCKS_CMD_CONNECT )
	{
		return ProcessSocksTcpConnection();
	}
	else if( command == SOCKS_CMD_BIND )
	{
		return ProcessSocksV4Bind();
	}

	return FALSE;
}

/** @brief Socks5 auth medthod 
*/
BOOL CSocksClient::ProcessSocksAuthMethod()
{
	PrintfW( LEVEL_INFO, _T("CSocksClient::ProcessSocksAuthMethod..." ) );

	if( Ver != SOCKS_V5 ) 
		return TRUE;

	// ������֤����
	int nMethods = 0;
	if( !_GetOneByteFromLocalSocket( nMethods ) )
	{
		PrintfW( LEVEL_ERROR, _T("CSocksClient::ProcessSocksAuthMethod: Get Methods ammount error." ) );
		return FALSE;
	}

	BOOL bClientSupportAuth = FALSE;
	bool bClientSupportAnonyMous = FALSE;

	// Ĭ�������뷽ʽ��֤.
	int MethodResp = SOCKS_AUTH_ANONYMOUS;
	unsigned char buffer[ 50 ] = {0};
	buffer[0] = SOCKS_V5;
	for ( int i = nMethods; i; i--)
	{
		int nMethod = 0 ;
		if( !_GetOneByteFromLocalSocket( nMethod ) )
		{
			PrintfW( LEVEL_ERROR, _T("CSocksClient::ProcessSocksAuthMethod: Get Method error." ) );
			return FALSE;
		}

		if( !_IsValidProxyMethod( nMethod ) )
		{
			PrintfW( LEVEL_ERROR, _T("CSocksClient::ProcessSocksAuthMethod: Get unspported auth method." ) );

			buffer[1] = SOCKS_AUTH_NO_METHODS ;

			_LocalSend( (char *)buffer , 2 );

			return FALSE;
		}

		if( nMethod == SOCKS_AUTH_USERPASS )
		{
			bClientSupportAuth = TRUE;
			//havepass = SOCKS_AUTH_USERPASS;
			//bNeedAuthByUserPass = TRUE;
		}

		else if( nMethod == SOCKS_AUTH_ANONYMOUS )
		{
			bClientSupportAnonyMous = TRUE;
		}
	}

	// ������Ҫ����֤. ���ǿͻ��˲�֧����֤.
	// ֱ�ӷ���SOCKS_AUTH_NO_METHODS
	if( ( ( szProxyUser[0] !=0 || szProxyPass[0] != 0 )
		&& !bClientSupportAuth
		)
		|| 

		// ������Ҫ������, ���ǿͻ��˲�֧������
		// ֱ�ӷ���SOCKS_AUTH_NO_METHODS
		( ( szProxyUser[0] ==0 && szProxyPass[0] == 0 )
		 && !bClientSupportAnonyMous
		 )
		 )
	{
		PrintfW( LEVEL_ERROR, _T("CSocksClient::ProcessSocksAuthMethod: Server need auth but client do not support auth." ) );

		buffer[1] = SOCKS_AUTH_NO_METHODS ;

		_LocalSend( (char *)buffer , 2 );

		return FALSE;
	}

	// �Ƿ���Ҫ��֤��Ӧ���ɿͻ��˾���,��Ӧ�ɷ���������.����û�ָ��������, ���������Ҫ��֤����.
	if( szProxyUser[0] !=0 || szProxyPass[0] != 0 )
		MethodResp = SOCKS_AUTH_USERPASS;
	
	buffer[1] = MethodResp;
	int ret = _LocalSend( (char *)buffer , 2 );

	if( ret != 2 )
	{
		PrintfW( LEVEL_ERROR, _T("CSocksClient::ProcessSocksAuthMethod: Resp Auth method error.") );
		return FALSE;
	}

	return TRUE;
}
/** @biref socks5 auth 
*/
BOOL CSocksClient::ProcessSocksAuth()
{
	PrintfW( LEVEL_INFO, _T("CSocksClient::ProcessSocksAuth...") );

	if( szProxyUser[0] == 0 && szProxyPass[0] == 0 )
		return TRUE;

	// disabled at 15.12.18 �Ƿ���Ҫ��֤��Ӧ���ɿͻ��˾���,��Ӧ�ɷ���������.����û�ָ��������, ���������Ҫ��֤����.
	/*
	if( !bNeedAuthByUserPass )
	{
		return TRUE;
	}
	*/

	// ��֤�汾��
	int auth_version  = 0;

	if( !_GetOneByteFromLocalSocket( auth_version ) )
	{
		return FALSE;
	}

	if( auth_version != SOCKS_AUTH_VERSION )
	{
		PrintfW( LEVEL_ERROR, _T("CSocksClient::ProcessSocksAuth: subnegotiation version error(%d)."), auth_version );
		return FALSE;
	}

	int user_length = 0;
	if( !_GetOneByteFromLocalSocket( user_length ) )
	{
		return FALSE;
	}

	if( user_length == 0 || user_length > SOCKS_MAX_USER_LEN )
	{
		PrintfW( LEVEL_ERROR, _T("CSocksClient::ProcessSocksAuth: Length of username wrong.") );
		return FALSE;
	}

	char username[256] = {0};
	char password[256] = {0};

	int ret = 0;
	ret = _LocalRecv( username, user_length );
	if( ret != user_length )
	{
		PrintfW( LEVEL_ERROR, _T("CSocksClient::ProcessSocksAuth: Recv username error.") );
		return FALSE;
	}

	int pass_length = 0;
	if( !_GetOneByteFromLocalSocket( pass_length ) )
	{
		return FALSE;
	}

	if( pass_length == 0 || pass_length > SOCKS_MAX_PASS_LEN )
	{
		PrintfW( LEVEL_ERROR, _T("CSocksClient::ProcessSocksAuth: Length of username wrong.") );
		return FALSE;
	}

	ret = _LocalRecv( password, pass_length );
	if( ret != pass_length )
	{
		PrintfW( LEVEL_ERROR, _T("CSocksClient::ProcessSocksAuth: Recv password error.") );
		return FALSE;
	}

	/** ��֤�ʺ�����*/
	BOOL bVerify = VerifySocksUsernamePassword( username, password );

/*
+----+--------+
|VER | STATUS |
+----+--------+
| 1  |   1    |
+----+--------+
 A STATUS field of X'00' indicates success. If the server returns a
 `failure' (STATUS value other than X'00') status, it MUST close the
 connection.
*/
	char buffer[10] = {0};
	buffer[0] = SOCKS_AUTH_VERSION;
	buffer[1] = !bVerify;

	ret = _LocalSend( buffer, 2 );
	if( ret != 2 )
	{
		PrintfW( LEVEL_ERROR, _T("CSocksClient::ProcessSocksAuth: Send Auth result failed.") );
		return FALSE;
	}

	return bVerify;
}
/** @brief ����socks��������
*/
BOOL CSocksClient::ProcessSocksConnection()
{
	PrintfW( LEVEL_INFO, _T("CSocksClient::ProcessSocksConnection...") );

	UINT version = 0;
	if( !_IsReceivedValidSocksVersion( version ) )
	{
		PrintfW( LEVEL_ERROR, _T("CSocksClient::ProcessSocksConnection: Received Invalid Socks Version.") );
		return FALSE;
	}

	int command = 0;
	int reserved = 0;
	int atype = 0 ;
	int ret = 0;

	// command
	if( !_GetOneByteFromLocalSocket( command ) )
	{
		_MakeSocksRspV5( SOCKS_ERR_COMMAND_NOT_SUPPORTED, 0, 0 );
		return FALSE;
	}

	// reserved
	if( !_GetOneByteFromLocalSocket( reserved ) )
	{
		_MakeSocksRspV5( SOCKS_ERR_GENERAL, 0, 0 );

		return FALSE;
	}

	/*
	In an address field (DST.ADDR, BND.ADDR), the ATYP field specifies
	the type of address contained within the field:

	o  X'01'

	the address is a version-4 IP address, with a length of 4 octets

	o  X'03'

	the address field contains a fully-qualified domain name.  The first
	octet of the address field contains the number of octets of name that
	follow, there is no terminating NUL octet.

	o  X'04'

	the address is a version-6 IP address, with a length of 16 octets.
	*/
	// addressType
	if( !_GetOneByteFromLocalSocket( atype ) )
	{
		_MakeSocksRspV5( SOCKS_ERR_ADDRESS_NO_SUPPRTED, 0, 0 );

		return FALSE;
	}

	if( !_IsReceivedValidCommand( command ) )
	{
		PrintfW( LEVEL_ERROR, _T("CSocksClient::ProcessSocksConnection: Received Invalid command(%d)."),command );
		return FALSE;
	}

	/** 
	�����CONNECT, ����ȡ���ĵ�ַ��Ҫ���ʵ�Ŀ���ַ:�˿�.
	�����BIND, ����ȡ���ĵ�ַ��Ҫ���ʵ�Ŀ���ַ:�˿�.
	�����UDP, ����ȡ������������Դ���󶨵ĵ�ַ:�˿�. �Ժ󽫽���������˵�ַ�Ͷ˿�ͨ��
	*/
	//struct sockaddr_in addrin;
	char bnddomain[SOCKS_MAX_DOMAIN_LEN + 1] = {0};
	char ipv6addr[16] = {0};
	u_long _TmpDstAddr = 0;
	u_short _TmpDstPort = 0;

	switch( atype )
	{
	case SOCKS_ATYPE_IPV4:
		{
			char addr_port[50] = {0};
			ret = _LocalRecv( addr_port, 6 );
			if( ret != 6 )
			{
				PrintfW( LEVEL_ERROR, _T("CSocksClient::ProcessSocksTcpConnection: Get IPv4 addr and port error.") );
				return FALSE;
			}

			u_long addr  = 0 ;
			u_short port = 0;

			memcpy(&addr, addr_port, 4 );
			memcpy( &port, addr_port + 4 , 2 );

			//UDP ASSOCIATE��ʱ��,addr, port������0, ����ʱ��Ӧ��Ϊ0
			if( command == SOCKS_CMD_CONNECT
				&& ( addr == 0 || port == 0 ) )
			{
				PrintfW( LEVEL_ERROR, _T("CSocksClient::ProcessSocksTcpConnection: Get IPv4 addr =0 or port = 0.") );
				return FALSE;
			}

			//addrin.sin_addr.s_addr = addr;
			//addrin.sin_port = port;

			_TmpDstAddr = addr;
			_TmpDstPort = port;

#ifdef _DEBUG
			in_addr inaddr;
			inaddr.s_addr = _TmpDstAddr ;

			PrintfA( LEVEL_INFO, ("CSocksClient::ProcessSocksTcpConnection: Destination IPv4 addr: %s:%d"),inet_ntoa( inaddr), ntohs( _TmpDstPort ) );
#endif
		}
		break;
	case SOCKS_ATYPE_DOMAIN:
		{
			int domain_len = 0;
			if( !_GetOneByteFromLocalSocket( domain_len ) )
			{
				return FALSE;
			}

			if( domain_len == 0 || domain_len > SOCKS_MAX_DOMAIN_LEN )
			{
				PrintfW( LEVEL_ERROR, _T("CSocksClient::ProcessSocksTcpConnection: [DOMAIN] Length of domain name wrong.") );
				return FALSE;
			}

			char domain_name[256] = {0};
			//recv domain
			ret = _LocalRecv( domain_name, domain_len );
			if( ret != domain_len )
			{
				PrintfW( LEVEL_ERROR, _T("CSocksClient::ProcessSocksTcpConnection: [DOMAIN] Get domain name error.") );
				return FALSE;
			}

			memcpy( bnddomain, domain_name, domain_len );

			// recv port
			ret = _LocalRecv( domain_name, 2 );
			if( ret != 2 )
			{
				PrintfW( LEVEL_ERROR, _T("CSocksClient::ProcessSocksTcpConnection: [DOMAIN] Get port error.") );
				return FALSE;
			}
			
			memcpy( &_TmpDstPort, domain_name, 2 );

			PrintfA( LEVEL_INFO, ("CSocksClient::ProcessSocksTcpConnection: Get Domain addr: %s:%d"),bnddomain ,htons( _TmpDstPort ));
		}
		break;
	case SOCKS_ATYPE_IPV6:
	default:
		{
			_MakeSocksRspV5( SOCKS_ERR_ADDRESS_NO_SUPPRTED, 0, 0 );

			PrintfW( LEVEL_ERROR, _T("CSocksClient::ProcessSocksTcpConnection: Unsupport addr method.") );
			return FALSE;
		}
		break;
	}

	Cmd = command;
	Rsv = reserved;
	ATyp = atype;

	// CONNECT , BIND
	// Ŀ���ַ,�˿�
	if( command == SOCKS_CMD_CONNECT || command == SOCKS_CMD_BIND )
	{
		uDestinationPort = ntohs( _TmpDstPort );

		if( ATyp == SOCKS_ATYPE_IPV4 )
		{
			in_addr inaddr;
			inaddr.s_addr = _TmpDstAddr ;

			strncpy_s( uDestinationAddr, SOCKS_MAX_DOMAIN_LEN + 1, inet_ntoa( inaddr ),  SOCKS_MAX_DOMAIN_LEN + 1  );
		}
		else if( ATyp == SOCKS_ATYPE_DOMAIN )
		{
			strncpy_s( uDestinationAddr, SOCKS_MAX_DOMAIN_LEN + 1, bnddomain,  SOCKS_MAX_DOMAIN_LEN + 1  );
		}
	}
	// UDP ASSOCIATE
	// Դ�󶨵�ַ�Ͷ˿�
	else if( command == SOCKS_CMD_UDP_ASSOCIATE )
	{
		//udpSrcBindPort = ntohs( _TmpDstPort );
		((sockaddr_in *)&udpSrcBindAddr)->sin_port = _TmpDstPort;
		if( ATyp == SOCKS_ATYPE_IPV4 )
		{
			//in_addr inaddr;
			//inaddr.s_addr = _TmpDstAddr ;

			//strncpy_s( udpSrcBindAddr, 16, inet_ntoa( inaddr ),  16  );
			((sockaddr_in *)&udpSrcBindAddr)->sin_addr.s_addr = _TmpDstAddr;
		}
		else if( ATyp == SOCKS_ATYPE_DOMAIN )
		{
			//memset( udpSrcBindAddr, 0, 16 );
		}
	}

	if( command == SOCKS_CMD_CONNECT )
	{
		return ProcessSocksTcpConnection();
	}
	else if( command == SOCKS_CMD_BIND )
	{
		return ProcessSocksV5Bind();
	}
	else if( command == SOCKS_CMD_UDP_ASSOCIATE )
	{
		return ProcessSocksUdpConnection();
	}
	
	PrintfW( LEVEL_ERROR, _T("CSocksClient::ProcessSocksConnection: Command Unsupport(%d)"), command );
	return FALSE;
}

BOOL CSocksClient::DirectConnectTo()
{
	PrintfA( LEVEL_INFO, ("CSocksClient::DirectConnectTo: %s:%d..."), uDestinationAddr, uDestinationPort );

	SOCKET s = INVALID_SOCKET;
	
	// create tcp no-blocking socket.
	s = CreateSocket( FALSE, FALSE );
	if( s == INVALID_SOCKET )
	{
		PrintfW( LEVEL_ERROR, _T("CSocksClient::DirectConnectTo: Create remote socket error.") );
		return FALSE;
	}

	PrintfW( LEVEL_INFO, _T("CSocksClient::DirectConnectTo: Remote socket created. Socket(%d)..."), s );

	if( !BindSocket( s ))
	{
		CLOSESOCKET( s );
		return FALSE;
	}

	SOCKADDR_IN ServerAddr;
	ServerAddr.sin_family = AF_INET;
	ServerAddr.sin_port = htons((u_short)uDestinationPort);
	ServerAddr.sin_addr.s_addr = inet_addr(uDestinationAddr);

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

	return TRUE;
}

/** @brief ����socks5 tcp ��������
*/
BOOL CSocksClient::ProcessSocksTcpConnection()
{
	PrintfW( LEVEL_INFO, _T("CSocksClient::ProcessSocksTcpConnection...") );

#if 0
	if( uDestinationAddr[0] == '1'
		&& uDestinationAddr[1] == '2'
		&& uDestinationAddr[2] == '7'
		&& uDestinationAddr[3] == '.'
		&& uDestinationAddr[4] == '0'
		&& uDestinationAddr[5] == '.'
		&& uDestinationAddr[6] == '0'
		&& uDestinationAddr[7] == '.'
		&& uDestinationAddr[8] == '1'
		&& uDestinationPort == 7300
		)
		return DirectConnectTo();
#endif

	return AfterProcessSocksTcpConnection();
}
/** @brief ����socks5 udp ��������
*/
BOOL CSocksClient::ProcessSocksUdpConnection()
{
	PrintfW( LEVEL_INFO, _T("CSocksClient::ProcessSocksUdpConnection...") );

	SOCKET s = INVALID_SOCKET;
	s = CreateSocket( TRUE, FALSE );
	if( s == INVALID_SOCKET )
	{
		_MakeSocksRspV5( SOCKS_ERR_GENERAL, 0 , 0 );

		PrintfW( LEVEL_ERROR, _T("CSocksClient::ProcessSocksUdpConnection: Create UDP ASSOCIATE socket error.") );

		return FALSE;
	}
	PrintfW( LEVEL_INFO, _T("CSocksClient::ProcessSocksUdpConnection: UDP ASSOCIATE socket created. Socket(%d)."), s );

	struct sockaddr_in serverbind_ssin,clientBindSsin;
	memset((char *)&serverbind_ssin, 0, sizeof(struct sockaddr_in));
	memset((char *)&clientBindSsin, 0, sizeof(struct sockaddr_in));

	if( !BindSocket( s ))
	{
		_MakeSocksRspV5( SOCKS_ERR_GENERAL, 0 , 0 );

		CLOSESOCKET( s );

		return FALSE;
	}

	/*
    * Get information about ip and port after bind operation
    * to send to client
    */
    int len = sizeof (struct sockaddr_in);
    getsockname( s,(struct sockaddr *)&serverbind_ssin,&len);

	/*
     * SS5: create response to send to client
     */
	_MakeSocksRspV5( SOCKS_ERR_SECCEEDED, serverbind_ssin.sin_addr.s_addr , serverbind_ssin.sin_port );

	udpSocket = s;

	return AfterProcessSocksUdpConnection();
}
/** @brief ����socks5 bind����
����CMDΪBIND���������FTPЭ�飬FTPЭ����ĳЩ�����Ҫ��FTP Server��������
��FTP Client�����ӣ���FTP��������

FTP Client - SOCKS Client - SOCKS Server - FTP Server

a. FTP Client��ͼ����FTP��������SOCKS Client��SOCKS Server����CONNECT����
������Ӧ��������FTP������������

CONNECT�������ָ��FTPSERVER.ADDR/FTPSERVER.PORT��

b. FTP Client��ͼ����FTP��������SOCKS Client�����µĵ�SOCKS Server��TCP��
�ӣ������µ�TCP�����Ϸ���BIND����

BIND���������Ȼָ��FTPSERVER.ADDR/FTPSERVER.PORT��SOCKS ServerӦ�þݴ�
����������

SOCKS Server�յ�BIND���󣬴������׽��֣�������AddrA/PortA�ϣ�����SOCKS
Client���͵�һ��BIND��Ӧ��������BND.ADDR/BND.PORT��AddrA/PortA��

c. SOCKS Client�յ���һ��BIND��Ӧ����FTP Clientͨ��FTP��������FTP Server��
��PORT���֪ͨFTP ServerӦ������������AddrA/PortA��TCP���ӡ�

d. FTP Server�յ�PORT�������������AddrA/PortA��TCP���ӣ�����TCP�������
��Ԫ����:

AddrB��PortB��AddrA��PortA

e. SOCKS Server�յ�����FTP Server��TCP����������SOCKS Client���͵ڶ���
BIND��Ӧ��������BND.ADDR/BND.PORT��AddrB/PortB��Ȼ��SOCKS Server��ʼת
��FTP��������
*/
BOOL CSocksClient::ProcessSocksV5Bind()
{
	PrintfW( LEVEL_INFO, _T("CSocksClient::ProcessSocksV5Bind...") );

	UINT err = 0;
	SOCKET s = INVALID_SOCKET;

	// create tcp blocking socket.
	s = CreateSocket( FALSE, FALSE );

	if( s == INVALID_SOCKET )
	{
		_MakeSocksRspV5( SOCKS_ERR_GENERAL, 0 , 0 );
		PrintfW( LEVEL_ERROR, _T("CSocksClient::ProcessSocksV5Bind: Create BIND socket error.") );
		return FALSE;
	}
	PrintfW( LEVEL_INFO, _T("CSocksClient::ProcessSocksV4Bind: BIND socket created. Socket(%d)..."), s );

	if( !BindSocket( s ))
	{
		_MakeSocksRspV5( SOCKS_ERR_GENERAL, 0 , 0 );

		CLOSESOCKET( s );

		return FALSE;
	}

	int len;
	struct sockaddr_in applicationSsin,clientBindSsin;
	/*
    * Get clientbind info
    */
    len=sizeof(struct sockaddr);
	memset((char *)&clientBindSsin, 0, sizeof(struct sockaddr_in));
	getsockname( s,(struct sockaddr *)&clientBindSsin,&len);

	/*
    * SS5: listen for a queue length equal to one
    */ 
    if (listen(s, 1) == -1) {
		_MakeSocksRspV5( SOCKS_ERR_GENERAL, 0 , 0 );

		PrintfW( LEVEL_ERROR, _T("CSocksClient::ProcessSocksV5Bind: Listen on BIND socket error.") );

		CLOSESOCKET( s );

		return FALSE;
    }

	/*
   * Send socks response
   */
	_MakeSocksRspV5( SOCKS_ERR_SECCEEDED, clientBindSsin.sin_addr.s_addr , clientBindSsin.sin_port );

	memset( &applicationSsin, 0 , sizeof( struct sockaddr_in ) );
	len = sizeof (struct sockaddr_in);

	fd_set fdset;
	struct timeval tv;
	int fd;
	
	FD_ZERO(&fdset);
	FD_SET(s,&fdset);
	
	//�ȴ�120��
	tv.tv_sec=BIND_TIMEOUT;
	tv.tv_usec=0;

	fd = select( 0, &fdset, NULL, NULL , &tv );
	 /*
    * Timeout expired accepting connection from remote application
    */
	if( fd == 0 || fd == SOCKET_ERROR )
	{
		PrintfW( LEVEL_ERROR, _T("CSocksClient::ProcessSocksV5Bind: accept select timeout or met other errors.") );

		CLOSESOCKET( s );

		return FALSE;
	}

	if( FD_ISSET( s, &fdset ) )
	{
		SOCKET sRemote = accept( s, (struct sockaddr *)&applicationSsin, &len );

		if( sRemote == INVALID_SOCKET )
		{
			PrintfW( LEVEL_ERROR, _T("CSocksClient::ProcessSocksV5Bind: accept error.") );

			CLOSESOCKET( s );

			return FALSE;
		}

		hRemoteSocket = sRemote;

		//In the second reply, the BND.PORT and BND.ADDR fields contain the
		//	address and port number of the connecting host.
		/*
		* Send socks response
		*/
		_MakeSocksRspV5( SOCKS_ERR_SECCEEDED, applicationSsin.sin_addr.s_addr , applicationSsin.sin_port );

		CLOSESOCKET( s );

		return TRUE;
	}

	CLOSESOCKET( s );

	return FALSE;
}
/**
The client connects to the SOCKS server and sends a BIND request when
it wants to prepare for an inbound connection from an application server.
This should only happen after a primary connection to the application
server has been established with a CONNECT.  Typically, this is part of
the sequence of actions:

-bind(): obtain a socket
-getsockname(): get the IP address and port number of the socket
-listen(): ready to accept call from the application server
-use the primary connection to inform the application server of
the IP address and the port number that it should connect to.
-accept(): accept a connection from the application server
*/
BOOL CSocksClient::ProcessSocksV4Bind()
{
	PrintfW( LEVEL_INFO, _T("CSocksClient::ProcessSocksV4Bind...") );

	UINT err = 0;
	SOCKET s = INVALID_SOCKET;

	// create tcp blocking socket.
	s = CreateSocket( FALSE, FALSE );

	if( s == INVALID_SOCKET )
	{
		_MakeSocksRspV4( SOCKS4_ERR_REJECTED, 0 , 0 );
		PrintfW( LEVEL_ERROR, _T("CSocksClient::ProcessSocksV4Bind: Create BIND socket error.") );
		return FALSE;
	}
	PrintfW( LEVEL_INFO, _T("CSocksClient::ProcessSocksV4Bind: BIND socket created. Socket(%d)..."), s );

	if( !BindSocket( s ))
	{
		_MakeSocksRspV4( SOCKS4_ERR_REJECTED, 0 , 0 );

		CLOSESOCKET( s );

		return FALSE;
	}

	int len;
	struct sockaddr_in applicationSsin,clientBindSsin;
	/*
    * Get clientbind info
    */
    len=sizeof(struct sockaddr);
	memset((char *)&clientBindSsin, 0, sizeof(struct sockaddr_in));
	getsockname( s,(struct sockaddr *)&clientBindSsin,&len);

	/*
    * SS5: listen for a queue length equal to one
    */ 
    if (listen(s, 1) == -1) {
		_MakeSocksRspV4( SOCKS4_ERR_REJECTED, 0 , 0 );

		PrintfW( LEVEL_ERROR, _T("CSocksClient::ProcessSocksV4Bind: Listen on BIND socket error.") );

		CLOSESOCKET( s );

		return FALSE;
    }

	/*
   * Send socks response
   */
	_MakeSocksRspV4( SOCKS4_ERR_GRANTED, clientBindSsin.sin_addr.s_addr , clientBindSsin.sin_port );

	memset( &applicationSsin, 0 , sizeof( struct sockaddr_in ) );
	len = sizeof (struct sockaddr_in);

	fd_set fdset;
	struct timeval tv;
	int fd;
	
	FD_ZERO(&fdset);
	FD_SET(s,&fdset);
	
	//�ȴ�120��
	tv.tv_sec=BIND_TIMEOUT;
	tv.tv_usec=0;

	fd = select( 0, &fdset, NULL, NULL , &tv );
	 /*
    * Timeout expired accepting connection from remote application
    */
	if( fd == 0 || fd == SOCKET_ERROR )
	{
		PrintfW( LEVEL_ERROR, _T("CSocksClient::ProcessSocksV4Bind: accept select timeout or met other errors.") );

		CLOSESOCKET( s );

		return FALSE;
	}

	if( FD_ISSET( s, &fdset ) )
	{
		SOCKET sRemote = accept( s, (struct sockaddr *)&applicationSsin, &len );

		if( sRemote == INVALID_SOCKET )
		{
			PrintfW( LEVEL_ERROR, _T("CSocksClient::ProcessSocksV4Bind: accept error.") );

			CLOSESOCKET( s );

			return FALSE;
		}

		hRemoteSocket = sRemote;

		//In the second reply, the BND.PORT and BND.ADDR fields contain the
		//	address and port number of the connecting host.
		/*
		* Send socks response
		*/
		_MakeSocksRspV4( SOCKS4_ERR_GRANTED, applicationSsin.sin_addr.s_addr , applicationSsin.sin_port );

		CLOSESOCKET( s );

		return TRUE;
	}

	CLOSESOCKET( s );

	return FALSE;
}
/* @brief ��ȡ���ӿͻ��� ��Ϣ
*/
void CSocksClient::_GetClientInfo( )
{
  int len = sizeof( SocksSrcAddr );

  //struct sockaddr_in sockAddr;
  //struct in_addr in;

  /*
   *    Get socket name, ip address and port
   */

  if( getpeername( hSocksSocket,(struct sockaddr *)&SocksSrcAddr,&len) == -1 ) {
    return;
  }

  //in.s_addr=sockAddr.sin_addr.s_addr;
  //strncpy( SrcAddr,(char *)inet_ntoa(in),sizeof(SrcAddr) );

  //SrcPort=ntohs(sockAddr.sin_port);

  PrintfA( LEVEL_INFO, ("CSocksClient::_GetClientInfo: client info( %s:%d)"), inet_ntoa( ((struct sockaddr_in*)&SocksSrcAddr)->sin_addr ), ntohs( ((struct sockaddr_in*)&SocksSrcAddr)->sin_port ) );

  return;
}
/** @brief �Ƿ��յ���Ч������
*/
BOOL CSocksClient::_IsReceivedValidCommand( UINT cmd )
{
	/*
     * Validate SOCKS5 command field
     */
    if( (cmd > SOCKS_CMD_UDP_ASSOCIATE) || (cmd < SOCKS_CMD_CONNECT ) )
      return FALSE;

	return TRUE;
}
/*
+----+----+----+----+----+----+----+----+
| VN | CD | DSTPORT |      DSTIP        |
+----+----+----+----+----+----+----+----+
#  1    1      2              4
*/
BOOL CSocksClient::_MakeSocksRspV4( UINT nErrCode , u_long addr, u_short port /** ������� */ )
{
	char buf[20] = {0};
	int len = 8;

	buf[0] = 0;
	buf[1] = nErrCode;
	memcpy( buf + 2, &port, 2 );
	memcpy( buf + 4, &addr, 4 );

	int ret = _LocalSend( buf, len );
	if( ret != len )
	{
		PrintfW( LEVEL_ERROR, _T("CSocksClient::_MakeSocksRspV4: code:%d"),nErrCode );
		return FALSE;
	}

	return TRUE;
}

/** @brief ����socks���������
* ���nErrCode = 0, ��ôaddr, portӦ�ñ�����Ӧ�ĵ�ַ�Ͷ˿�,nErrCode != 0 ʱaddr, port����ν.
*/
BOOL CSocksClient::_MakeSocksRspV5( UINT nErrCode , u_long addr, u_short port /** ������� */ )
{
	if( nErrCode < 0 || nErrCode > 0xFF )
		return FALSE;

	UINT len = 10;
	char buf[10] = {0};

	/*
	+----+-----+-------+------+----------+----------+
	|VER | REP |  RSV  | ATYP | BND.ADDR | BND.PORT |
	+----+-----+-------+------+----------+----------+
	| 1  |  1  | X'00' |  1   | Variable |    2     |
	+----+-----+-------+------+----------+----------+

	Where:

	o  VER    protocol version: X'05'
	o  REP    Reply field:
	o  X'00' succeeded
	o  X'01' general SOCKS server failure
	o  X'02' connection not allowed by ruleset
	o  X'03' Network unreachable
	o  X'04' Host unreachable
	o  X'05' Connection refused
	o  X'06' TTL expired
	o  X'07' Command not supported
	o  X'08' Address type not supported
	o  X'09' to X'FF' unassigned
	o  RSV    RESERVED
	o  ATYP   address type of following address
	o  IP V4 address: X'01'
	o  DOMAINNAME: X'03'
	o  IP V6 address: X'04'
	o  BND.ADDR       server bound address IPV4��4�ֽ�, IPV6��16�ֽ�
	o  BND.PORT       server bound port in network octet order
	*/
	buf[0] = SOCKS_V5;
	buf[1] = nErrCode;
	buf[2] = SOCKS_RESERVED;
	buf[3] = SOCKS_ATYPE_IPV4;
	memcpy( buf + 4, &addr, 4 );
	memcpy( buf + 8, &port, 2 );

	int ret = _LocalSend( buf, 10 );
	if( ret != 10 )
	{
		PrintfW( LEVEL_ERROR, _T("CSocksClient::_MakeSocksRspV5: code:%d"),nErrCode );
		return FALSE;
	}

	return TRUE;
}
/** @brief ���UDP�������İ���ԭ�Ƚ�����TCP������ָ����UDP��ַ,�˿��Ƿ�һ��.
* �������TCP�����е�ָ����UDP��ַ�Ͷ˿���0,������ֱ��ͨ��
*
* @return TRUE ���ͨ��, FALSE ��ͨ��, �յ��İ����Ե�
*/
BOOL CSocksClient::_CheckUdpSrcBindAddr( struct sockaddr *addr )
{
	assert( addr != NULL );

	if( addr == NULL ) return TRUE;

	struct sockaddr_in *addr_bnd = (sockaddr_in *)&udpSrcBindAddr;
	struct sockaddr_in *addr_pkg = (sockaddr_in *)addr;

	BOOL addr_check = TRUE;
	BOOL port_check = TRUE;

	if( addr_bnd->sin_addr.s_addr != 0 
		&& addr_bnd->sin_addr.s_addr != addr_pkg->sin_addr.s_addr 
		)
		addr_check = FALSE;

	if( addr_bnd->sin_port != 0 
		&& addr_bnd->sin_port != addr_pkg->sin_port 
		)
		port_check = FALSE;

	if( !addr_check || !port_check )
		return FALSE;

	return TRUE;
}
void CSocksClient::SetProxyListenPort( u_short port )
{
	nProxyListenPort = port;
}
/** @brief ��֤�ʺ�����
*/
BOOL CSocksClient::VerifySocksUsernamePassword( char *username, char * password )
{
	// �ʺ�,���붼δָ��, ����Ϊ��������
	if( szProxyUser[0] == 0 && szProxyPass[0] == 0 )
		return TRUE;

	BOOL bUserVerfied = FALSE;
	BOOL bPassVerified = FALSE;

	// verifing user
	if( szProxyUser[0] != 0 )
	{
		if( username )
		{
			if( strcmp( szProxyUser, username ) == 0 )
				bUserVerfied = TRUE;
		}
	}

	if( szProxyPass[0] != 0 )
	{
		if( password )
		{
			if( strcmp( szProxyPass,password ) == 0 )
			{
				bPassVerified = TRUE;
			}
		}
	}

	if( bUserVerfied && bPassVerified )
		return TRUE;

	// ���ǲ���֤�ʺ�����, ֱ�ӷ���TRUE;
	return FALSE;
	//return VerifyLocalSocksUserPass( username, password );
}

/** @brief method�Ƿ���Ч. ����ֻ֧��������USER/PASS������֤��ʽ
*/
BOOL CSocksClient::_IsValidProxyMethod( int method )
{
	if( method == SOCKS_AUTH_ANONYMOUS || method == SOCKS_AUTH_USERPASS || method == SOCKS_AUTH_GSSAPI )
		return TRUE;

	return FALSE;
}