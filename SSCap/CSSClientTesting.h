#pragma once

#include "SSClient.h"
#include "BaseDef.h"
#include "TestingLogRichedit.h"

class CSSClientTesting : public CSSCLient
{
public:
	CSSClientTesting( CTestingLogRichedit *pLog );
	virtual ~CSSClientTesting();

	/** @brief ����һ��SS�ڵ�
	*/
	BOOL TestSSNode( CSSNodeInfo *pNode ,BOOL bIsUdp);

protected:
	DWORD nLatency;
	DWORD nDataTransferLatency;
protected:
	BOOL TestSSNodeTcp( CSSNodeInfo *pNode 	);
	BOOL TestSSNodeUdp( CSSNodeInfo *pNode 	);
	/** @brief ���ӵ�ss ������
	* 
	* @param bUdp: ��udp ����
	* @param bNonBlocking: ������
	*/
	virtual BOOL ConnectToSSServer( BOOL bUdp = FALSE, BOOL bNonBlocking = TRUE ,SOCKADDR_IN *pSockAddrIn = NULL );
protected:
	CTestingLogRichedit *pLogCtrol;
};