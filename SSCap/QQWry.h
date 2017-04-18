#ifndef IPINFO_HEADER_FILE
#define IPINFO_HEADER_FILE

#pragma once

/* @brief ��������IP��. ͨ��IP������ڵ�ַ
  ����IP��ٷ���ַ: http://www.cz88.net/
 
 ʹ�÷���:
 char	szAddr[100]={0};
 CQQWry NewIPInfo;

 NewIPInfo.OpenA("D:\\qqwry.dat";);
 NewIPInfo.QueryIPA("1.2.3.4",szAddr,sizeof(szAddr));
 NewIPInfo.Close();

*/
 
 
/*
QQWry.dat�ж���Сβ�ֽ���
 �ļ���
 ��¼��
 ������
*/

#define DEFAULT_QQWRY_FILENAME _T("qqwry.dat")

#pragma pack(push,1)
#define IPINFO_INDEX_SIZE	7	//һ�������Ĵ�С
#pragma pack(pop)
 
class CQQWry
{
public:
	CQQWry(void);
	~CQQWry();
 
	//�ڵ�ǰĿ¼����Ĭ�ϵļ�������,QQWry.dat,IPwry.dat,��
	BOOL OpenDefDat(void);
 
	BOOL OpenA(IN char*  pszDatPath);
	BOOL OpenW(IN WCHAR* pszDatPath);
 
	BOOL Close(void);
 
	BOOL QueryIPA(IN char* pszIP,OUT char* pszAddr,IN ULONG ulAddrBuffSize);
	BOOL QueryIPW(IN WCHAR* pszIP,OUT WCHAR* pszAddr,IN ULONG ulAddrBuffSize);
 
	BOOL QueryIPA(IN ULONG ulIP,OUT char* pszAddr,IN ULONG ulAddrBuffSize);
	BOOL QueryIPW(IN ULONG ulIP,OUT WCHAR* pszAddr,IN ULONG ulAddrBuffSize);
 
	#ifdef _UNICODE
		#define QueryIP	QueryIPW
	#else
		#define QueryIP	QueryIPA
	#endif
 
	//����ָ��IP���ڼ�¼��ƫ��
	ULONG FindIP(IN ULONG ulIP);
 
#if 0
	//���IP��Ч��
	BOOL CheckIP(IN char* pszIP);
 #endif

private:
	BOOL LoadFileData();
#if 0
	//Map���ݿ��ļ�
	BOOL Map(IN ULONG ulMapSize);
 
	//UnMap���ݿ��ļ�
	BOOL UnMap(void);
 #endif

	BOOL IsFileOpened()
	{
		return ( INVALID_HANDLE_VALUE != m_hDatFile );
	}
	BOOL IsDataLoaded()
	{
		return ( m_bDataLoaded );
	}
private:
	unsigned char *pFileBody;
	ULONG	m_ulFileSize;
	HANDLE	m_hDatFile;

	void*	m_pDatBase;
#if 0
	HANDLE	m_hDatMap;
 #endif

	void*	m_pIndex;				//ָ��������
	ULONG	m_ulFirstIndexOffset;	//��һ��������ƫ��
	ULONG	m_ulLastIndexOffset;	//���һ��������ƫ��
	ULONG	m_ulRecordNum;			//��¼����
	BOOL	m_bDataLoaded;		// �����Ѽ���
};
 
#endif