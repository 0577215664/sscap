#pragma once

/** @brief ����ϵͳ���� (IE����)
*
* @param lpszProxyServer �����������ַ, ΪNULL��ʾ������
* @param lpszPacUrl pac��ַ,ΪNULL��ʾ������
* @param lpszByPass ����By pass( ����������������б� ), ΪNULL��ʱ������
* @param bLocalAddrNotUseProxy ��Ӧ��IE���������е�: ���ڱ��ص�ַ��ʹ�ô��������
*
* @note ���ָ����lpszByPass, bLocalAddrNotUseProxy��������
*/
BOOL SetSystemProxy( 
	LPCTSTR lpszProxyServer, 
	LPCTSTR lpszPacUrl ,
	LPCTSTR lpszByPass = NULL,
	BOOL bLocalAddrNotUseProxy = FALSE );

/** @brief ����ϵͳ����
*/
BOOL DisableSystemProxy();

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
	);

/** @brief �Ƿ�������ϵͳ����
*/
BOOL IsSetSystemProxy();