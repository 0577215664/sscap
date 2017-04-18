#include "stdheader.h"
#include "SocketBase.h"
#include "SocksClient.h"
#include "SSClient.h"
#include "Listener.h"
#include "SSManager.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CSSManager::CSSManager()
{

}

CSSManager::~CSSManager()
{

}

/** @brief ��ʼ����
*/
BOOL CSSManager::StartServices( )
{
	//CSSConfigInfo *pCfg = GetConfigInfo();
	//if( !pCfg ) return FALSE;

	//SetListenPort( pCfg->localPort );
	//SetListenShareOverLan( pCfg->shareOverLan );

	BOOL bRet = Start();

	//u_short realPort = GetListenPort();

	// �����еĶ˿ں�ʵ�ʶ˿ڲ�ͬ��,˵�����õĶ˿ڱ�ʹ����. �����������˿��ö˿�
	/*
	if( bRet && pCfg->localPort != realPort )
	{
		pCfg->localPort = realPort;
		SaveShadowsocksServer();
	}
	*/

	return bRet;
}

/** @brief ֹͣ����
*/
void CSSManager::StopServices()
{
	if( IsListenerStarted() )
		Stop();

	return ;
}