#pragma once

/*!
 * @file SSClient.h
 * @date 2015/10/19 10:31
 *
 * @brief shadowsocks����ز�����
 *
 * Shadowsocks protocol
 * https://shadowsocks.org/en/spec/protocol.html
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
/*
 *
 * SOCKS UDP Request
 * Socks Client����SSCAP��UDP�ṹ��������.
 * +----+------+------+----------+----------+----------+
 * |RSV | FRAG | ATYP | DST.ADDR | DST.PORT |   DATA   |
 * +----+------+------+----------+----------+----------+
 * | 2  |  1   |  1   | Variable |    2     | Variable |
 * +----+------+------+----------+----------+----------+
 *
 * SOCKS5 UDP Response
 * SSCap����Socks Client��UDP�ṹ��������.
 * +----+------+------+----------+----------+----------+
 * |RSV | FRAG | ATYP | DST.ADDR | DST.PORT |   DATA   |
 * +----+------+------+----------+----------+----------+
 * | 2  |  1   |  1   | Variable |    2     | Variable |
 * +----+------+------+----------+----------+----------+
 *
 * shadowsocks UDP Request (before encrypted)
 * SSCap����Shadowsocks ��UDP�ṹ��������.
 * +------+----------+----------+----------+
 * | ATYP | DST.ADDR | DST.PORT |   DATA   |
 * +------+----------+----------+----------+
 * |  1   | Variable |    2     | Variable |
 * +------+----------+----------+----------+
 *
 * shadowsocks UDP Response (before encrypted)
 * Shadowsocks ���ظ�SSCap��UDP�ṹ��������.
 * +------+----------+----------+----------+
 * | ATYP | DST.ADDR | DST.PORT |   DATA   |
 * +------+----------+----------+----------+
 * |  1   | Variable |    2     | Variable |
 * +------+----------+----------+----------+
 *
 * shadowsocks UDP Request and Response (after encrypted)
 * SSCap��Shadowsocks����������ʱ���ܺ��������������
 * ��һ�����ݰ������IV,�����İ����� 
 * +-------+--------------+
 * |   IV  |    PAYLOAD   |
 * +-------+--------------+
 * | Fixed |   Variable   |
 * +-------+--------------+
 *
 * SSCap��Shadowsocks������������TCP���ݽṹ
 * ֻ�е�һ�����������ͷ. ֮��İ����������ͷ.
 * +------+----------+----------+----------+
 * | ATYP | DST.ADDR | DST.PORT |   DATA   |
 * +------+----------+----------+----------+
 * |  1   | Variable |    2     | Variable |
 * +------+----------+----------+----------+
 *
 * tcp�����ܺ�Ľṹ��
 * ��һ�����ݰ������IV,�����İ����� 
 * +-------+--------------+
 * |   IV  |    PAYLOAD   |
 * +-------+--------------+
 * | Fixed |   Variable   |
 * +-------+--------------+
 */
class CSocksClient;
#include "BaseDef.h"
#include "Encypter.h"
#define MIN_CHECK_SPEED_TIME 0.005

class CSSCLient: public CSocksClient
{
public:
	CSSCLient( SOCKET s ,const char *username =NULL  /* proxy user */, const char *password = NULL/* proxy pass */);
	virtual ~CSSCLient();

	/** @brief ����socks 5����
	*/
	virtual BOOL Start();

	/** @brief ȡ�õ�ǰ�ڵ���Ϣ
	*/
	CSSNodeInfo *GetNodeInfo()
	{
		return pNodeInfo;
	}

	/** @brief �û�ϣ��������ֹ��ǰ����ת������
	*/
	void SetUserBreaking();
protected:
	CSSNodeInfo *pNodeInfo;
	CCryptor *pCryptor;

	//double nSentKB;// �ѷ���KB
	//double nRecvKB; // �ѽ���KB
	//double nSentSeconds; // ������ʱ(��)
	//double nRecvSeconds; // ������ʱ(��)
	//unsigned long nUpdateTimes;

	BOOL bIsCipher;	// �����ķ�, ��privoxy����������������
	//string strPacBodyCache;	// ����PAC�ļ����ݻ���.

	BOOL bUserBreaking;		// �û�������ֹ��ǰ����
protected:
	/** @brief ����socks udp ��������
	*/
	virtual BOOL ProcessSocksUdpConnection();

	/** @brief ִ����ProcessSocksTcpConnection֮��Ĳ���
	*/
	virtual BOOL AfterProcessSocksTcpConnection();
	/** @brief ִ����ProcessSocks5UdpConnection֮��Ĳ���
	*/
	virtual BOOL AfterProcessSocksUdpConnection();
	/** @brief ������������
	* 
	* @returns
	*	0: ʧ��, �����߿���ֱ�ӷ�����.
	*	1: �ɹ�, ���ǵ����߿���ֱ�ӷ��ز���Ҫ���Ŵ���MainLoop��
	*	2: �ɹ�, ��������Ҫ����ȥ�����±ߵ�MainLoop
	*/
	virtual int ProcessOtherRequest(int nUnrecognizedByte);
	/** @brief ִ�����ݴ��ݵ�loop
	*/
	virtual BOOL MainLoop();
	BOOL UDPMainLoop();
	BOOL TCPMainLoop();
protected:
	/** @brief ִ��shadowsocks�ķ��Ͳ���
	* ��Ϊ����Զ��SS�������ͷ���SOCKS CLIENT
	* SS CLIENT -> SS SERVER :����Ҫִ�м��ܹ���
	* SS SERVER -> SS CLIENT :  ��Ҫִ�н��ܹ���
	*/
	int SSSend( SOCKET s, char *buffer, int length  ,int timeout );
	int SSSendTo( SOCKET s, char *buffer, int length  ,int timeout );

	/** @brief ִ��shadowsocks�Ľ��ղ���
	* ��Ϊ����Զ��SS�������ͽ���SOCKS CLIENT
	* SS CLIENT -> SS SERVER :����Ҫִ�м��ܹ���
	* SS SERVER -> SS CLIENT :  ��Ҫִ�н��ܹ���
	*/
	int SSRecv( SOCKET s,  char *buffer, int length  ,int timeout );
	int SSRecvFrom( SOCKET s,  char *buffer, int length  ,int timeout );

	BOOL IsRemoteSocket( SOCKET s )
	{
		return s == hRemoteSocket ? TRUE: FALSE;
	}
	BOOL IsLocalSocket( SOCKET s )
	{
		return s == hSocksSocket ? TRUE: FALSE;
	}
	/** @brief �ǲ���UDP CLIENT SOCKET , �ȴ�SOCKS CLIENT�������ݵ�UDP SOCKET
	*/
	BOOL IsUdpClientSocket( SOCKET s )
	{
		return s == udpSocket ? TRUE : FALSE;
	}
	/** @brief ���ӵ�ss ������
	* 
	* @param bUdp: ��udp ����
	* @param bNonBlocking: ������
	*/
	virtual BOOL ConnectToSSServer( BOOL bUdp = FALSE, BOOL bNonBlocking = TRUE ,SOCKADDR_IN *pSockAddrIn = NULL );
	/** @brief  ����SSЭ��
	* https://shadowsocks.org/en/spec/protocol.html
	* +--------------+---------------------+------------------+----------+
	* | Address Type | Destination Address | Destination Port |   Data   |
	* +--------------+---------------------+------------------+----------+
	* |      1       |       Variable      |         2        | Variable |
	* +--------------+---------------------+------------------+----------+
	*/
	BOOL SendSSProtocol();
	/** @brief ��������ĵ�ַ. �п�����һ������, �������IP
	* ��������ɹ�, ����Ĵ�ַ��Ϣͨ��sockAddr����
	*/
	BOOL TranslateSSServerAddress( SOCKADDR_IN &sockAddr , char *szAddress, u_short port );

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
	BOOL Add3BytesForUdpHeader( char *buffer, int DataLen, int BufLen );
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
	BOOL Remove3BytesForUdpHeader( char *bufer, int DataLen );
	/** @brief ����PAC�ļ�����
	*/
	BOOL RespPacFileBody();
	BOOL ConnectToPrivoxy();
};