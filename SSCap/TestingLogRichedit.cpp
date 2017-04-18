#include "stdafx.h"
#include "AutoRichEditCtrl.h"
#include "Utils.h"
//#include "SocksProxyTesting.h"
#include "TestingLogRichedit.h"
//#include "proxy/socks_test.h"

CTestingLogRichedit::CTestingLogRichedit()
	:CAutoRichEditCtrl()
{
	m_bShowTimeInfo = FALSE;
}

CTestingLogRichedit::~CTestingLogRichedit()
{

}
/** @brief ��ʼ��������ʾLOG��RICHEDIT. ��Ҫ�����弰�м��ĳ�ʼ��
*/
BOOL CTestingLogRichedit::InitializeLogRichedit()
{
	Init();
	SetAutoURLDetect( TRUE );
	SetEventMask(ENM_MOUSEEVENTS);
	SetSel(0,0);

	return TRUE;
}
/** @brief ���ʹ������LOG����������� ���ֽڸ�ʽ
	*/
void CTestingLogRichedit::AppendTestingLogA( LPCSTR lpMsg , int msg_type )
{
	if( lpMsg ){
		wchar_t *u16 = lm_a2u( lpMsg );
		if( u16 ){
			AppendTestingLogW( CString( u16 ), msg_type );
			delete []u16;
		}
	}
}
/** @brief ����һ��������Ե���Ϣ. version utf8
*/
void CTestingLogRichedit::AppendTestingLogUTF8( LPCSTR lpMsg , int msg_type )
{
	if( lpMsg ){
		wchar_t *u16 = lm_u82u16( lpMsg );
		if( u16 )
		{
			AppendTestingLogW( CString( u16 ), msg_type );
			delete []u16;
		}
	}
}
/** @brief ����һ��������Ե���Ϣ.
*
* @param strMsg Ҫ�������Ϣ.
* @param msg_type ��Ϣ����
*	- TESTMSG_INFO 1 ��ͨ��Ϣ
*	- TESTMSG_AFFIRMATIVE 2 �϶�����Ϣ
*	TESTMSG_NEGATIVE 3 �񶨵���Ϣ
*/
void CTestingLogRichedit::AppendTestingLogW(CString strMsg, int msg_type )
{
	if( m_bShowTimeInfo )
	{
		TCHAR szTimeString[100] = {0};
		GetTimeString( 0,_T("[%M:%S]  "),szTimeString, 100 );
		AppendString( szTimeString, RGB( 0,0, 0 ) );
	}

	COLORREF clr;
	if( msg_type == TESTMSG_INFO )
		clr = RGB( 0,0, 0 );
	else if( msg_type == TESTMSG_AFFIRMATIVE )
		clr = RGB( 0, 200, 0 );
	else if( msg_type == TESTMSG_NEGATIVE )
		clr = RGB( 255, 0, 0 );
	else 
		clr = RGB( 0,0, 0 );

	AppendString( strMsg, clr );
	AppendString( _T("\r\n"), RGB( 0,0, 0 ) );
}
/** @biref �����������LOG��ʱ���Ƿ������ǰ��ʱ��
* ��ǰʱ������ĸ�ʽ��: ʱ:��:��
*/
void CTestingLogRichedit::SetShowTimeInfo( BOOL bShow )
{
	m_bShowTimeInfo = bShow;
}

/** @brief ���������ı���������
*/
void CTestingLogRichedit::CopyTextToClipboard()
{
	SetSel(0,-1);
	Copy();
}
/** @brief ��������ı�
*/
void CTestingLogRichedit::ClearText()
{
	SetWindowText(_T(""));
}