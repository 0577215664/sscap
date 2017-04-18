#pragma once

#include <string>
#include <vector>
#include "mymutex.h"
#include "SpeedAnalytics.h"

using namespace std;

#define DEFAULT_SOCKS_PORT 1080
#define DEFAULT_RECONNECT_TIMES 3
// Ĭ�ϵı���PAC��ַ.
//#define DEFAULT_LOCAL_PAC_HSOT "http://127.0.0.1:%d"
#ifdef USE_LIBPRIVOXY
#define DEFAULT_LOCAL_PAC_URL "echo-pac"
#else 
#define DEFAULT_LOCAL_PAC_URL "sscap_pac"
#endif
#define DEFAULT_LOCAL_PAC_FILENAME _T("/config/pac.txt")
//#define DEFAULT_LOCAL_USERPAC_FILENAME _T("/config/userpac.txt")
#define DEFAULT_TESTING_URL "http://global.bing.com"
#define LOCAL_LOOPBACK_IP_W _T("127.0.0.1")
#define LOCAL_LOOPBACK_IP "127.0.0.1"

#define CHARGETYPE_LOCAL			0	// local
#define CHARGETYPE_ONLINE_ECON		1	// online Economic 
#define CHARGETYPE_ONLINE_PRO		2	// online pro
#define CHARGETYPE_ONLINE_VIP		3

class CSSNodeInfo : public CSpeedAnalytics
{
public:
	CSSNodeInfo()
	{
		id = 0;
		server_port = 0;
		enable = true;
		Latency = 0;
		//Speed = 0;
		Upload_traffic = 0;
		Download_traffic = 0;
		ConnectedTimes = 0;
		FailureTimes = 0;
		Failure_rate = 0;
		nConnections = 0;
		lastStatus = -1 ;
		charge_type = CHARGETYPE_LOCAL;
	}
	CSSNodeInfo( int nId )
	{
		id = nId;
		server_port = 0;
		enable = true;
		Latency = 0;
		//Speed = 0;
		Upload_traffic = 0;
		Download_traffic = 0;
		ConnectedTimes = 0;
		FailureTimes = 0;
		Failure_rate = 0;
		nConnections = 0;
		lastStatus = -1;
		charge_type = CHARGETYPE_LOCAL;
	}

	CSSNodeInfo( const CSSNodeInfo &node )
	{
		id = node.id;
		server = node.server;
		server_port = node.server_port;
		password = node.password;
		method = node.method;
		remarks = node.remarks;
		enable = node.enable;

		Latency = node.Latency;
		//Speed = node.Speed;
		Upload_traffic = node.Upload_traffic;
		Download_traffic = node.Download_traffic;
		ConnectedTimes = node.ConnectedTimes;
		FailureTimes = node.FailureTimes;
		Failure_rate = node.Failure_rate;

		nConnections = node.nConnections;
		lastStatus = node.lastStatus;

		charge_type = node.charge_type;
	}

	void operator = ( CSSNodeInfo &node )
	{
		id = node.id;
		server = node.server;
		server_port = node.server_port;
		password = node.password;
		method = node.method;
		remarks = node.remarks;
		enable = node.enable;

		Latency = node.Latency;
		//Speed = node.Speed;
		Upload_traffic = node.Upload_traffic;
		Download_traffic = node.Download_traffic;
		ConnectedTimes = node.ConnectedTimes;
		FailureTimes = node.FailureTimes;
		Failure_rate = node.Failure_rate;

		nConnections = node.nConnections;
		lastStatus = node.lastStatus;

		charge_type = node.charge_type;
	}
public:
	//////////////////////////////////////////////////////////////////////////
	/** @brief ���½ڵ�ͳ����Ϣ
	*/
	void UpdateLatency( unsigned int Latency /** ms */);
	//void UpdateSpeed( unsigned int Speed /* bytes per sec */ );
	void UpdateUpload_traffic(unsigned int Upload_traffic /* bytes */);
	void UpdateDownload_traffic(unsigned int Download_traffic /* bytes */);
	void UpdateConnectedTimes( unsigned int ConnectedTimes /* ÿ�����Ӿ͵���*/);
	void UpdateFailureTimes( unsigned int FailureTimes /* ÿ��ʧ�ܾ͵���*/);
	void UpdateConnections( unsigned int type /* 1: ��һ������, 2: ��һ������*/);
	void SetLastStatus( int status );

	BOOL IsOnlineNode();
	//////////////////////////////////////////////////////////////////////////
	string ToSSlink();
	string ToPlainNodeInfo();
	string ToJson();
public:
	unsigned int id;	// �ڵ�ID. id���ظ�.����µĽڵ�, ID�����һ��ID˳������ȥ.
	string server;
	u_short server_port;
	string password;
	string method;
	string remarks;
	bool enable;
	string iplocation;		// IP����λ��.

public:
	// ss�ڵ�Ķ�̬������Ϣ
	unsigned int Latency;  // ms
	//unsigned int Speed;		// kb/s
	double Upload_traffic;	// bytes
	double Download_traffic;	// bytes
	unsigned int ConnectedTimes;	
	unsigned int FailureTimes;
	unsigned int Failure_rate; // %

	// ������
	unsigned int nConnections;

	int lastStatus; /// �����Ե�״̬, 0: ������, 1: ����, -1: δ֪

	int charge_type; // ��������

	CMyMutex mutexNode;
};

class CSSConfigInfo 
{
public:
	CSSConfigInfo()
	{
		runAtStartup = false;
		startInSystray = false;
		idInUse = 0;
		random = false;
		global = false;
		enable = false;
		shareOverLan = false;
		auto_disconnect_connection = false;
		localPort = DEFAULT_SOCKS_PORT;
		useOnlinePac = false;
		reconnectTimes = DEFAULT_RECONNECT_TIMES;
		//randomAlgorithm = 2; // ���
		autoban = false;
		nNextNodeId = 0;
		localPacFileFullName = wstring(_T(".")) + wstring( DEFAULT_LOCAL_PAC_FILENAME );
		//localUserPacFileFullName = wstring(_T(".")) + wstring( DEFAULT_LOCAL_USERPAC_FILENAME );
		isPrivoxyRunned = false;
		testing_url = string( DEFAULT_TESTING_URL );
		m_HotKeyForAddFromQRCode = 0;
		bLoadSaveNodes = true;
	}
	~CSSConfigInfo()
	{
		int i = 0;
		for( i = 0 ; i < (int)ssNodeInfo.size(); i ++ )
		{
			CSSNodeInfo *pNode = ssNodeInfo[i];

			if( pNode )
				delete pNode;
		}
		ssNodeInfo.clear();

		for( i = 0 ; i <(int) ssDeleteNodeInfo.size(); i ++ )
		{
			CSSNodeInfo *pNode = ssDeleteNodeInfo[i];

			if( pNode )
				delete pNode;
		}
		ssDeleteNodeInfo.clear();
	}
public:
	int GetActiveProxyId(){
		return idInUse;
	}
	CSSNodeInfo *GetActiveNodeInfo()
	{
		CMutexParser p( &mutex );

		if( ssNodeInfo.size() <= 0 ) 
			return NULL;

		for( int i = 0 ; i < (int)ssNodeInfo.size(); i ++ )
		{
			if( ssNodeInfo[i]->id == idInUse )
				return ssNodeInfo[i];
		}

		return NULL;
	}
	void ActiveSSNode( int idx )
	{
		CMutexParser p( &mutex );

		if( ssNodeInfo.size() <= 0 ) 
			return;

		for( int i = 0 ; i < (int)ssNodeInfo.size(); i ++ )
		{
			// �ҵ����ID,��������Ϊʹ��.
			if( ssNodeInfo[i]->id == idx )
			{
				idInUse = idx;
				break;
			}
		}
	}
	/** @biref ��˳��ȡ��i���ڵ�
	*/
	CSSNodeInfo *GetNodeByIndex( int i );

	/** @brief ����ĳ��idָ���Ľڵ�
	*/
	CSSNodeInfo *GetNodeById( int i );

	/** @brief �༭һ���ڵ�,����Ϊ�����޸�
	*/
	CSSNodeInfo *EditNode( int i ,string server,u_short server_port,string password,string method,string remarks,bool enable );
	/** @brief ����һ��SS�������ڵ�
	* �����²���ڵ��index
	*/
	CSSNodeInfo *AddNode( CSSNodeInfo *pNode );
	/** @brief ����һ��SS�������ڵ�
	* �����²���ڵ��index
	*/
	CSSNodeInfo *AddNode(int charge_type, string server,u_short server_port,string password,string method,string remarks,bool enable );
	/** @brief ͨ��ss link����һ��SS�������ڵ�
	* �����²���ڵ��index
	* ����-1����δ�ɹ�
	*
	* һ��SS LINK��ʽ��:
	* ss://YWVzLTI1Ni1jZmI6YXNkMTQ3QGxhbGFhaS1tb3V5dWVtb3VyaS5teWFsYXVkYS5jbjoxMDcwMA==
	* ��BASE64����, �������:
	* ss://aes-256-cfb:asd147@lalaai-mouyuemouri.myalauda.cn:10700
	* ��ʽΪ: ss://���ܷ�ʽ:����@������:�˿�
	*/
	CSSNodeInfo *AddNodeFromLink( string link );
	/** @brief ɾ��һ���ڵ�
	*/
	BOOL DeleteNode( int i );
	/** @brief ɾ�����нڵ� 
	* @param bExcludeOnline: �Ƿ��ų�����
	*/
	void DeleteAllNodes( BOOL bExcludeLocal = TRUE );
		
	/** @brief ����/����һ��NODE
	*/
	void EnableNode( int i , bool bEnable );
	/** @brief ��ȡPAC ��URL 
	*/
	wstring GetPacUrl( );
	/** @brief ���ϵͳ����ȫ��ģʽ�Ĵ����ַ
	*/
	wstring GetSysGlobalProxyAddr();
	int GetNodeSize()
	{
		return (int)ssNodeInfo.size();
	}
	void ClearAllNodesTrafficData();
	/** @brief ����add from qrcode �Ŀ�ݼ�
	*/
	void SetHotKeyForAddQRCode( WORD wModifiers,WORD wVirtualKeyCode ,wstring hotkey );

	/** @brief ���add from qrcode �Ŀ�ݼ�
	*/
	BOOL GetHotKeyForAddQRCode( WORD &wModifiers,WORD &wVirtualKeyCode );
public:
	CMyMutex mutex;
	vector<CSSNodeInfo *> ssNodeInfo;			// ss�ڵ���Ϣ
	vector<CSSNodeInfo *> ssDeleteNodeInfo;		// ���û�ɾ���Ľڵ���Ϣ, �ڳ����˳�ʱ���ͷ�

	bool runAtStartup;	///< ��������
	bool startInSystray; ///< ��������С��ϵͳ����.
	int idInUse;  ///< ��ǰʹ���еĽڵ�,CSSNodeInfo��idֵ.
	bool random;  ///< �����ѡ�ڵ�( ���������� )
	bool global; ///< ϵͳ����Globalģʽ true: ȫ��ģʽ, false: pacģʽ
	bool enable; ///< ����ϵͳ����
	bool shareOverLan; ///< ��������������
	bool auto_disconnect_connection; ///< �л��ڵ�ʱ�Զ��Ͽ�����
	u_short localPort; ///< ����socks5����˿�
	string localSocksUser; ///< ����socks5�����ʺ�
	string localSocksPass; ///< ����socks5��������
	wstring pacUrl; ///< ����pac��url.
	bool useOnlinePac; ///< ʹ������pac
	int reconnectTimes; ///< ��������
	//int randomAlgorithm; ///< radmon = true ( ���������� ) ������µ��㷨.
	bool autoban; ///< �Զ���ֹ����ķ������ڵ�.
	wstring localPacFileFullName;  // ����pac�ļ�ȫ��
	//wstring localUserPacFileFullName;  // �����û�pac�ļ�ȫ��
	bool isPrivoxyRunned;	// privoxy�Ƿ��Ѿ�����,��δ��������ʹ��ϵͳ������صĹ���.
	string testing_url;	// ����URL 
	unsigned int m_HotKeyForAddFromQRCode;	///< add from qrcode�Ŀ�ݷ�ʽ
	wstring strHotKey;

	bool bLoadSaveNodes;	// �Ƿ���ػ��߱���ڵ���Ϣ

	wstring strExeMainWorkingDirectory; /// EXE����Ĺ���Ŀ¼
public:
	/** @brief ����idInUse�Ƿ���Ч. �ǲ��Ǹ���һ����Ч��ID��. ��������Զ���0
	*/
	void VerifyInUseId();
	/** @�����һ���ڵ��ID��
	*
	* @param bInrement ����
	*/
	unsigned int GetNextNodeId( BOOL bInrement = TRUE );

	unsigned int nNextNodeId; /// �ڵ�ID������. ÿ����һ���ͼ�1, �ڵ��ID���Ǵ����ֵȡ����. ĳ�ڵ㱻ɾ����,����ID���ᱻ�ٴ�ʹ��.
};

void InitializeWorkingDirectory( LPCTSTR lpszWorkingDir );
/** @brief ����ss����ڵ�
*/
BOOL LoadShadowsocksServer( );
void SaveShadowsocksServer( );

CSSConfigInfo *GetConfigInfo();
//vector<CSSNodeInfo> *GetSSNodeInfo();
/** @brief ��֤����socks������ʺ�����
*/
bool VerifyLocalSocksUserPass( string user, string pass );
/** @brief ��õ�ǰ��ss�������ڵ�, ���δ��������������Ļ�,���ǵ�ǰѡ�е�,��������˷�����
* ����,����Ҫ���ݹ���ѡȡ
*/
//CSSNodeInfo *GetCurrentSSServer();