#pragma once

/*!
 * @file proxynscache.h
 * @date 2015/05/27 8:48
 *
 * @brief ��Ҫ�����û�������һ��������Ϊ�����ַ�������, ��Ҫ���û�������������������ʵIP, Ȼ��CACHE����.
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
#include <map>
using namespace std;

#define PROXY_NS_CACHE_VALID_TIME 172800 ///< ������Чʱ��, Ĭ��48Сʱ

/** @brief ����������NS��¼����
*/
struct _PROXY_NS_CACHE_
{
	string szRealIPAddress; ///< ��ʵIP��ַ.
	time_t lastResolveTime;   ///< ��������IP��ʱ��
};


/** @brief ���ش���NS��¼�Ļ���
* 
* @param szWorkingDir ����Ŀ¼
*/
BOOL LoadProxyNSCache( TCHAR *szWorkingDir );
/** @brief �������NS�����¼���浽�ļ�
*/
void SaveProxyNSCache();
/** @brief ����ĳ��������NS��¼�����Ƿ����
*
* @param domain Ҫ���ҵ�����
* @return 
* - TRUE: ����
* - FALSE: ������
*/
BOOL IsProxyNSRecordExist( string domain );
/** @brief ���һ��������IP��ַ.
*
* @param[in] domain ����
* @param[out] ip ������IP
* @param[out] lastResolveTime ������������IP��ʱ��. ���ڵ�IP�������½���.( �����߸��� )
* @return 
* - TRUE: �ɹ�,  ip�з����˻�õ�IP
* - FALSE: ʧ��.
*/
BOOL GetProxyNSIPForDomain( string domain, string &ip );
/** @brief ��һ�������NS��¼ѹ�뻺��
* ������ھͻ��滻. �����ھ�ֱ�Ӳ���.
*/
void PushProxyNSCache( string domain , string ip);