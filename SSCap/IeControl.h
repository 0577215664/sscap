#pragma once


/** \brief
* IE�ؼ��࣬���ڴ�IE���ڣ���������һ��URL
* ����ѡ���½����ڴ򿪣��������������д��ڴ�
*
* \author : Taro
*/

#define BASICLIB_API

class BASICLIB_API CURL
{
public:
	CURL(){}
	~CURL(){}
private:
	// The default browser
	static CString m_strBrowser;
//	static HANDLE m_hIeInstance;
	static LPCTSTR GetBrowser(void);
public:
	// ��ȡ���һ��򿪵�IE���ڵľ��
//	static HANDLE GetInstance();
	// ��һ��IE���ڣ�����ѡ�����´��ڴ򿪣�����������IE���ڴ�
	static void Open(LPCTSTR lpszURL, bool bNewWindow = true);
	// �� CreateProcess ��ʽ��
	static HANDLE Open_Process(LPCTSTR lpszURL);
};
