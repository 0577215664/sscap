
// SSCapDlg.h : header file
//

#pragma once

#include "BaseDef.h"
#include "afxcmn.h"
#include "ToolTipEx.h"
#include "SSManager.h"
#include "afxwin.h"
#include "Label.h"
#include "BtnST.h"
#include "TrayIcon.h"
#include "SSListCtrl.h"
#include "TestingCurrentNodeDlg.h"
#include "LibQREncode/qrencode.h"

//class CSSListCtrl;
// CSSCapDlg dialog
class CCounterPageDlg;

class CSSCapDlg : public CDialogEx
{
// Construction
public:
	CSSCapDlg(CWnd* pParent = NULL);	// standard constructor
	~CSSCapDlg();

// Dialog Data
	enum { IDD = IDD_SSCAP_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	static BOOL bHasShowedBalloonTip;			///< �Ƿ��Ѿ���ʾ��Balloon Tips, ��ʾ���Ͳ�����ʾ��,һֱ��ʾ������
	//CSSConfigInfo ssNodesInfo;   ///< ss�ڵ���Ϣ
	UINT_PTR m_nMainWndTimer;
	CTrayIcon nTrayIcon;

	//////////////////////////////////////////////////////////////////////////
	// ��������ʱ��IE�ɵ�����, �˳�ʱ�ָ�
	BOOL bUseAutoDetect;
	BOOL bUseAutoConfigUrl;
	TCHAR lpAutoConfigUrl[1024];
	BOOL bUseProxyServer;
	TCHAR lpProxyServer[100];
	TCHAR lpByPass[500];
	//////////////////////////////////////////////////////////////////////////

	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

protected:
	CToolTipCtrlEx tooltip;
	CImageList m_ImageList;
	CSSManager ssManager;
	BOOL bMainWindowShow;						///< �Ƿ���ʾ������, Ĭ������ʾ��. ���ͨ����������������ʱ,Ĭ������.

public:
	void SetShowMainWindow( BOOL bShow )
	{
		bMainWindowShow = bShow;
	}
	/** @brief �Ƿ���ʾ������
	*/
	BOOL IsShowMainWindow()
	{
		return bMainWindowShow;
	}
	// ��ȡssManager����
	CSSManager *GetSSManagerObj();
	/** @brief ����socks 5�����Ƿ��Ѿ�����
	*/
	BOOL IsLocalSocks5ServiceStarted( );
	// ��������SOCKS����
	BOOL RestartLocalSocketService( BOOL bNoticeSearchPort = FALSE /** ������ʾ�����˿�,ֻ��������ʱ, �����ý������ǲ�������ʾ��*/);

	afx_msg void OnBnClickedButtonAdd();
	afx_msg void OnBnClickedButtonAddQRCode();
	afx_msg void OnBnClickedButtonEdit();
	afx_msg void OnBnClickedButtonDel();
	afx_msg void OnBnClickedButtonDelAll();
	afx_msg void OnBnClickedButtonAddBatch();
	afx_msg void OnBnClickedButtonAddLink();
	afx_msg void CopyToSSlink();
	afx_msg void CopyToPlainNodeInfo();
	afx_msg void CopyToJosn();
	afx_msg void CopyToQRCode();
	HBITMAP QRcode_encodeStrongToBitmap(const char *string, int version, QRecLevel level, QRencodeMode hint, int casesensitive);

	/** @brief ע���ȼ�
	*/
	BOOL RegisterHotKey( CString strHotKey,WORD wNewVirtualKeyCode = 0 , WORD wNewModifiers = 0 ,BOOL bSave = FALSE);
protected:
	BOOL InitializeDialog();
	BOOL InitializeSSNodeList();
	void InitializeButtons();
	/** @brief ����SS�ڵ�
	*
	* @param bIsUdp �Ƿ����UDP
	*/
	void CheckSSNode( BOOL bIsUdp );
	void PingSSNode( );

	/** @brief ��������SS�ڵ�
	*
	* @param bIsUdp �Ƿ����UDP
	*/
	void CheckAllSSNode( BOOL bIsUdp );
public:
	afx_msg void OnBnClickedButtonActive();
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	virtual BOOL DestroyWindow();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);
	virtual void OnCancel();
	virtual void OnOK();
	afx_msg LRESULT OnTrayNotification(WPARAM wp, LPARAM lp);
	afx_msg LRESULT OnShowMainWnd();
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	void EnableSelectSSNode( bool bEnable );
	/** @brief ע�Ὺ���Զ����е�ע�������
	*/
	void RegisterSysConfigAutoRun();
	void RefreshSSNodeListCountTip();
public:
	CListCtrl *GetSSListContainer()
	{
		return &m_listSSNodes;
	}

protected:
	void ChangeControlSize();
	BOOL InitWebAd();
	/** @brief ����tray icon��ʾ��Ϣ
	*/
	void _UpdateTrayIconMessage();
	void ShowQRCode();
	void OnDisconnectAllConnections();
	void OnClearAllTrafficData();
	/** @brief ��������PAC�ļ�
	*/
	BOOL UpdateOnlinePacFile();

protected:
	CCounterPageDlg *pCounterPage;
	CWinThread * pWebLoaderTread;
	CTestingCurrentNodeDlg *pTestingCurrentProxyDlg;
	BOOL m_bRegisterHotKey;
	// ǰһ��ʹ���е�item, ��Ҫ�����л�ʹ���е�ITEMʱ�����׵�ͼ����ʾ.
	//int m_nLastActiveItem;
	CSSListCtrl m_listSSNodes;
	CButtonST m_btnAdd;
	CButtonST m_btnAddByQRCode;
	CButtonST m_btnAddByBatch;
	CButtonST m_btnAddByLink;
	CButtonST m_btnActiveNode;
	CButtonST m_btnEditNode;
	CButtonST m_btnDelNode;
	CButtonST m_btnCheck;
	CLabel m_staLocalSocksPortTip;
	CLabel m_staHotKeyTips;

	CButtonST m_btnRefresh;
	CButtonST m_btnSetting;
	CButtonST m_btnMainPage;
	CButtonST m_btnAbout;
	CButtonST m_btnExit;
public:
	afx_msg void OnBnClickedButtonExit();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
public:
	afx_msg void OnDropFiles(HDROP hDropInfo);
	afx_msg void OnNMDblclkListSsnode(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMRClickListSsnode(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMClickListSsnode(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedButtonCheck();
	afx_msg void OnBnClickedButtonSetting();
	afx_msg void OnBnClickedButtonMainpage();
	afx_msg void OnBnClickedButtonAbout();
	afx_msg void OnBnClickedButtonRefresh();
	LRESULT OnFoundNewVersion(WPARAM wParam ,LPARAM lParam);
	afx_msg void OnWindowPosChanging(WINDOWPOS* lpwndpos);
	//CStatic m_staBottomRange;
	//CStatic m_staTopRange;
	afx_msg void OnWindowPosChanged(WINDOWPOS* lpwndpos);
	afx_msg void OnHotKey(UINT nHotKeyId, UINT nKey1, UINT nKey2);
};

CListCtrl* GetSSListContainer();
/** @brief ȡ���������������Ի����ָ��
*/
CSSCapDlg *GetMainDlg();