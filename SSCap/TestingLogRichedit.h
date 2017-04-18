#pragma once

/*!
 * @file TestingLogRichedit.h
 * @date 2015/06/18 14:32
 *
 * @brief ������ʾ�������LOG��Richedit.
 *
 * 
 * @author Taro
 * Contact: sockscap64@gmail.com
 *
 *
 * @version 1.0
 *
 * @History
 *
 * @note
*/
#include "AutoRichEditCtrl.h"

/** ���Դ���ʱ��һЩ������ʾ��Ϣ*/
#define TESTMSG_INFO 1				///< ��ͨ����Ϣ
#define TESTMSG_AFFIRMATIVE 2		///< �϶����ı�. (�ɹ�)
#define TESTMSG_NEGATIVE 3			///< �񶨵��ı�. (ʧ��)

class CAutoRichEditCtrl;
class CTestingLogRichedit : public CAutoRichEditCtrl
{
public:
	CTestingLogRichedit();
	~CTestingLogRichedit();

public:
	/** @brief ��ʼ��������ʾLOG��RICHEDIT. ��Ҫ�����弰�м��ĳ�ʼ��
	*/
	BOOL InitializeLogRichedit();
	/** @brief ���ʹ������LOG����������� ���ֽڸ�ʽ
	*/
	void AppendTestingLogA( LPCSTR lpMsg , int msg_type = TESTMSG_INFO);
	/** @brief ����һ��������Ե���Ϣ. version utf8
	*/
	void AppendTestingLogUTF8( LPCSTR lpMsg , int msg_type = TESTMSG_INFO);
	/** @brief ����һ��������Ե���Ϣ.
	*
	* @param strMsg Ҫ�������Ϣ.
	* @param msg_type ��Ϣ����
	*	- TESTMSG_INFO 1 ��ͨ��Ϣ
	*	- TESTMSG_AFFIRMATIVE 2 �϶�����Ϣ
	*	TESTMSG_NEGATIVE 3 �񶨵���Ϣ
	*/
	void AppendTestingLogW(CString strMsg, int msg_type = TESTMSG_INFO );
	/** @biref �����������LOG��ʱ���Ƿ������ǰ��ʱ��
	* ��ǰʱ������ĸ�ʽ��: ʱ:��:��
	*/
	void SetShowTimeInfo( BOOL bShow );
	/** @brief ���������ı���������
	*/
	void CopyTextToClipboard();
	/** @brief ��������ı�
	*/
	void ClearText();
protected:
	BOOL m_bShowTimeInfo;
};