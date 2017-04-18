#pragma once

/*!
 * @file ProxySocketClient.h
 * @date 2015/06/12 15:54
 *
 * @brief ͨ�������SOCKET. ��Ҫ���dns��ʹ��,ͨ����������DNS����.�Դ����ж�һ�������UDP�Ƿ�����������.
 * ʹ�������ʱ,�������Ѿ��Լ�������tcp controller��.����಻�Ὠ��TCP����.ֻ���װUDP��,ֱ�ӷ�������.
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

#define _DNS_LIB_

#include "Utils.h"
#include "SocketEx.h"

/** @brief ͨ����������DNS, ʹ��ǰ��Ҫ�Լ�����tcp controller����,�Ӷ��õ�bound port
*/
class _DNS_LIB_ CProxySocketClient : public CSocketEx
{
public:
	/** @brief ͨ����������DNS, ʹ��ǰ��Ҫ�Լ�����tcp controller����,�Ӷ��õ�bound port
	* 
	* @param addr �����������IP��ַ
	* @param bound_port �Լ���������������tcp controller��õ���udp bound port
	*/
	CProxySocketClient( LPCTSTR proxyaddr, u_short proxy_port , LPCTSTR destaddr, u_short dest_port ) ;

	virtual BOOL ConnectTo(LPCTSTR lpszHost , int nPort);

	/** @brief ���ӵ����������
	*/
	virtual BOOL ConnectTo();
	/** @brief ����UDPת���Ļ�����
	*/
	virtual int _make_udpsend_packet_base(  char *real_buffer , int buf_len );
	/** @brief ����UDP���ݵ�������,������ͷ�Լ�ʵ�ʵ����ݰ�
	* 
	* @param Buffer ʵ��Ҫ���͵����ݰ�
	* @param Length ʵ��Ҫ���͵����ݰ�����
	* @param real_udp_buffer_len ��װ�õ�UDP�����ܳ�
	* @return ��װ�õ�UDP���ݰ�
	*/
	virtual char *_MakeUDPProxyData( void *Buffer, int Length ,int &real_udp_buffer_len );
	virtual BOOL WriteBytes ( void * Buffer, int Length );
	virtual int ReadBytes (void * Buffer , int Length);
protected:
	// �����ַ
	TCHAR szPorxyAddress[PROXY_IP_ADDRESS_MAX_LENGTH];
	// ����BOUND PORT
	u_short m_Proxyport;
	// DNS IP
	TCHAR szDestAddress[PROXY_IP_ADDRESS_MAX_LENGTH];
	// DNS PORT
	u_short m_Destport;

protected:
	SOCKADDR_IN m_addrDestination; // Ŀ�ĵ�ַ( DNS�ĵ�ַ:53)
};