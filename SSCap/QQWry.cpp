#include "stdafx.h"
#include <WinSock2.h>
#include <stdio.h>
#include "QQWry.h"
#include "Debug.h" 
#include <malloc.h>

using namespace debug;

CQQWry::CQQWry()
{
	m_hDatFile	= INVALID_HANDLE_VALUE;
	m_pDatBase	= NULL;

#if 0
	m_hDatMap	= NULL;
#endif

	m_pIndex			 = NULL;	//ָ��������
	m_ulFirstIndexOffset =0;	//��һ��������ƫ��
	m_ulLastIndexOffset	 =0;	//���һ��������ƫ��
	m_ulRecordNum		 =0;	//��¼����
	m_bDataLoaded = FALSE;
	pFileBody = NULL;
}
 
CQQWry::~CQQWry()
{
	m_bDataLoaded = FALSE;
	if( pFileBody )
	{
		free( pFileBody );
		pFileBody = NULL;
	}
	Close();

	//UnMap();
}
 
//�ڵ�ǰĿ¼����Ĭ�ϵļ�������,QQWry.dat,IPwry.dat,��
BOOL CQQWry::OpenDefDat(void)
{
	BOOL bReturnFlag=FALSE;
	int	nIndex=0;
 
	char DefNameArray[][20] = 
	{
		"QQWry.dat",
		"CoralWry.dat",
		"MyIP.dat",
		"IPWry.dat",
		"IP.dat",
		"Coral.dat",
		"Wry.dat"
	};
 
	char	szSelfFolder[MAX_PATH]={0};
	char	szDatFile[MAX_PATH]={0};
 
 
	do 
	{
		GetModuleFileNameA(GetModuleHandle(NULL),szSelfFolder,MAX_PATH);
 
		*(strrchr(szSelfFolder,'\\')) = '\0';
 
		for (nIndex=0; nIndex < sizeof(DefNameArray)/sizeof(DefNameArray[0]); nIndex++)
		{
			RtlZeroMemory(szDatFile,sizeof(szDatFile));
 
			_snprintf(szDatFile,MAX_PATH-1,"%s\\%s",szSelfFolder,DefNameArray[nIndex]);
 
			if (OpenA(szDatFile))
			{
				bReturnFlag =TRUE;
				break;
			}
		}
 
	} while (FALSE);
 
	return bReturnFlag;
}
 
BOOL CQQWry::OpenA(IN char* pszDatPath)
{
 
	PrintfA( LEVEL_INFO,("[OpenA] %s"),pszDatPath );

	#if 0
		m_hDatFile = CreateFileA(pszDatPath,GENERIC_READ|GENERIC_WRITE,FILE_SHARE_READ|FILE_SHARE_WRITE,\
			NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
	#else
		m_hDatFile = CreateFileA(pszDatPath,GENERIC_READ,FILE_SHARE_READ,\
			NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
	#endif
 
	if (INVALID_HANDLE_VALUE == m_hDatFile)
	{
		PrintfA( LEVEL_INFO,("[OpenA] %s failed, code: %d"),pszDatPath ,GetLastError() );

		return FALSE;
	}
#if 0
	return this->Map(0);
#else 
	return this->LoadFileData();
#endif
	
}
 
BOOL CQQWry::OpenW(IN WCHAR* pszDatPath)
{
	PrintfW( LEVEL_INFO,_T("[OpenW] %s "),pszDatPath );

	#if 0
		m_hDatFile = CreateFileW(pszDatPath,GENERIC_READ|GENERIC_WRITE,FILE_SHARE_READ|FILE_SHARE_WRITE,\
			NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
	#else
		m_hDatFile = CreateFileW(pszDatPath,GENERIC_READ,FILE_SHARE_READ,\
			NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
	#endif
 
	if (INVALID_HANDLE_VALUE == m_hDatFile)
	{
		PrintfW( LEVEL_INFO,_T("[OpenW] %s failed, code: %d"),pszDatPath ,GetLastError() );

		return FALSE;
	}
 
#if 0
	return this->Map(0);
#else 
	return this->LoadFileData();
#endif
}
 
BOOL CQQWry::LoadFileData()
{
	if ( !IsFileOpened() )
	{
		PrintfW( LEVEL_INFO,_T("[LoadFileData] INVALID_HANDLE_VALUE") );
		return FALSE;
	}

	m_ulFileSize = GetFileSize(m_hDatFile,NULL);
	if (0 == m_ulFileSize)
	{
		PrintfW( LEVEL_INFO,_T("[LoadFileData] m_ulFileSize = 0") );
		return FALSE;
	}

	pFileBody = (unsigned char * )malloc( m_ulFileSize );
	if( !pFileBody ) return FALSE;
	memset( pFileBody, 0, m_ulFileSize );

	DWORD dwReaded = 0;

	if( !ReadFile( m_hDatFile, pFileBody, m_ulFileSize, &dwReaded, NULL ) )
	{
		PrintfW( LEVEL_INFO,_T("[LoadFileData] ReadFile failed. Code: %d"),GetLastError() );
		return FALSE;
	}

	m_pDatBase = pFileBody;

	//////////////////////////////////////////////////////////////////////////
	//��һ��������ƫ��
	m_ulFirstIndexOffset = *( (ULONG*)pFileBody );

	//���һ��������ƫ��
	m_ulLastIndexOffset =  *( (ULONG*) ( (BYTE*)pFileBody + sizeof(ULONG) ) );

	//ָ��������
	m_pIndex	= ((BYTE*)pFileBody + m_ulFirstIndexOffset);

	//��¼����
	m_ulRecordNum = 1 + (m_ulLastIndexOffset - m_ulFirstIndexOffset) / IPINFO_INDEX_SIZE;

	//////////////////////////////////////////////////////////////////////////

	PrintfW( LEVEL_INFO,_T("[Map] m_ulFirstIndexOffset: %u, m_ulLastIndexOffset: %u, m_pIndex: %d, m_ulRecordNum: %u, "),
		m_ulFirstIndexOffset,m_ulLastIndexOffset, m_pIndex, m_ulRecordNum );

	m_bDataLoaded = TRUE;

	return TRUE;
}
#if 0
BOOL CQQWry::Map(IN ULONG ulMapSize)
{
	BOOL bReturnFlag=FALSE;
 
	PrintfW( LEVEL_INFO,_T("[Map] ulMapSize: %u"),ulMapSize );

	do 
	{
		if ( !IsFileOpened() )
		{
			PrintfW( LEVEL_INFO,_T("[Map] ulMapSize: %u INVALID_HANDLE_VALUE"),ulMapSize );
			break;
		}
 
		m_ulFileSize = GetFileSize(m_hDatFile,NULL);
		if (0 == m_ulFileSize)
		{
			PrintfW( LEVEL_INFO,_T("[Map] ulMapSize: %u m_ulFileSize = 0"),ulMapSize );

			break;
		}
 
		#if 0
			m_hDatMap = CreateFileMapping(m_hDatFile,NULL,PAGE_READWRITE,0,ulMapSize,NULL);
			if(NULL == m_hDatMap)
			{
				break;
			}
 
			m_pDatBase = MapViewOfFile(m_hDatMap,FILE_MAP_READ|FILE_MAP_WRITE,0,0,0);
			if (NULL == m_pDatBase)
			{
				//XDebug((_T("map����? %s"),xGetLastErrorString() ));
				break;
			}
		#else
			PrintfW( LEVEL_INFO,_T("[Map] ulMapSize: %u CreateFileMapping"),ulMapSize );
			m_hDatMap = CreateFileMapping(m_hDatFile,NULL,PAGE_READONLY,0,ulMapSize,NULL);
			if(NULL == m_hDatMap)
			{
				PrintfW( LEVEL_INFO,_T("[Map] ulMapSize: %u CreateFileMapping failed, code: %d"),ulMapSize ,GetLastError() );

				break;
			}
 
			PrintfW( LEVEL_INFO,_T("[Map] ulMapSize: %u MapViewOfFile"),ulMapSize );

			m_pDatBase = MapViewOfFile(m_hDatMap,FILE_MAP_READ,0,0,0);
			if (NULL == m_pDatBase)
			{
				PrintfW( LEVEL_INFO,_T("[Map] ulMapSize: %u MapViewOfFile failed, code: %d"),ulMapSize,GetLastError() );

				//XDebug((_T("map����? %s"),xGetLastErrorString() ));
				break;
			}
		#endif
 
		//////////////////////////////////////////////////////////////////////////
		//��һ��������ƫ��
		m_ulFirstIndexOffset = *( (ULONG*)m_pDatBase );
 
		//���һ��������ƫ��
		m_ulLastIndexOffset =  *( (ULONG*) ( (BYTE*)m_pDatBase + sizeof(ULONG) ) );
 
		//ָ��������
		m_pIndex	= ((BYTE*)m_pDatBase + m_ulFirstIndexOffset);
 
		//��¼����
		m_ulRecordNum = 1 + (m_ulLastIndexOffset - m_ulFirstIndexOffset) / IPINFO_INDEX_SIZE;
 
		//////////////////////////////////////////////////////////////////////////
 
		PrintfW( LEVEL_INFO,_T("[Map] ulMapSize: %u m_ulFirstIndexOffset: %u, m_ulLastIndexOffset: %u, m_pIndex: %d, m_ulRecordNum: %u, "),
			ulMapSize,m_ulFirstIndexOffset,m_ulLastIndexOffset, m_pIndex, m_ulRecordNum );
 
		bReturnFlag =TRUE;

		m_bDataLoaded = TRUE;
	} while (FALSE);
 
	return bReturnFlag;
}
 
 
BOOL CQQWry::UnMap(void)
{
	BOOL	bReturnFlag=FALSE;
 
	do 
	{	
		if (NULL ==  m_pDatBase)
		{
			break;
		}
 
		if (!FlushViewOfFile(m_pDatBase,m_ulFileSize))
		{
			break;
		}
 
		if (!UnmapViewOfFile(m_pDatBase))
		{
			break;
		}
 
		if (NULL == m_hDatMap)
		{
			break;
		}
 
		if (!CloseHandle(m_hDatMap))
		{
			break;
		}
 
		bReturnFlag =TRUE;
	} while (FALSE);
 
	return bReturnFlag;
}
#endif

BOOL CQQWry::Close(void)
{
	BOOL	bReturnFlag=FALSE;
 
	do 
	{
		if (INVALID_HANDLE_VALUE == m_hDatFile)
		{
			break;
		}
 
		if (!CloseHandle(m_hDatFile))
		{
			break;
		}
 
		m_hDatFile = INVALID_HANDLE_VALUE;
		bReturnFlag =TRUE;
	} while (FALSE);
 
	return bReturnFlag;
}
 
BOOL CQQWry::QueryIPA(IN char* pszIP,OUT char* pszAddr,IN ULONG ulAddrBuffSize)
{
	BOOL	bReturnFlag=FALSE;
	ULONG	ulTemp=0;
	ULONG	ulNetByteOrderAddr=0;
 
	PrintfA( LEVEL_INFO,("[QueryIPA] %s"),pszIP );

	if( !IsFileOpened() || !IsDataLoaded() )
		return FALSE;

	do 
	{
		if ( (NULL==pszIP) || (0==strlen(pszIP)) || (NULL==pszAddr) || (0==ulAddrBuffSize) )
		{
			break;
		}
 
		ulTemp = inet_addr(pszIP);
		if (ulTemp == INADDR_NONE)
		{
			LPHOSTENT lphost;
			lphost = gethostbyname(pszIP);
			if (lphost != NULL)
			{
				ulTemp = ((LPIN_ADDR)lphost->h_addr)->s_addr;
			}
			else break;
		}

#if 0
		if (!CheckIP(pszIP))
		{
			//IP��ʽ����ȷ
			break;
		}
#endif
		PrintfW( LEVEL_INFO,_T("[QueryIPA] NetByteOrderAddr: %u"),ulNetByteOrderAddr );

		ulNetByteOrderAddr = htonl(ulTemp);
 
		bReturnFlag = this->QueryIPA(ulNetByteOrderAddr,pszAddr,ulAddrBuffSize);
 
	} while (FALSE);
 
	return bReturnFlag;
}
 
BOOL CQQWry::QueryIPA(IN ULONG ulIP,OUT char* pszAddr,IN ULONG ulAddrBuffSize)
{
	BOOL	bReturnFlag=FALSE;
	ULONG	ulRecordOffset=0;
	ULONG	ulSecondOffset=0;
	BYTE	bMode=0;
 
	ULONG	ulRedirect=0;
 
	char*	pszMajor=NULL;
	char*	pszMinor=NULL;
	BYTE*	pRecord=NULL;
 
	PrintfW( LEVEL_INFO,_T("[QueryIPA] uIP: %u"),ulIP );

	if( !IsFileOpened() || !IsDataLoaded() )
		return FALSE;

	do 
	{
		if ( (0==ulIP) || (NULL==pszAddr) || (0==ulAddrBuffSize) )
		{
			break;
		}
		//RtlZeroMemory(pszAddr,ulAddrBuffSize);
 
		ulRecordOffset =FindIP(ulIP);
		if( 0 == ulRecordOffset)
		{
			break;
		}
 
		pRecord = (BYTE*)m_pDatBase + ulRecordOffset;
		bMode = *( pRecord+ sizeof(ULONG) );
 
		if ( 0 == bMode)
		{
			//δ֪���Ҽ�¼
		}
		else if ( 1 == bMode)
		{
			//��������Ҫ�ض���,
			memcpy(&ulRedirect,pRecord + sizeof(ULONG) + sizeof(BYTE),3);
 
			//�жϹ������ض����Ĺ������Ƿ���Ҫ�ٴ��ض���,�ض������2��
			memcpy(&bMode,(BYTE*)m_pDatBase + ulRedirect,1);
 
			//��������������˵ڶ����ض�������ڶ����ض���һ��Ϊģʽ2
			if( 2 != bMode)
			{
				//����������Ҫ�����ض���,���Ǽ򵥵��ض���ģʽ1
				//�ض����Ĺ��Ҽ�¼192.168.1.1
				pszMajor = (char*)((BYTE*)m_pDatBase + ulRedirect);
 
				memcpy(&bMode,(BYTE*)m_pDatBase+ ulRedirect + strlen(pszMajor) +1,1);
				//�жϵ����Ƿ���Ҫ�ض���
				if ( 0 == bMode)
				{
					//δ֪����
					pszMinor =NULL;
				}
				else if ( (1==bMode) || (2==bMode) )
				{
					//������Ҫ�ض���1.2.3.4
					memcpy(&ulRedirect,(BYTE*)m_pDatBase+ ulRedirect + strlen(pszMajor) +1 + sizeof(BYTE),3);
					pszMinor = (char*)m_pDatBase + ulRedirect;
				}
				else
				{	
					pszMinor = (char*)( (BYTE*)m_pDatBase+ ulRedirect + strlen(pszMajor) +1);
				}
			}
			else
			{
				//��������Ҫ2���ض���,�ڶ����ض���֮��һ��Ϊģʽ2,ֻ�ᷢ���ڹ��Ҽ�¼��
				memcpy(&ulSecondOffset,(BYTE*)m_pDatBase + ulRedirect + sizeof(BYTE),3);
 
				//������
				pszMajor = (char*)((BYTE*)m_pDatBase + ulSecondOffset);
 
				//�жϵ����Ƿ���Ҫ�ض���
				memcpy(&bMode,(BYTE*)m_pDatBase + ulRedirect + sizeof(ULONG),1);
				if ( 0 == bMode)
				{
					//δ֪����
				}
				else if ( (1== bMode) || (2 ==bMode) )
				{
					//������Ҫ�ض��� 119.119.119.119
					memcpy(&ulSecondOffset,(BYTE*)m_pDatBase + ulRedirect + sizeof(ULONG) +1,3);
 
					//������
					pszMinor = (char*)((BYTE*)m_pDatBase + ulSecondOffset);
				}
				else
				{
					//��ͨ��,���ض���ģʽ2һ��,ֱ�Ӹ��ڹ���ƫ��֮��
					//12.34.56.78
					/*
					BYTE  bMode=2;	//һ���ֽڱ�ʾ���ض���ģʽ2
					BYTE  CountryOffset[3];	//3���ֽ�,��Ź������ľ���ƫ��
					char[������] �ĵ�����,��\0��β.
					*/
					pszMinor = (char*)((BYTE*)m_pDatBase + ulRedirect + sizeof(ULONG));
				}
			}
		}
		else if ( 2 == bMode)			//������Ҫ�ض���,��������Ҫ,���Һ͵���������һ���
		{
			/*
			�ض���ģʽ2		202.115.128.13
 
			ULONG ulEndIP;
			BYTE  bMode=2;	//һ���ֽڱ�ʾ���ض���ģʽ2
			BYTE  CountryOffset[3];	//3���ֽ�,��Ź������ľ���ƫ��
			char[������] �ĵ�����,��\0��β.
			*/
 
			memcpy(&ulRedirect,pRecord + sizeof(ULONG) + sizeof(BYTE),3);			
 
			//��������Ҫ�ض���
			pszMajor = (char*)((BYTE*)m_pDatBase + ulRedirect);
 
			//�жϵ����Ƿ���Ҫ�ض���
			memcpy(&bMode,(BYTE*)pRecord+ sizeof(ULONG) + sizeof(ULONG),1);
			if ( 0 == bMode)
			{
				//δ֪����
				pszMinor =NULL;
			}
			else if ( (1==bMode) || (2==bMode) )
			{
				//������Ҫ�ض���1.2.3.4
				memcpy(&ulRedirect,(BYTE*)pRecord+ sizeof(ULONG) + sizeof(ULONG) +sizeof(BYTE),3);
				pszMinor = (char*)m_pDatBase + ulRedirect;
			}
			else
			{	
				//������û���ض���
				pszMinor = (char*)( pRecord+ sizeof(ULONG) + sizeof(ULONG));
			}
 
		}
		else
		{
			//��򵥵�����,���Ҳ���Ҫ�ض���
			pszMajor = (char*)pRecord+ sizeof(ULONG);
 
			//�жϵ����Ƿ���Ҫ�ض���
			memcpy(&bMode,(BYTE*)pRecord + sizeof(ULONG) + strlen(pszMajor) +1,1);
			if ( 0 == bMode)
			{
				//δ֪����
				pszMinor =NULL;
			}
			else if ( (1==bMode) || (2==bMode) )
			{
				//������Ҫ�ض���1.2.3.4
				memcpy(&ulRedirect,(BYTE*)pRecord + sizeof(ULONG) + strlen(pszMajor) +1 +sizeof(BYTE),3);
				pszMinor = (char*)m_pDatBase + ulRedirect;
			}
			else
			{	
				/*
				ULONG ulEndIP;	//4���ֽڵĽ���IP
				char[������] �Ĺ�����.������\0��β
				char[������] �ĵ�����,������\0��β.
				*/
				pszMinor = (char*)pRecord + sizeof(ULONG) + strlen(pszMajor) +1;
			}
		}
 
		//�ۺϹ������͵�����
		if ( NULL== pszMajor )
		{
			//strcpy(pszAddr,"δ֪����");
			break;
		}
 
		RtlZeroMemory(pszAddr,ulAddrBuffSize);

		_snprintf_s( pszAddr, ulAddrBuffSize, ulAddrBuffSize-1 , "%s%s%s",pszMajor,pszMinor?" ":"", pszMinor?pszMinor:"");
 
		PrintfA( LEVEL_INFO,("[QueryIPA] uIP: %u, Location: %s"),ulIP,pszAddr );
		
		bReturnFlag =TRUE;
	} while (FALSE);
 
	return bReturnFlag;
}
 
BOOL CQQWry::QueryIPW(IN WCHAR* pszWideIP,OUT WCHAR* pszWideAddr,IN ULONG ulWideBuffSize)
{
	char	szAnsiIP[40]={0};
	char*	pszAnsiAddr=NULL;
	ULONG	ulAnsiBuffSize=0;
	BOOL	bFlag=FALSE;
 
	do 
	{
		if ( (NULL==pszWideIP) || (NULL==pszWideAddr) || (0==ulWideBuffSize) )
		{
			break;
		}
 
		if ( 0 == WideCharToMultiByte(CP_ACP,0,pszWideIP,-1,szAnsiIP,sizeof(szAnsiIP),0,0))
		{
			break;
		}
 
		ulAnsiBuffSize = ulWideBuffSize * sizeof(WCHAR);
		pszAnsiAddr = new char[ulAnsiBuffSize];
		if (NULL == pszAnsiAddr)
		{
			break;
		}
		RtlZeroMemory(pszAnsiAddr,ulAnsiBuffSize);
 
		if (!QueryIPA(szAnsiIP,pszAnsiAddr,ulAnsiBuffSize))
		{
			break;
		}
 
		if ( 0 == MultiByteToWideChar(CP_ACP,MB_ERR_INVALID_CHARS,pszAnsiAddr,-1,pszWideAddr,ulWideBuffSize))
		{
			break;
		}
 
		bFlag =TRUE;
	} while (FALSE);
 
	if (NULL != pszAnsiAddr)
	{
		delete[] pszAnsiAddr;
		pszAnsiAddr = NULL;
	}
 
	return bFlag;
}
 
BOOL CQQWry::QueryIPW(IN ULONG ulIP,OUT WCHAR* pszWideAddr,IN ULONG ulWideBuffSize)
{
	char*	pszAnsiAddr=NULL;
	ULONG	ulAnsiBuffSize=0;
	BOOL	bFlag=FALSE;
 
	do 
	{
		if ( (0==ulIP) || (NULL==pszWideAddr) || (0==ulWideBuffSize) )
		{
			break;
		}
 
		ulAnsiBuffSize = ulWideBuffSize * sizeof(WCHAR);
		pszAnsiAddr = new char[ulAnsiBuffSize];
		if (NULL == pszAnsiAddr)
		{
			break;
		}
		RtlZeroMemory(pszAnsiAddr,ulAnsiBuffSize);
 
		if (!QueryIPA(ulIP,pszAnsiAddr,ulAnsiBuffSize))
		{
			break;
		}
 
		if ( 0 == MultiByteToWideChar(CP_ACP,MB_ERR_INVALID_CHARS,pszAnsiAddr,-1,pszWideAddr,ulWideBuffSize))
		{
			break;
		}
 
		bFlag =TRUE;
	} while (FALSE);
 
	if (NULL != pszAnsiAddr)
	{
		delete[] pszAnsiAddr;
		pszAnsiAddr = NULL;
	}
 
	return bFlag;
}
 
//����ָ��IP,���ؼ�¼��ƫ��
ULONG CQQWry::FindIP(IN ULONG ulIP)
{
	ULONG			ulBegin=0;
	ULONG			ulEnd=m_ulRecordNum;
	ULONG			ulMid=0;
	ULONG			ulStartIP=0;	//������,��ʼIP
	ULONG			ulEndIP=0;		//��¼��,����IP
	ULONG			ulOffset=0;
 
 
	PrintfW( LEVEL_INFO,_T("[FindIP] uIP: %u,ulEnd:%u"),ulIP ,ulEnd);

	if( !IsFileOpened() || !IsDataLoaded() )
		return 0;

	//�����ṹ��7���ֽ�
	//ǰ4���ֽڱ�ʾһ����¼����ʼIP
	//��3���ֽڱ�ʾ������¼���ļ��еľ���ƫ��.
 
	//���ַ���������
	while(ulBegin < ulEnd-1)
	{
		ulMid = ulBegin + ( (ulEnd - ulBegin)/2 );
 
		PrintfW( LEVEL_INFO,_T("[FindIP] uIP: %u,ulMid:%u,m_pIndex:%u"),ulIP ,ulMid,m_pIndex );

		//������ǰ�ĸ��ֽھ�����ʼIP
		ulStartIP = * ( (ULONG*)( (BYTE*)m_pIndex +  ulMid * IPINFO_INDEX_SIZE ));
 
		//�ҵ���,����
		if (ulIP < ulStartIP)
		{
			ulEnd = ulMid;
		}
		else
		{	
			ulBegin = ulMid;	
		}			

		PrintfW( LEVEL_INFO,_T("[FindIP] uIP: %u,ulEnd:%u,ulMid:%u,ulStartIP: %u"),ulIP ,ulEnd,ulMid,ulStartIP);

	}
 
	PrintfW( LEVEL_INFO,_T("[FindIP] uIP: %u ready to copy record. Offset: %u"),ulIP ,(BYTE*)m_pIndex + ulBegin*IPINFO_INDEX_SIZE + sizeof(ULONG) );

	//ulOffset ���Ƕ�Ӧ�ļ�¼��
	memcpy(&ulOffset,(BYTE*)m_pIndex + ulBegin*IPINFO_INDEX_SIZE + sizeof(ULONG),3);
 
	//��¼�ṹ
	//����IP(4�ֽ�) + ������(������) + ������(������)
	//ƫ�Ƽ�¼�ĵ�����ֽ���1����2,�ͱ�ʾ��Ҫ�ض���,QueryIPA���д���
 
	PrintfW( LEVEL_INFO,_T("[FindIP] uIP: %u end copy record. ulOffset: %u"),ulIP ,ulOffset );

	//��¼��ǰ4���ֽھ��ǽ���IP
	ulEndIP = *( (ULONG*)((BYTE*)m_pDatBase + ulOffset));
 
	if (ulIP >  ulEndIP )
	{
		return 0;
	}
 
	PrintfW( LEVEL_INFO,_T("[FindIP] uIP: %u,offset: %u"),ulIP,ulOffset );

	return ulOffset;
}
 
#if 0
//���IP��Ч��
BOOL CQQWry::CheckIP(IN char* pszIP)
{	
	BOOL bReturnFlag=FALSE;
	int  a=0;
	int  b=0;
	int  c=0;
	int  d=0;
	int  e=0;
	int  nIPLen=0;
 
 
	do 
	{
		if ( (NULL == pszIP) || (0==strlen(pszIP)) )
		{
			break;
		}
 
		//255.255.255.255
		//0.0.0.0
		nIPLen = strlen(pszIP);
		if ( (7 > nIPLen) || (nIPLen > 15 ) )
		{
			break;
		}
 
		if ( 4 != sscanf(pszIP,"%d.%d.%d.%d%c",&a,&b,&c,&d,&e))
		{
			break;
		}
 
		if ( (0>a) || (a>255) )
		{
			break;
		}
 
		if ( (0>b) || (b>255) )
		{
			break;
		}
 
		if ( (0>c) || (c>255) )
		{
			break;
		}
 
		if ( (0>d) || (d>255) )
		{
			break;
		}
 
		bReturnFlag =TRUE;
	} while (FALSE);
 
	return bReturnFlag;
}
#endif