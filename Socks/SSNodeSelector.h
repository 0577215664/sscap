#pragma once

/*!
 * @file SSNodeSelector.h
 * @date 2015/11/01 12:13
 *
 * @brief Shadowsocks �ڵ�ѡ����. ����ݵ�ǰ��ѡ���㷨ѡ��һ��SS�ڵ����ʹ��
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
#include <string>

using namespace std;

struct _random_algorithm{
	int id;
	char *name;
};

class CSSNodeSelector
{
public:
	CSSNodeSelector();
	~CSSNodeSelector();

protected:
	static int nLastSelectedAlgorithm;
	/** @brief ��������
	*/
	static CSSNodeInfo *_UpToDown();
	static int GenerateSSNodeIdList( int **idlist );
	/* @brief ���
	*/
	//static CSSNodeInfo *_Random();
	/** @brief ���ӳ�����
	*/
	//static CSSNodeInfo *_LowLatancyFirst();
	/** @brief �ٴ�������
	*/
	//static CSSNodeInfo *_LessErrorsFirst();
public:
	/** @brief ���ݵ�ǰ�㷨( �Ƿ����÷��������� )ѡ��һ��SS�ڵ�ʹ��
	*/
	static CSSNodeInfo *SelectNode();
	/** @brief �������㷨�б�
	*/
	//static void GetRandomAlgorithms( vector<string> &algrothms );
};