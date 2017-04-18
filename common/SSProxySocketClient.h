#pragma once

#include "ProxySocketClient.h"
#include "Socks/Encypter.h"
class CSSProxySocketClient : public CProxySocketClient
{
public:
	CSSProxySocketClient( LPCTSTR proxyaddr, u_short proxy_port , LPCTSTR destaddr, u_short dest_port ,  CCryptor *pCryptor) ;

	/** @brief ����UDPת���Ļ�����
	* ssЭ��� SOCKSЭ����Ƚ�, ����Ҫǰ�ߵ�3���ֽڵĽھͿ�����
	*/
	virtual int _make_udpsend_packet_base(  char *real_buffer , int buf_len );
	/** @brief ����UDP���ݵ�������,������ͷ�Լ�ʵ�ʵ����ݰ�
	* 
	* @param Buffer ʵ��Ҫ���͵����ݰ�
	* @param Length ʵ��Ҫ���͵����ݰ�����
	* @param real_udp_buffer_len ��װ�õ�UDP�����ܳ�
	* @return ��װ�õ�UDP���ݰ�
	*/
	//virtual char *_MakeUDPProxyData( void *Buffer, int Length ,int &real_udp_buffer_len );
	virtual BOOL WriteBytes ( void * Buffer, int Length );
	virtual int ReadBytes (void * Buffer , int Length);
protected:
	CCryptor *m_pCryptor;
};