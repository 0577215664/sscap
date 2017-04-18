#pragma once
#include "afxcmn.h"
#include "TestingLogRichedit.h"
#include "mymutex.h"
#include "BaseDef.h"
#include <list>

using namespace std;

class CTestingCurrentNodeDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CTestingCurrentNodeDlg)

public:
	/** @brief ��ʼ���Ե�ǰ����
	*/
	BOOL StartTestCurrentProxy( BOOL bIsUdp = FALSE );

public:
	CTestingCurrentNodeDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CTestingCurrentNodeDlg();

// Dialog Data
	enum { IDD = IDD_DIALOG_TESTING_CURRENT_SERVER };
	void PushSSNodeInfo( CSSNodeInfo *pNode )
	{
		if( !pNode ) return;

		pNodeForTesting.push_back( pNode );
	}
	void ClearSSTestingNodes()
	{
		pNodeForTesting.clear();
	}
	list<CSSNodeInfo *> &GetNodeForTesting()
	{
		return pNodeForTesting;
	}
	BOOL GetIsTestUdp()
	{
		return bIsTestingUdp;
	}
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
protected:
	list<CSSNodeInfo *> pNodeForTesting;

	CTestingLogRichedit m_richTestingLog;
	HANDLE hTestingThread; ///< �����߳̾��
	CMyMutex ThreadMutex; ///< ���ڴ�������̵߳ľ��.
	BOOL bIsTestingUdp;
	/** @brief ���ڲ��Դ�����߳� 
	* 
	* @param lpParam CSocksProxyTesting�����ָ
	*/
	static DWORD WINAPI Thread_TestProxy( LPVOID lpParam );
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual BOOL OnInitDialog();
	virtual void OnOK();

	/** @brief �õ�������ʾLOG��richedit�Ŀؼ�ָ��
	*/
	CTestingLogRichedit *GetRichEditLogCtrl();
	afx_msg void OnEnSetfocusRicheditCurrentProxyLog();
};
