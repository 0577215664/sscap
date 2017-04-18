#pragma once

class CRunConfig
{
public:
	CRunConfig(){}
	virtual ~CRunConfig(){}

public:
	static BOOL InitializeAppPath();
	static LPCTSTR GetAppWorkingDirectory();
	static LPCTSTR GetApplicationFullPathName();

protected:
	static TCHAR szApplicationFileName[1024 ];	///< ��ǰEXE�ļ�������·���ļ���.
	static TCHAR szProgramWorkingDirectory[ 1024 ];			///< SocksCap64��������Ŀ¼
};

void Init_Locale( const TCHAR *szWorkingDir );