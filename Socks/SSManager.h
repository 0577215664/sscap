#pragma once

/*!
 * @file SSManager.h
 * @date 2015/10/19 13:35
 *
 * @brief shadowsocks�Ĺ�����, ʵ�����е�ss�����޼�, ������, ֹͣ, �л�������.
 *
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
 * @note
*/
#include "BaseDef.h"
#include "Listener.h"

class CListener;
class CSSManager : public CListener
{
public:
	CSSManager();
	virtual ~CSSManager();

public:
	/** @brief ��ʼ����
	*/
	BOOL StartServices( );
	/** @brief ֹͣ����
	*/
	void StopServices();
public:
};
