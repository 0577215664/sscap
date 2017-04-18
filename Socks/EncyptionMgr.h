#pragma once

/*!
 * @file EncyptionMgr.h
 * @date 2015/10/20 20:44
 *
 * @brief ���ܹ�����
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
#include <string>
#include <vector>

using namespace std;

class CCryptor;

struct __cryption_list 
{
	int idx;
	string name;
};

class CEncryptionMgr
{
public:
	CEncryptionMgr();
	virtual ~CEncryptionMgr();

public:
	static CCryptor *Create( string method, string key );
	static int GetEncyptionList( vector<string> &enc );
	/** @brief ��ֹchacha20�㷨
	* ��Ϊ�п��ܼ���chacha�Ŀ�: sodiumʧ��
	*/
	static void DisableChaCha20()
	{
		CEncryptionMgr::bChaCha20Disabled = true;
	}
	/** @brief ͨ���������ƻ�ȡ���ܵ�IDEX
	* 
	* @return ���ؼ������ƶ�Ӧ��INDEX,δ���Է���-1
	*/
	static int GetCryptionIndexByName( string name );
protected:
	static bool bChaCha20Disabled;
};