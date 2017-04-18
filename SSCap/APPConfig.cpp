#include "stdafx.h"
#include "SSCap.h"
#include "APPConfig.h"
#include <assert.h>
#include "Utils.h"

bool CAPPConfig::is_valid_config = false;				// �Ƿ������Ч����Ϣ
bool CAPPConfig::hidden_ip_port = false;				// ���ش����IP,PORT
bool CAPPConfig::always_enable_sysproxy = false;		// ��������ʱ��������ϵͳ����
wstring CAPPConfig::help_url = SSCAP_DEFAULT_WEBSITE_GITHUB;		// ����URL
wstring CAPPConfig::contactme_url = SSCAP_DEFAULT_WEBSITE_GITHUB;	// ��ϵ��URL
wstring CAPPConfig::feedback_url = SSCAP_DEFAULT_WEBSITE_GITHUB;	// ����URL
wstring CAPPConfig::softname = SSCAP_NAME;				// �������
bool CAPPConfig::disable_sysproxy_function = false;		// ����ϵͳ����ģ�鹦��
wstring CAPPConfig::strGoogleplus = _T("https://plus.google.com/+Sockscap64bit");	// Google+��ַ
wstring CAPPConfig::strTwitter = _T("https://twitter.com/sockscap64");		// twitter
wstring CAPPConfig::strOtherInfo = _T("tarolabs1@gmail.com");	// other infomation
wstring CAPPConfig::strWebsite = SSCAP_DEFAULT_WEBSITE_GITHUB;		// ��վ��ַ
bool CAPPConfig::disable_add_nodes = false; // ��ֹ��ӽڵ�
// // ��ȡ�°汾��URL��ַ. ��Ӧ�ĵ�ַ��һ��TXT�ļ�. ��ʽ�ǰ汾��|��������
wstring CAPPConfig::newversion_url = NEWVERSION_URL;
wstring CAPPConfig::newverion_download_url_1 = SSCAP_DEFAULT_WEBSITE_GITHUB_RELEASE;

CAPPConfig::CAPPConfig()
{

}

CAPPConfig::~CAPPConfig()
{

}

/** @brief ͨ�������ļ�������Ϣ.
*/
BOOL CAPPConfig::create(  LPCTSTR lpszDir )
{
	return TRUE;
}

/** @brief Config��Ϣ�Ƿ���Ч
*/
BOOL CAPPConfig::IsValidConfig()
{
	return is_valid_config;
}

/** @brief �Ƿ���Ҫ���ش����IP,PORT
*/
bool CAPPConfig::IsHiddenIPPort()
{
	return hidden_ip_port;
}

/** @brief ��ð���URL
*/
wstring &CAPPConfig::GetHelpUrl()
{
	return help_url;
}
	
/** @brief �����ϵ��URL 
*/
wstring &CAPPConfig::GtetContactmeUrl()
{
	return contactme_url;
}
	
/** @brief ��÷���URL
*/
wstring &CAPPConfig::GetFeedbackUrl()
{
	return feedback_url;
}

/** @brief ����������
*/
wstring &CAPPConfig::GetSoftName()
{
	return softname;
}
bool CAPPConfig::AlwaysEnableSysProxy()
{
	return always_enable_sysproxy;
}

/** @brief �Ƿ������ϵͳ����ģ�鹦��
	*/
bool CAPPConfig::IsDisableSysProxyFunction()
{
	return disable_sysproxy_function;
}