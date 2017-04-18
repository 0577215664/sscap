/*!
 * <custome mutex>
 * 
 * Copyright (c) 2005 by <sockscap64@gmail.com>
 */
#ifndef _CRITICALSECTION_HEADER_FILE
#define _CRITICALSECTION_HEADER_FILE

#include <windows.h>

/** 
* \class CMyMutex
* �Զ���mutex��
*/
class CMyMutex
{
    public:
	CMyMutex(){
		InitializeCriticalSection(&cs);
	}
	~CMyMutex(){
		DeleteCriticalSection(&cs);
	}

public:
	void Lock(){
		EnterCriticalSection(&cs);
	}
	void UnLock(){
		LeaveCriticalSection(&cs);
	}
private:
	CRITICAL_SECTION cs;
};

/**
* ����Ϊ�˷�����.�������LOCK��LOCKȥ��.�鷳!
*/
class CMutexParser
{
public:
    CMutexParser(CMyMutex *mutex)
    {
        if(mutex)
        {
            gMutex = mutex;
            gMutex->Lock();
        }
    }
    ~CMutexParser()
    {
        if(gMutex)
            gMutex->UnLock();
    }
    CMyMutex *gMutex;
};
#endif