#pragma once

/*!
 * @file SocksServer.h
 * @date 2015/10/19 15:18
 *
 * @brief ���ص�socks 5 �������������
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
#include <map>
#include <mymutex.h>
#include "SocketBase.h"

using namespace std;
class CSocketBase;

// success
#define ERR_RET_OK 0
// �˿ڰ�ʧ��,(�ѱ�ռ��)
#define ERR_BIND_PORT 1
// ����SOCKETʧ��
#define ERR_CREATE_SOCKET 2
// ����ʧ��
#define ERR_LISTEN 3
// ����ACCEPT�߳�ʧ��
#define ERR_CREATE_ACCEPT_THREAD 4
// Ĭ������100���˿�
#define DEFAULT_SEARCH_AMOUNT 100

class CListener : public CSocketBase
{
public:
	CListener();
	virtual ~CListener();

protected:
	//u_short nListenPort;
	//bool shareOverLan;
	SOCKET hListenSocket;
	u_long uConnections;
	bool bSearchingPort; ///< �˿�ռ����, �Զ��������ö� ��
	int nLastError;
	int nSearchingPortAmount; ///< �˿���������.

public:
	LPCTSTR GetLastErrorString();

	/* @brief ȡ��������
	*/
	int GetLastError()
	{
		return nLastError;
	}

	void IncrementConnection();
	void DecrementConnection();
	/*
	bool IsShareOverLan()
	{
		return shareOverLan;
	}
	*/
	/** @brief ��õ�ǰ������
	*/
	int GetConnections()
	{
		return uConnections;
	}

	/*void SetListenPort( u_short p)
	{
		nListenPort = p;
	}*/
	/*u_short GetListenPort()
	{
		return nListenPort;
	}*/
	/*void SetListenShareOverLan( bool s )
	{
		shareOverLan = s;
	}*/

	SOCKET GetSocketHandle()
	{
		return hListenSocket;
	}
	BOOL IsListenerStarted()
	{
		return ( hListenSocket == INVALID_SOCKET ? FALSE: TRUE );
	}

public:
	BOOL Start();
	void Stop();
};
void DisconnectAllConnection( int nNodeId );