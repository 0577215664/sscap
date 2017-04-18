/*!
 * @date 2016/02/29 16:06
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
 * @note
*/
#include <string>
#include <json/json.h>
#include <json/value.h>

using namespace std;

class CAPPConfig
{
public:
	CAPPConfig();
	virtual ~CAPPConfig();

public:
	/** @brief ͨ�������ļ�������Ϣ.
	*/
	static BOOL create(  LPCTSTR lpszDir );

	/** @brief Config��Ϣ�Ƿ���Ч
	*/
	static BOOL IsValidConfig();

	/** @brief �Ƿ���Ҫ���ش����IP,PORT
	*/
	static bool IsHiddenIPPort();

	static bool AlwaysEnableSysProxy();
	/** @brief ��ð���URL
	*/
	static wstring &GetHelpUrl();
	
	/** @brief �����ϵ��URL 
	*/
	static wstring &GtetContactmeUrl();
	
	/** @brief ��÷���URL
	*/
	static wstring &GetFeedbackUrl();

	/** @brief ����������
	*/
	static wstring &GetSoftName();

	/** @brief �Ƿ������ϵͳ����ģ�鹦��
	*/
	static bool IsDisableSysProxyFunction();
	static wstring &GetGoogleplus(){
		return strGoogleplus;
	}
	static wstring &GetTwitter(){
		return strTwitter;
	}
	static wstring &GetOtherinfo(){
		return strOtherInfo;
	}
	static wstring &GetWebsite(){
		return strWebsite;
	}
	static bool IsDisableAddNodes(){
		return disable_add_nodes;
	}
	static wstring &GetNewVersionUrl(){
		return newversion_url;
	}
	static wstring &GetNewVersionDownloadUrl_1(){
		return newverion_download_url_1;
	}
protected:
	static bool is_valid_config;				// �Ƿ������Ч��������Ϣ
	static bool hidden_ip_port;				// ���ش����IP,PORT
	static bool always_enable_sysproxy;		// ��������ʱ��������ϵͳ����
	static wstring help_url;				// ����URL
	static wstring contactme_url;			// ��ϵ��URL
	static wstring feedback_url;			// ����URL
	static wstring softname;				// �������
	static bool disable_sysproxy_function; // ����ϵͳ����ģ�鹦��
	static wstring strGoogleplus;	// Google+��ַ
	static wstring strTwitter;		// twitter
	static wstring strOtherInfo;	// other infomation
	static wstring strWebsite;		// ��վ��ַ
	static bool disable_add_nodes;	// ��ֹ��ӽڵ�
	static wstring newversion_url; // ��ȡ�°汾��URL��ַ. ��Ӧ�ĵ�ַ��һ��TXT�ļ�. ��ʽ�ǰ汾��|��������
	static wstring newverion_download_url_1; // �°汾���ص�ַ1
	//static wstring newverion_download_url_2; // �°汾���ص�ַ2
};