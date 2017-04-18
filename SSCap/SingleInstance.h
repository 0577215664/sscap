#pragma once

/*!
 * @file SingleInstance.h
 * @date 2015/05/15 15:33
 *
 * @brief �����ʵ������. ֻ����һ����������һ��ʵ��.
 * ʵ��ԭ����ͨ��CreateMutex����һ�������Mutex�Դﵽ����̼���Ŀ��.
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
 * @TODO: long description
 *
 * @see https://support.microsoft.com/en-us/kb/243953
 * @note
 * Typical use instructions:
 *	#include "LimitSingleInstance.H"
 *  The one and only CLimitSingleInstance object.
 *  Change what is passed to constructor. GUIDGEN Tool may be of help.
 *  CLimitSingleInstance g_SingleInstanceObj(TEXT("Global\\{719967F0-DCC6-49b5-9C61-DE91175C3187}"));
 *
*/

#ifndef LimitSingleInstance_H
#define LimitSingleInstance_H

#include <windows.h> 

//This code is from Q243953 in case you lose the article and wonder
//where this code came from.
class CLimitSingleInstance
{
protected:
	DWORD  m_dwLastError;				///< ������.
	HANDLE m_hMutex;
	BOOL m_bIsAnotherInstanceRunning; ///< ������һ��ʵ��������ô?( TRUE / FALSE )

public:
	CLimitSingleInstance() 
		:m_dwLastError( 0 )
		, m_hMutex( NULL )
		, m_bIsAnotherInstanceRunning(FALSE)
	{
	}
	/**
	* @����һ��ȫ�ֵ�Mutex, ��ʵ�ֶ�ʵ���ļ��
	* The name can have a "Global\" or "Local\" prefix to explicitly create the object in the global or session namespace. The remainder of the name can contain any character except the backslash character (\). 
	*
	* @param[in] strMutexName MUTEXʵ������
	*/
	CLimitSingleInstance( TCHAR *strMutexName )
		: m_dwLastError( 0 )
		, m_hMutex( NULL )
		, m_bIsAnotherInstanceRunning(FALSE)
	{
		Create( strMutexName );
	}

	~CLimitSingleInstance() 
	{
		if (m_hMutex)  //Do not forget to close handles.
		{
			CloseHandle(m_hMutex); //Do as late as possible.
			m_hMutex = NULL; //Good habit to be in.
		}
	}
	void Create( TCHAR *strMutexName )
	{
		
		/**Make sure that you use a name that is unique for this application otherwise
		* two apps may think they are the same if they are using same name for
		*3rd parm to CreateMutex
		* 
		* If the function succeeds, the return value is a handle to the newly created mutex object.
		* If the function fails, the return value is NULL. To get extended error information, call GetLastError.
		* If the mutex is a named mutex and the object existed before this function call, the return value is a handle to the existing object, GetLastError returns  ERROR_ALREADY_EXISTS, bInitialOwner is ignored, and the calling thread is not granted ownership. However, if the caller has limited access rights, the function will fail with ERROR_ACCESS_DENIED and the caller should use the OpenMutex function.
		*/
		m_hMutex = CreateMutex(NULL, FALSE, strMutexName); //do early
		m_dwLastError = GetLastError(); //save for use later...

		/** 
		If the mutex is a named mutex and the object existed before this function call, the return value is a handle to the existing object, GetLastError returns ERROR_ALREADY_EXISTS, bInitialOwner is ignored, and the calling thread is not granted ownership. However, if the caller has limited access rights, the function will fail with ERROR_ACCESS_DENIED and the caller should use the OpenMutex function.
		https://msdn.microsoft.com/en-us/library/windows/desktop/ms682411%28v=vs.85%29.aspx
		*/
		if( m_dwLastError ==  ERROR_ACCESS_DENIED )
		{
			// LastError��ERROR_ACCESS_DENIED ��ʾ���MUTEX�Ѿ�����, ���ǵ�ǰû���㹻��Ȩ�޷�����. MSDN˵������OpenMutex����һ��.
			m_hMutex = OpenMutex( SYNCHRONIZE , FALSE, strMutexName );

			if( m_hMutex )
			{
				m_bIsAnotherInstanceRunning = TRUE;
			}
		}
		else if( m_dwLastError ==  ERROR_ALREADY_EXISTS )
		{
			m_bIsAnotherInstanceRunning = TRUE;
		}
	}

	BOOL IsAnotherInstanceRunning() 
	{
		return m_bIsAnotherInstanceRunning;
	}
};
#endif