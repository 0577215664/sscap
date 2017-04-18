#pragma once

/*!
 * @file SocketBase.h
 * @date 2015/10/19 10:52
 *
 * @brief socket�Ļ���, ʵ��socket����ػ�������
 *
 * 
 * @author Taro
 * Contact: sockscap64@gmail.com
 *
 *
 * @version 1.0
 *
 * @History
 * <author>      <time>      <version>         <desc>
 *
 * @TODO: long description
 *
 * @note
*/

#define CLOSESOCKET(x) { if( x != INVALID_SOCKET ){ closesocket( x ) ; x = INVALID_SOCKET ;} }

class CSocketBase
{
public:
	CSocketBase();
	virtual ~CSocketBase();

protected:
	/** @brief ����TCP NAGLE �㷨
	*  ������ͨ�� TCP socket �ֶ�η��ͽ��ٵ�����ʱ������С�� 1460 ���� 100 ���ڣ��Զ˿��ܻ�ܳ�ʱ���ղ������ݣ����±���Ӧ�ó�����Ϊ��ʱ������ʱ�������ܵ��� TCP NAGLE �㷨��Ӱ�졣
	*/
	void SetNodelay( SOCKET s );
	/** @brief ��socket�н���һ���ֽ�
	*/
	virtual  BOOL _GetOneByteFromSocket( SOCKET s, int &n ,int timeout );
	
	virtual void _SetSocketTimeout( SOCKET s, int nTimeout = 5 /** Ĭ��5��*/, int t = 0 /** 0: �����պͷ���ʱ, 1: ֻ������, 2: ֻ���÷�*/ );

	virtual int _Recv( SOCKET s,  char *buffer, int length  ,int timeout);
	virtual int _RecvFrom( SOCKET s,  char *buffer, int length  , struct sockaddr* AddrFrom, int *AddrLen,int timeout);

	virtual int _Send( SOCKET s, char *buffer, int length  ,int timeout);
	virtual int _SendTo( SOCKET s, char *buffer, int length  ,const struct sockaddr *To, int ToLen,int timeout);

	/** @brief �����׽���
	*/
	SOCKET CreateSocket( BOOL bIsUdp = FALSE /* FALSE: TCP, TRUE: UDP */, BOOL bNonBlocking = TRUE );
	void SetNoBlocking( SOCKET s , BOOL bIsNonBlocking = TRUE /** TRUE: NON- BLOCKING, FALSE: BLOCKING */ );

	/** @brief ���׽���
	*/
	BOOL BindSocket( SOCKET s, u_long addr = ADDR_ANY , u_short port = 0 );
	/*  @brief Waits until socket states completion of operation
	* @param[in]		hSock			Socket for calling "select"
	* @param[in] nTimeout timeout in secods.
	* @return							0 on success, SOCKET_ERROR on error (use WSAGetLastError to get error info)
	*/
	int WaitCompletion(SOCKET hSock , int nTimeout /* seconds */);
};