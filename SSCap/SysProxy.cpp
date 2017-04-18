#include "stdafx.h"
#include "SysProxy.h"
#include "Utils.h"
#include <WinInet.h>
#include <assert.h>
#include "Debug.h"
using namespace debug;

#pragma comment( lib,"wininet.lib")
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/** @brief ����ϵͳ���� (IE����)
*
* @param lpszProxyServer �����������ַ, ΪNULL��ʾ������
* @param lpszPacUrl pac��ַ,ΪNULL��ʾ������
* @param lpszByPass ����By pass( ����������������б� ), ΪNULL��ʱ������
* @param lpszPac pac��ַ,ֻ����bIsPacMode=TRUEʱ����Ч
*
* @note ���ָ����lpszByPass, bLocalAddrNotUseProxy��������
*/
BOOL SetSystemProxy( 
	LPCTSTR lpszProxyServer, 
	LPCTSTR lpszPacUrl ,
	LPCTSTR lpszByPass /*= NULL */,
	BOOL bLocalAddrNotUseProxy /* = TRUE */ )
{
	// To include server for FTP, HTTPS, and so on, use the string
	// (ftp=http://<ProxyServerName>:80; https=https://<ProxyServerName>:80) 
	INTERNET_PER_CONN_OPTION_LIST    List; 
	INTERNET_PER_CONN_OPTION         Option[5];  //  ���5��
	unsigned long                    nSize = sizeof(INTERNET_PER_CONN_OPTION_LIST); 
	int nPos = 0;

	PrintfW( LEVEL_INFO, _T("[SetSystemProxy]: Proxy Server: %s, PacUrl: %s") , lpszProxyServer?lpszProxyServer:_T("NULL"),lpszPacUrl?lpszPacUrl:_T("NULL"));

	// proxy server
	if( lpszProxyServer )
	{
		Option[nPos].dwOption = INTERNET_PER_CONN_PROXY_SERVER; 
		Option[nPos].Value.pszValue = (LPTSTR)lpszProxyServer; 
		nPos ++;
	}
	if( lpszPacUrl )
	{
		Option[nPos].dwOption = INTERNET_PER_CONN_AUTOCONFIG_URL; 
		Option[nPos].Value.pszValue = (LPTSTR)lpszPacUrl; 
		nPos ++;
	}

	 // ָ����������
	 // Sets or retrieves the connection type. The Value member will contain one or more of the following values:
	 Option[nPos].dwOption = INTERNET_PER_CONN_FLAGS; 
	 Option[nPos].Value.dwValue = PROXY_TYPE_DIRECT;
	 if( lpszProxyServer )
		 Option[nPos].Value.dwValue |= PROXY_TYPE_PROXY;
	 if( lpszPacUrl )
		 Option[nPos].Value.dwValue |= PROXY_TYPE_AUTO_PROXY_URL;
	 nPos++;

	 // ָ����ͨ������������ĵ�ַ
	 // Sets or retrieves a string containing the URLs that do not use the proxy server.
	 Option[nPos].dwOption = INTERNET_PER_CONN_PROXY_BYPASS; 
	 if( lpszByPass != NULL )
	 {
		 Option[nPos].Value.pszValue = (LPTSTR)lpszByPass; 
	 }
	 else
	 {
		 if( bLocalAddrNotUseProxy )
			 // local;<local> ���ֵ�ǲ�ע����֪��. IE��������"���ڱ��ص�ַ��ʹ�ô��������"�����������
			 Option[nPos].Value.pszValue = _T("local;<local>"); 
		 else 
			 Option[nPos].Value.pszValue = _T("local"); 
	 }
	nPos ++;

	 List.dwSize = nSize; 
	 List.pszConnection = NULL; 
	 List.dwOptionCount = nPos; 
	 List.dwOptionError = 0; 
	 List.pOptions = Option; 

	 if(!InternetSetOption(NULL, INTERNET_OPTION_PER_CONNECTION_OPTION, &List, nSize)) 
	{
		PrintfW( LEVEL_ERROR, _T("[SetSystemProxy]: Set internet options failed, Code: %d") , GetLastError() );
		return FALSE;
	 }
	 // To refresh the global proxy settings, you must call InternetSetOption with the INTERNET_OPTION_REFRESH option flag.
	 InternetSetOption(NULL, INTERNET_OPTION_SETTINGS_CHANGED, NULL, 0);
	 InternetSetOption(NULL, INTERNET_OPTION_REFRESH, NULL,NULL); 
	 //The connection settings for other instances of Internet Explorer. 

	 PrintfW( LEVEL_INFO, _T("[SetSystemProxy]: Set internet options done") );

	 return TRUE;
}

/** @brief �Ƿ�������ϵͳ����
*/
BOOL IsSetSystemProxy()
{
	TCHAR szPacUrl[1024]={0};
	TCHAR szProxy[1024]= {0};
	TCHAR szByPass[1024]={0};
	DWORD flags = 0;
	BOOL bUseAutoDetect;
	BOOL bUseAutoConfigUrl;
	BOOL bUseProxyServer;

	if( GetSystemProxyInfo(
		bUseAutoDetect,
		bUseAutoConfigUrl,
		szPacUrl,
		1024,
		bUseProxyServer,
		szProxy,
		1024,
		szByPass,
		1024 ) && ( szProxy[0] != 0 || szPacUrl[0] != 0 ) )
		return TRUE;

	return FALSE;
}

/** @brief ��ȡIE�Ĵ�������,( ϵͳ����)
* 
* @param bUseAutoDetect[out] �Զ������������
* @param bUseAutoConfigUrl[out] Bufferʹ���Զ����ýű�����
* @param lpAutoConfigUrl[out] �Զ����ýű�URL��ַ
* @param nAutoConfigUrlLe[out] �Զ����ýű�URL Buffer����
* @param bUseProxyServer[out] ʹ�ô����������ַ
* @param lpProxyServer[out] �����������ַ
* @param nProxyServerLen[out] �����������ַBUFFER ����
* @param lpByPass[out] By pass�ַ���
* @param nByPassLen[out] By pass�ַ���BUFFER����

* @return 
* TRUE �ɹ�, ����ֵ�����ϲ�����
* FALSE ʧ��.
*
* https://msdn.microsoft.com/en-us/library/aa385145.aspx
*/
BOOL GetSystemProxyInfo( 
	BOOL &bUseAutoDetect, // �Զ������������
	BOOL &bUseAutoConfigUrl, // ʹ���Զ����ýű�����
	LPTSTR lpAutoConfigUrl,	// �Զ����ýű�URL��ַ
	int nAutoConfigUrlLen,		// �Զ����ýű�URL Buffer����
	BOOL &bUseProxyServer,		// ʹ�ô����������ַ
	LPTSTR lpProxyServer,		// �����������ַ
	int nProxyServerLen,		// �����������ַBUFFER ����
	LPTSTR lpByPass,			// By pass�ַ���
	int nByPassLen				// By pass�ַ���BUFFER����
	)
{
	assert( lpAutoConfigUrl != NULL );
	assert( nAutoConfigUrlLen > 0 );

	assert( lpProxyServer != NULL );
	assert( nProxyServerLen > 0 );

	assert( lpByPass != NULL );
	assert( nByPassLen > 0 );

	const int optionCount = 5;
	INTERNET_PER_CONN_OPTION_LIST List;
	INTERNET_PER_CONN_OPTION Option[optionCount];
	unsigned long nSize = sizeof(INTERNET_PER_CONN_OPTION_LIST);

	 	Option[0].dwOption = INTERNET_PER_CONN_FLAGS;
	 	Option[1].dwOption = INTERNET_PER_CONN_PROXY_SERVER;
	 	Option[2].dwOption = INTERNET_PER_CONN_PROXY_BYPASS;
	 	Option[3].dwOption = INTERNET_PER_CONN_AUTOCONFIG_URL;
	 	Option[4].dwOption = INTERNET_PER_CONN_AUTODISCOVERY_FLAGS;
	//Option[0].dwOption = 1;
	//for (int i = 1; i < optionCount; i++) {
	//	Option[i].dwOption = i;
	//}
	List.dwSize = sizeof(INTERNET_PER_CONN_OPTION_LIST);
	List.pszConnection = NULL;
	List.dwOptionCount = optionCount;
	List.dwOptionError = 0;
	List.pOptions = Option;

	if(!InternetQueryOption(NULL, INTERNET_OPTION_PER_CONNECTION_OPTION, &List, &nSize)) {
		return FALSE;
	}


	// proxy server
	bUseProxyServer = FALSE;
	if( (Option[0].Value.dwValue & PROXY_TYPE_PROXY) == PROXY_TYPE_PROXY
		&& Option[1].Value.pszValue != NULL 
		) 
	{
		bUseProxyServer = TRUE;
		_tcsncpy_s( lpProxyServer, nProxyServerLen, Option[1].Value.pszValue, nProxyServerLen - 1 );
	}

	// autoconfig url
	bUseAutoConfigUrl = FALSE;
	if( (Option[0].Value.dwValue & PROXY_TYPE_AUTO_PROXY_URL) == PROXY_TYPE_AUTO_PROXY_URL
		&&  Option[3].Value.pszValue != NULL 
		) 
	{
		bUseAutoConfigUrl = TRUE;
		_tcsncpy_s( lpAutoConfigUrl, nAutoConfigUrlLen, Option[3].Value.pszValue, nAutoConfigUrlLen - 1 );
	}

	// auto detect
	bUseAutoDetect = FALSE;
	if( (Option[0].Value.dwValue & PROXY_TYPE_AUTO_DETECT) == PROXY_TYPE_AUTO_DETECT )
	{
		bUseAutoDetect = TRUE;
	}

	if( Option[2].Value.pszValue != NULL )
	{
		_tcsncpy_s( lpByPass, nByPassLen, Option[2].Value.pszValue, nByPassLen - 1 );
	}

	return TRUE;
}

/** @brief ����ϵͳ����
*/
BOOL DisableSystemProxy()
{
	INTERNET_PER_CONN_OPTION_LIST    List; 
	INTERNET_PER_CONN_OPTION         Option; 
	unsigned long                    nSize = sizeof(INTERNET_PER_CONN_OPTION_LIST); 

	PrintfW( LEVEL_INFO, _T("[DisableSystemProxy]: ...") );

	// Set flags.
	Option.dwOption = INTERNET_PER_CONN_FLAGS;
	Option.Value.dwValue = PROXY_TYPE_DIRECT  ;

	// Fill out list struct.
	List.dwSize = nSize;
	// NULL == LAN, otherwise connectoid name.
	List.pszConnection = NULL;
	// Set three options.
	List.dwOptionCount = 1;
	List.pOptions = &Option; 

	// Set the options on the connection.
	if( !InternetSetOption(NULL,
		INTERNET_OPTION_PER_CONNECTION_OPTION, &List, nSize) )
	{
		PrintfW( LEVEL_ERROR, _T("[DisableSystemProxy]: Set internet options failed, Code: %d") ,GetLastError() );
		return FALSE;
	}

	InternetSetOption(NULL, INTERNET_OPTION_SETTINGS_CHANGED, NULL, 0);
	InternetSetOption(NULL, INTERNET_OPTION_REFRESH , NULL, 0);

	PrintfW( LEVEL_INFO, _T("[DisableSystemProxy]: Set internet options done."));

	return TRUE;
}
