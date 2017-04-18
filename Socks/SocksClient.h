#pragma once

/*!
 * \file SocksClient.h
 * @All Right Reserved (C), 2014-2015, sockscap64.com
 * Socks5����Ļ�����
 * ִ��Socks5�����һЩ��������
 *
 * \author Taro
 * \date ʮ�� 2015
 * \version 1.0
 * \remark
 *
 * \History
 * <author>      <time>      <version>         <desc>
 * 
 * 
 */


#define SOCKS_V5 0x05
#define SOCKS_V4 0x04

#define SOCKS_AUTH_ANONYMOUS 0x00	// X'00' NO AUTHENTICATION REQUIRED
#define SOCKS_AUTH_GSSAPI 0x01	// X'01' GSSAPI
#define SOCKS_AUTH_USERPASS 0x02	// X'02' USERNAME/PASSWORD
// X'03' to X'7F' IANA ASSIGNED
// X'80' to X'FE' RESERVED FOR PRIVATE METHODS

// X'FF' NO ACCEPTABLE METHODS
#define SOCKS_AUTH_NO_METHODS 0xFF

//  The VER field contains the current version of the subnegotiation, which is X'01'
// http://tools.ietf.org/html/rfc1929
#define SOCKS_AUTH_VERSION 0x01

#define SOCKS_MAX_USER_LEN 255
#define SOCKS_MAX_PASS_LEN 255
#define SOCKS_MAX_DOMAIN_LEN 255

/**
o  CONNECT X'01'
o  BIND X'02'
o  UDP ASSOCIATE X'03'
*/
#define SOCKS_CMD_CONNECT 0x01
#define SOCKS_CMD_BIND 0x02
#define SOCKS_CMD_UDP_ASSOCIATE 0x03

#define SOCKS_ATYPE_IPV4 0x01
#define SOCKS_ATYPE_DOMAIN 0x03
#define SOCKS_ATYPE_IPV6 0x04

//o  X'00' succeeded
#define SOCKS_ERR_SECCEEDED 0x00
//o  X'01' general SOCKS server failure
#define SOCKS_ERR_GENERAL 0x01
//o  X'02' connection not allowed by ruleset
#define SOCKS_ERR_CONNECTION_NOT_ALLOWED 0x02
//o  X'03' Network unreachable
#define SOCKS_ERR_NETWORK_UNREACHABLE 0x03
//o  X'04' Host unreachable
#define SOCKS_ERR_HOST_UNREACHABLE 0x04
//o  X'05' Connection refused
#define SOCKS_ERR_CONNECTION_REFUSED 0x05
//o  X'06' TTL expired
#define SOCKS_ERR_TTL_EXPRED 0x06
//o  X'07' Command not supported
#define SOCKS_ERR_COMMAND_NOT_SUPPORTED 0x07
//o  X'08' Address type not supported
#define SOCKS_ERR_ADDRESS_NO_SUPPRTED 0x08
//o  X'09' to X'FF' unassigned

// �����ֶ�
#define SOCKS_RESERVED 0x00

// Ĭ�ϳ�ʱ:5 ��
#define DEFAULT_TIMEOUT 10

#define MAX_UDP_PACKET_SIZE 65507
#define BUF_SIZE MAX_UDP_PACKET_SIZE

// BIND����ʱ�ĳ�ʱʱ��
#define BIND_TIMEOUT    120     /* Seconds */
// UDP�ȴ�ʱ��
#define UDP_TIMEOUT     60      /* Seconds */

// socks v4 connect
#define SOCKS4_CMD_CONNECT 0x01
#define SOCKS4_CMD_BIND 0x02

// 	90: request granted
#define SOCKS4_ERR_GRANTED 90
//	91: request rejected or failed
#define SOCKS4_ERR_REJECTED 91
// 	92: request rejected becasue SOCKS server cannot connect to identd on the client
#define SOCKS4_ERR_CANNOT_CONNECT 92
// 	93: request rejected because the client program and identd report different user-ids
#define SOCKS4_ERR_UNAUTH 93
#define TCP_BUFSIZE 1024

class CSocketBase;
class CSocksClient : public CSocketBase
{
public:
	CSocksClient( SOCKET s ,const char *username =NULL  /* proxy user */, const char *password = NULL/* proxy pass */ );
	
	virtual ~CSocksClient();

protected:
	SOCKET hSocksSocket; ///< socks5�������׽���
	SOCKET hRemoteSocket; ///< ����Զ�̵�SOCKET. UDP ASSOCIATEʱ��UDP��

	//BOOL bNeedAuthByUserPass; ///< ��Ҫͨ��user, pass��֤
	UINT Ver;
	UINT Cmd;
	UINT Rsv;
	UINT ATyp;

	char szSocksV4UserId[256];
	char uDestinationAddr[ SOCKS_MAX_DOMAIN_LEN + 1 ];		 ///< Ŀ���ַ
	u_short uDestinationPort; ///< Ŀ�Ķ˿�
	//char szDestinationDomainName[SOCKS_MAX_DOMAIN_LEN + 1]; ///< Ŀ������

	//char SrcAddr[16];
	//u_short SrcPort;
	struct sockaddr SocksSrcAddr;

	SOCKET udpSocket;			// UDP ����SOCKS CLIENT�������İ���SOCKET
	//char udpSrcAddr[16];  // UDP ��������ʱ, ���ͷ���Դ��ַ.
	//UINT udpSrcPort;		// UDP ��������ʱ, ���ͷ���Դ�˿�.
	struct sockaddr udpSrcAddr;

	//char udpSrcBindAddr[16]; // udp, ����TCP����ʱ, ���͹�����Դ�󶨵�ַ.  �Ժ��������ʱ���ж�һ���Ƿ��Դ�󶨵�ַһ��. ������⹥��.
	//UINT udpSrcBindPort;	// udp, ����TCP����ʱ, ���͹�����Դ�󶨶˿�.   �Ժ��������ʱ���ж�һ���Ƿ��Դ�󶨵�ַһ��. ������⹥��.
	struct sockaddr udpSrcBindAddr;

	struct sockaddr_in udpDstAddr;	// udp��Ŀ�ĵ�ַ.

	int nUnrecognizedByte;	//����δ��ʶ�������,��Ϊ�Ѿ���ȡ��һ������,�������ﱣ����������.
	BOOL bFoundUnrecognizedByte; // ����δ��ʶ�������.
	BOOL bCouldProcessOtherRequest; // ���Դ�����������,��HTTP��������, PAC�ļ�����.
	u_short nProxyListenPort;	//���ش�������Ķ˿�. �����õ�.
	
	char szProxyUser[256]; ///< �����û���
	char szProxyPass[256];	///< ��������

public:
	void SetProxyListenPort( u_short port );
	/** @brief ����socks 5����
	*/
	virtual BOOL Start();
	/** @brief ���ÿ��Դ�����������, HTTP,HTTPS����,PAC�ļ�
	*/
	void SetCouldProcessOtherRequest( BOOL bCould )
	{
		bCouldProcessOtherRequest = bCould;
	}
	BOOL IsGotUnrecognizedByte()
	{
		return bFoundUnrecognizedByte;
	}
	int GetUnrecognizedByte()
	{
		return nUnrecognizedByte;
	}
protected:
	/** @brief Socks5 auth medthod 
	*/
	virtual BOOL ProcessSocksAuthMethod();
	/** @brief ����Socks v4����
	*/
	virtual BOOL ProcessSocksV4();
	/** @biref socks5 auth 
	*/
	virtual BOOL ProcessSocksAuth();
	/** @brief ����socks��������
	*/
	virtual BOOL ProcessSocksConnection();
	/** @brief ����socks5 tcp ��������
	*/
	virtual BOOL ProcessSocksTcpConnection();
	/** @brief ����socks5 udp ��������
	*/
	virtual BOOL ProcessSocksUdpConnection();
	/** @brief ����socks5 bind����
	*/
	virtual BOOL ProcessSocksV5Bind();
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
	virtual BOOL ProcessSocksV4Bind();
protected:
	/** @brief ��֤�ʺ�����
	*/
	virtual BOOL VerifySocksUsernamePassword( char *username, char * password );
	/** @brief ִ����ProcessSocks5TcpConnection֮��Ĳ���
	*/
	virtual BOOL AfterProcessSocksTcpConnection() = 0 ;
	/** @brief ִ����ProcessSocks5UdpConnection֮��Ĳ���
	*/
	virtual BOOL AfterProcessSocksUdpConnection() = 0 ;
	/** @brief ������������
	* 
	* @returns
	*	0: ʧ��, �����߿���ֱ�ӷ�����.
	*	1: �ɹ�, ���ǵ����߿���ֱ�ӷ��ز���Ҫ���Ŵ���MainLoop��
	*	2: �ɹ�, ��������Ҫ����ȥ�����±ߵ�MainLoop
	*/
	virtual int ProcessOtherRequest( int nUnrecognizedByte ) = 0;
	/** @brief ִ�����ݴ��ݵ�loop
	*/
	virtual BOOL MainLoop() = 0;
protected:
	BOOL DirectConnectTo();
	void CloseAllSocket();
	/** @brief method�Ƿ���Ч. ����ֻ֧��������USER/PASS������֤��ʽ
	*/
	BOOL _IsValidProxyMethod( int method );
	/** @brief ���UDP�������İ���ԭ�Ƚ�����TCP������ָ����UDP��ַ,�˿��Ƿ�һ��.
	* �������TCP�����е�ָ����UDP��ַ�Ͷ˿���0,������ֱ��ͨ��
	*
	* @return TRUE ���ͨ��, FALSE ��ͨ��, �յ��İ����Ե�
	*/
	BOOL _CheckUdpSrcBindAddr(struct sockaddr *addr );
	/** @brief �Ƿ��յ���Ч������
	*/
	BOOL _IsReceivedValidCommand( UINT cmd );
	/** �Ƿ��յ�һ����Ч��SOCKS�汾��
	*/
	BOOL _IsReceivedValidSocksVersion( UINT &r_version );

	/** @brief ��local socket( app�����socket ) �н���һ���ֽ�
	*/
	virtual BOOL _GetOneByteFromLocalSocket( int &n );
	/** @brief ��local socket( ���ӵ�Ŀ�ĵص�socket ) �н���һ���ֽ�
	*/
	virtual BOOL _GetOneByteFromRemoteSocket( int &n );
	
	virtual int _LocalRecv( char *buffer, int length );
	virtual int _RemoteRecv( char *buffer, int length );
	virtual int _LocalSend( char *buffer, int length );
	virtual int _RemoteSend( char *buffer, int length );
	/* @brief ��ȡ���ӿͻ��� ��Ϣ
	*/
	void _GetClientInfo( );
	/** @brief ����socks���������
	* ���nErrCode = 0, ��ôaddr, portӦ�ñ�����Ӧ�ĵ�ַ�Ͷ˿�,nErrCode != 0 ʱaddr, port����ν.
	*/
	BOOL _MakeSocksRspV5( UINT nErrCode , u_long addr, u_short port /** ������� */ );
	BOOL _MakeSocksRspV4( UINT nErrCode , u_long addr, u_short port /** ������� */ );
};