#include "stdheader.h"
#include "proxynscache.h"
#include "Ini.h"
#include "mymutex.h"
#include "Utils.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

typedef map<string,_PROXY_NS_CACHE_> MAP_PROXY_NS_CACHE;
typedef map<string,_PROXY_NS_CACHE_>::iterator ITER_MAP_PROXY_NS_CACHE;

static MAP_PROXY_NS_CACHE _map_proxy_ns_cache;
static CMyMutex _map_proxy_ns_cahce_mutex;

#define PROXY_NS_CACHE_FILENAME _T("proxynscache.ini")
#define PROXY_NS_CACHE_SECTION _T("cache")
#define PROXY_NS_CACHE_DOMAIN _T("domain")
#define PROXY_NS_CACHE_IP _T("ip")
#define PROXY_NS_CACHE_RESOLVETIME _T("t")

static TCHAR szProxyNSCacheWorkingDir[MAX_PATH] = {0};

/** @brief ���ش���NS��¼�Ļ���
* 
* @param szWorkingDir ����Ŀ¼
*/
BOOL LoadProxyNSCache( TCHAR *szWorkingDir )
{
	if( szWorkingDir == NULL ) return FALSE;

	_tcscpy_s( szProxyNSCacheWorkingDir, MAX_PATH, szWorkingDir );

	TCHAR szConfigFile[MAX_PATH];
	_stprintf_s(szConfigFile ,MAX_PATH, _T("%s\\config\\%s"), szWorkingDir, PROXY_NS_CACHE_FILENAME );
	CIni ini( szConfigFile );

	int index = 0;
	while( 1 ) 
	{
		TCHAR szSection[MAX_PATH];
		_stprintf_s( szSection,MAX_PATH, _T("%s%d"),PROXY_NS_CACHE_SECTION,index );
		if( ini.IsSectionExist( szSection ) )
		{
			TCHAR szDomain[260] = {0};
			TCHAR szIP[260] = {0};
			time_t nResolveTime = 0;

			ini.GetString( szSection, PROXY_NS_CACHE_DOMAIN, szDomain, 260 );
			ini.GetString( szSection, PROXY_NS_CACHE_IP, szIP, 260 );
			nResolveTime = ini.GetUInt( szSection,PROXY_NS_CACHE_RESOLVETIME,PROXY_NS_CACHE_VALID_TIME );

			if( szDomain[0] != 0 && szIP[0] != 0 )
			{
				_PROXY_NS_CACHE_ cache;

				char *pAnsiDomain = lm_u2a( szDomain );
				char *pAnsiIP = lm_u2a( szIP );

				if( pAnsiDomain && pAnsiIP )
				{
					// �����¼������
					if( !IsProxyNSRecordExist( string( pAnsiDomain ) ) )
					{
						cache.szRealIPAddress = string(pAnsiIP );
						cache.lastResolveTime = nResolveTime;

						_map_proxy_ns_cahce_mutex.Lock();
						_map_proxy_ns_cache.insert( make_pair( pAnsiDomain, cache ) );
						_map_proxy_ns_cahce_mutex.UnLock();
					}
				}

				if( pAnsiIP )
					delete [] pAnsiIP;

				if( pAnsiDomain )
				delete [] pAnsiDomain;
			}
		}
		else break;

		index ++;
	}
	return TRUE;
}
/** @brief �������NS�����¼���浽�ļ�
*/
void SaveProxyNSCache()
{
	if( szProxyNSCacheWorkingDir[0] == 0 ) return;

	TCHAR szConfigFile[MAX_PATH];
	_stprintf_s(szConfigFile ,MAX_PATH, _T("%s\\config\\%s"), szProxyNSCacheWorkingDir, PROXY_NS_CACHE_FILENAME );
	CIni ini( szConfigFile );

	int index = 0;

	_map_proxy_ns_cahce_mutex.Lock();
	ITER_MAP_PROXY_NS_CACHE iter = _map_proxy_ns_cache.begin();

	for( iter; iter != _map_proxy_ns_cache.end(); index ++,iter ++ )
	{
		if( !iter->first.empty() && !iter->second.szRealIPAddress.empty() )
		{
			wchar_t *pWDomain = lm_a2u( iter->first.c_str() );
			wchar_t *pWIP = lm_a2u( iter->second.szRealIPAddress.c_str() );

			TCHAR szSection[MAX_PATH];
			_stprintf_s( szSection,MAX_PATH, _T("%s%d"),PROXY_NS_CACHE_SECTION,index );

			if( pWDomain && pWIP )
			{
				ini.WriteString( szSection, PROXY_NS_CACHE_DOMAIN, pWDomain );
				ini.WriteString( szSection, PROXY_NS_CACHE_IP , pWIP );
				ini.WriteUInt( szSection,  PROXY_NS_CACHE_RESOLVETIME, (unsigned int)iter->second.lastResolveTime );
			}

			if( pWIP )
				delete []pWIP;

			if( pWDomain )
				delete []pWDomain;
		}
	}
	_map_proxy_ns_cahce_mutex.UnLock();

	return;
}

/** @brief ����ĳ��������NS��¼�����Ƿ����
*
* @param domain Ҫ���ҵ�����
* @return 
* - TRUE: ����
* - FALSE: ������
*/
BOOL IsProxyNSRecordExist( string domain )
{
	CMutexParser parser(&_map_proxy_ns_cahce_mutex);

	ITER_MAP_PROXY_NS_CACHE iter = _map_proxy_ns_cache.find( domain );
	if( iter != _map_proxy_ns_cache.end() )
		return TRUE;

	return FALSE;
}
/** @brief ���һ��������IP��ַ.
*
* @param[in] domain ����
* @param[out] ip ������IP
* @param[out] lastResolveTime ������������IP��ʱ��. ���ڵ�IP�������½���.( �����߸��� )
* @return 
* - TRUE: �ɹ�,  ip�з����˻�õ�IP
* - FALSE: ʧ��, �����ǲ�����, Ҳ�п�����֮ǰ�ļ�¼������, ��Ҫ�Լ������½���һ��.
*/
BOOL GetProxyNSIPForDomain( string domain, string &ip )
{
	CMutexParser parser(&_map_proxy_ns_cahce_mutex);

	ITER_MAP_PROXY_NS_CACHE iter = _map_proxy_ns_cache.find( domain );
	if( iter != _map_proxy_ns_cache.end() )
	{
		time_t now = time( NULL );
		if( GetTimeInterval( now, iter->second.lastResolveTime ) < PROXY_NS_CACHE_VALID_TIME )
		{
			ip = iter->second.szRealIPAddress;
			return TRUE;
		}
	}

	return FALSE;
}
/** @brief ��һ�������NS��¼ѹ�뻺��
* ������ھͻ��滻. �����ھ�ֱ�Ӳ���.
*/
void PushProxyNSCache( string domain , string ip )
{
	CMutexParser parser(&_map_proxy_ns_cahce_mutex);

	ITER_MAP_PROXY_NS_CACHE iter = _map_proxy_ns_cache.find( domain );

	// �ҵ��Ļ�,�ͽ����ļ�¼�滻��, ��������.
	if( iter != _map_proxy_ns_cache.end() )
	{
		iter->second.lastResolveTime = time( NULL );
		iter->second.szRealIPAddress = ip;
	}
	// û���ҵ�, �ͽ��¼�¼�����ȥ.
	else
	{
		_PROXY_NS_CACHE_ cache;
		cache.lastResolveTime = time( NULL );
		cache.szRealIPAddress = ip;

		_map_proxy_ns_cache.insert( make_pair( domain, cache ) );
	}
	return;
}