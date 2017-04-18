#pragma once

/*!
 * @file SSLoader.h
 * @date 2015/10/25 10:48
 *
 * @brief shadowsocks�ڵ������
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
#include "JsonParser.h"
#include "BaseDef.h"
#include <assert.h>

class CSSLoader : public CJsonParser
{
public:
	CSSLoader(CSSConfigInfo *pNodeObj )
	{
		assert( pNodeObj != NULL );

		pSSNodeObj = pNodeObj;
	}

	virtual ~CSSLoader(){}

	BOOL LoadSS( LPCTSTR lpszWorkingDir );
	void SaveSS( LPCTSTR lpszWorkingDir );
	/** ����JSON��*/
	virtual BOOL ParseValueTree(Json::Value &value);

protected:
	BOOL GenerateJsonFromSSNodes( Json::Value &value );
protected:
	CSSConfigInfo *pSSNodeObj;
};